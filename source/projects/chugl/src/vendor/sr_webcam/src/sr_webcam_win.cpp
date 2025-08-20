#include "sr_webcam_internal.h"

#ifdef __cplusplus

#include <mfapi.h>
#include <windows.h>

#include <cmath>
#include <mferror.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <shlwapi.h>

struct IMFMediaType;
struct IMFActivate;
struct IMFMediaSource;
struct IMFAttributes;

namespace
{

// Context
class SRWebcamMFContext
{
  public:
    static SRWebcamMFContext& getContext()
    {
        static SRWebcamMFContext instance;
        return instance;
    }

    ~SRWebcamMFContext(void)
    {
        CoUninitialize();
    }

  private:
    SRWebcamMFContext(void)
    {
        CoInitialize(0);
        SUCCEEDED(MFStartup(MF_VERSION));
    }
};
} // namespace

struct SRWebcamFormat {

    SRWebcamFormat()
    {
        memset(&type, 0, sizeof(GUID));
    }

    SRWebcamFormat(IMFMediaType* pType)
    {
        memset(&type, 0, sizeof(GUID));

        // Extract the properties we need.
        UINT32 count = 0;
        if (SUCCEEDED(pType->GetCount(&count)) && SUCCEEDED(pType->LockStore())) {
            for (UINT32 i = 0; i < count; i++) {
                // Value of the property.
                PROPVARIANT var;
                PropVariantInit(&var);
                GUID guid = { 0 };
                if (!SUCCEEDED(pType->GetItemByIndex(i, &guid, &var))) {
                    continue;
                }
                // Extract the properties we need.
                if (guid == MF_MT_FRAME_RATE && var.vt == VT_UI8) {
                    UINT32 frameRateNum = 0, frameRateDenom = 0;
                    Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &frameRateNum,
                                          &frameRateDenom);
                    // Compute framerate.
                    if (frameRateDenom != 0) {
                        framerate = ((double)frameRateNum) / ((double)frameRateDenom);
                    }
                } else if (guid == MF_MT_SAMPLE_SIZE && var.vt == VT_UI4) {
                    sampleSize = var.ulVal;
                } else if (guid == MF_MT_MAJOR_TYPE && var.vt == VT_CLSID) {
                    type = *var.puuid;
                } else if (guid == MF_MT_FRAME_SIZE && var.vt == VT_UI8) {
                    Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &width, &height);
                }
                PropVariantClear(&var);
            }
            pType->UnlockStore();
        }
    }

    UINT32 width            = 0;
    UINT32 height           = 0;
    unsigned int sampleSize = 0;
    double framerate        = 0.0;
    GUID type;
};

class SRWebcamVideoStreamMF : public IMFSourceReaderCallback
{
  public:
    SRWebcamVideoStreamMF() : context(SRWebcamMFContext::getContext())
    {
    }

    virtual ~SRWebcamVideoStreamMF()
    {
        //
    }

    STDMETHODIMP
    QueryInterface(REFIID riid,
                   _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
    {
#pragma warning(push)
#pragma warning(disable : 4838)
        static const QITAB qit[] = {
            QITABENT(SRWebcamVideoStreamMF, IMFSourceReaderCallback),
            { 0 },
        };
#pragma warning(pop)
        return QISearch(this, qit, riid, ppvObject);
    };

    STDMETHODIMP_(ULONG)
    AddRef() override
    {
        return InterlockedIncrement(&refCount);
    }

    STDMETHODIMP_(ULONG)
    Release() override
    {
        ULONG uCount = InterlockedDecrement(&refCount);
        if (uCount == 0) {
            delete this;
        }
        return uCount;
    }

    STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*) override
    {
        return S_OK;
    }

    STDMETHODIMP OnFlush(DWORD) override
    {
        return S_OK;
    }

    STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                              DWORD dwStreamFlags, LONGLONG llTimestamp,
                              IMFSample* pSample) override
    {
        if (!_parent || !SUCCEEDED(hrStatus)) {
            return S_OK;
        }

        // double buffer for concurrent reader/writer
        static int dst_buffer_a_size        = 0;
        static int dst_buffer_b_size        = 0;
        static unsigned char* dst_buffer_a  = NULL;
        static unsigned char* dst_buffer_b  = NULL;
        static unsigned char** write_buffer = &dst_buffer_a;
        static int* write_buffer_size       = &dst_buffer_a_size;

        // Extract data and pass it.
        if (pSample != NULL) {
            IMFMediaBuffer* buffer = NULL;
            bool is2DLocked        = false;
            BYTE* ptr              = NULL;
            LONG pitch             = 0;
            DWORD maxSize = 0, curSize = 0;
            int exactRowSize      = captureFormat.width * 3;
            IMF2DBuffer* buffer2d = NULL;

            // Generate data buffer from sample.
            if (!SUCCEEDED(pSample->ConvertToContiguousBuffer(&buffer))) {
                // Try to get direct access to the buffer.
                DWORD bcnt = 0;
                if (!SUCCEEDED(pSample->GetBufferCount(&bcnt)) || bcnt == 0) {
                    return S_OK;
                }
                if (!SUCCEEDED(pSample->GetBufferByIndex(0, &buffer))) {
                    return S_OK;
                }
            }
            // Try to convert the buffer as-is. As in OpenCV, we use a lock2D if
            // possible.
            HRESULT res1 = buffer->QueryInterface(
              __uuidof(IMF2DBuffer),
              reinterpret_cast<void**>((IMFMediaBuffer**)&buffer2d));
            // Try to lock the buffer.
            if (res1 && SUCCEEDED(buffer2d->Lock2D(&ptr, &pitch))) {
                is2DLocked = true;
            }

            if (pitch == 0) {
                pitch = exactRowSize;
            }
            // If the 2D lock failed, try a regular one.
            if (!is2DLocked) {
                if (!SUCCEEDED(buffer->Lock(&ptr, &maxSize, &curSize))) {
                    if (buffer) {
                        buffer->Release();
                    }
                    if (buffer2d) {
                        buffer2d->Release();
                    }
                    return S_OK;
                }
            }
            // Maybe it still failed, skip.
            if (!ptr) {
                if (buffer) {
                    buffer->Release();
                }
                if (buffer2d) {
                    buffer2d->Release();
                }
                return S_OK;
            }
            // If the size of the buffer is not correct, skip.
            if (!is2DLocked && ((unsigned int)curSize != captureFormat.sampleSize)) {
                buffer->Unlock();
                if (buffer) {
                    buffer->Release();
                }
                if (buffer2d) {
                    buffer2d->Release();
                }
                return S_OK;
            }

            // flip buffers
            write_buffer
              = (write_buffer == &dst_buffer_a) ? &dst_buffer_b : &dst_buffer_a;
            write_buffer_size = (write_buffer_size == &dst_buffer_a_size) ?
                                  &dst_buffer_b_size :
                                  &dst_buffer_a_size;

            // resize
            int buffer_size = captureFormat.width * captureFormat.height * 4;
            if (*write_buffer_size != buffer_size) {
                *write_buffer_size = buffer_size;
                *write_buffer = (unsigned char*)realloc(*write_buffer, buffer_size);
            }

            // Convert from BGR (with stride) to compact RGBA
            // Copy each pixel and switch components, taking the pitch into account.
            int bytes_per_row = 4 * captureFormat.width;
            for (unsigned int y = 0; y < captureFormat.height; ++y) {
                for (unsigned int x = 0; x < captureFormat.width; ++x) {
                    (*write_buffer)[(captureFormat.height - 1 - y) * bytes_per_row
                                    + (captureFormat.width - 1 - x) * 4 + 0]
                      = ptr[y * pitch + 3 * x + 2];
                    (*write_buffer)[(captureFormat.height - 1 - y) * bytes_per_row
                                    + (captureFormat.width - 1 - x) * 4 + 1]
                      = ptr[y * pitch + 3 * x + 1];
                    (*write_buffer)[(captureFormat.height - 1 - y) * bytes_per_row
                                    + (captureFormat.width - 1 - x) * 4 + 2]
                      = ptr[y * pitch + 3 * x + 0];
                    (*write_buffer)[(captureFormat.height - 1 - y) * bytes_per_row
                                    + (captureFormat.width - 1 - x) * 4 + 3]
                      = 255;
                }
            }

            // Transmit data to user.
            _parent->callback(_parent, *write_buffer);
            // Release and clean.
            if (is2DLocked) {
                buffer2d->Unlock2D();
            } else {
                buffer->Unlock();
            }
            if (buffer) {
                buffer->Release();
            }
            if (buffer2d) {
                buffer2d->Release();
            }
        }

        // Schedule next sample.
        HRESULT res = videoReader->ReadSample(dwStreamIndex, 0, NULL, NULL, NULL, NULL);
        if (FAILED(res)) {
            // Scheduling failed, reached end of file, ...
            return S_OK;
        }
        return S_OK;
    }

    bool setupWith(int id, int framerate, int w, int h, char* device_name,
                   int device_name_len)
    {
        // Prepate video devices query.
        IMFAttributes* msAttr = NULL;
        if (!SUCCEEDED(MFCreateAttributes(&msAttr, 1))) {
            return false;
        }
        if (!SUCCEEDED(
              msAttr->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID))) {
            return false;
        }
        IMFActivate** ppDevices = NULL;
        UINT32 count            = 0;
        bool opened             = false;
        // Enumerate devices.
        if (!SUCCEEDED(MFEnumDeviceSources(msAttr, &ppDevices, &count)) || count == 0
            || id < 0) {
            CoTaskMemFree(ppDevices);
            return false;
        }
        _id = min((int)count - 1, id);
        // Release all the others.
        for (int i = 0; i < (int)count; ++i) {
            if (i == _id || !ppDevices[i]) {
                continue;
            }
            ppDevices[i]->Release();
        }
        // If the device is null, not available.
        if (!ppDevices[_id]) {
            CoTaskMemFree(ppDevices);
            return false;
        }

        // Set source reader parameters
        IMFMediaSource* mSrc  = NULL;
        IMFAttributes* srAttr = NULL;
        if (!SUCCEEDED(
              ppDevices[_id]->ActivateObject(__uuidof(IMFMediaSource), (void**)&mSrc))
            || !mSrc) {
            ppDevices[_id]->Release();
            CoTaskMemFree(ppDevices);
            return false;
        }
        // Create attributes.
        if (!SUCCEEDED(MFCreateAttributes(&srAttr, 6))) {
            ppDevices[_id]->Release();
            CoTaskMemFree(ppDevices);
            return false;
        }
        srAttr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
        srAttr->SetUINT32(MF_SOURCE_READER_DISABLE_DXVA, FALSE);
        srAttr->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, FALSE);
        srAttr->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE);
        // Define callback.
        HRESULT res = srAttr->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK,
                                         (IMFSourceReaderCallback*)this);
        if (FAILED(res)) {
            ppDevices[_id]->Release();
            CoTaskMemFree(ppDevices);
            return false;
        }
        // Create the reader from the attributes and device.
        if (!SUCCEEDED(
              MFCreateSourceReaderFromMediaSource(mSrc, srAttr, &videoReader))) {
            ppDevices[_id]->Release();
            CoTaskMemFree(ppDevices);
            return false;
        }

        // Iterate over streams and media types to find the best fit.
        HRESULT hr     = S_OK;
        int bestStream = -1;
        SRWebcamFormat bestFormat;
        float bestFit  = 1e9;
        DWORD streamId = 0;
        DWORD typeId   = 0;

        while (SUCCEEDED(hr)) {
            IMFMediaType* pType = NULL;
            hr = videoReader->GetNativeMediaType(streamId, typeId, &pType);
            // If we reached the end of format types for this stream, move to the next.
            if (hr == MF_E_NO_MORE_TYPES) {
                hr = S_OK;
                ++streamId;
                typeId = 0;
                continue;
            }
            // If hr is neither no_more_types nor success, we reached the end of the
            // streams list.
            if (!SUCCEEDED(hr)) {
                continue;
            }
            SRWebcamFormat format(pType);
            // We only care about video types.
            if (format.type != MFMediaType_Video) {
                ++typeId;
                continue;
            }
            // Init with the first available video format.
            if (bestStream < 0) {
                const float dw = (float)(w - format.width);
                const float dh = (float)(h - format.height);
                bestFit        = sqrtf(dw * dw + dh * dh);
                bestStream     = (int)streamId;
                bestFormat     = format;
                ++typeId;
                continue;
            }
            // If the current best already has the same size, replace it only if the
            // framerate is closer to the requested one.
            if (format.width == bestFormat.width
                && format.height == bestFormat.height) {
                if (abs(framerate - format.framerate)
                    < abs(framerate - bestFormat.framerate)) {
                    bestStream = (int)streamId;
                    bestFormat = format;
                }
                ++typeId;
                continue;
            }
            // Else, replace the format if its size is closer to the required one.
            const float dw  = (float)(w - format.width);
            const float dh  = (float)(h - format.height);
            const float fit = sqrtf(dw * dw + dh * dh);
            if (fit < bestFit) {
                bestFit    = fit;
                bestStream = (int)streamId;
                bestFormat = format;
            }
            ++typeId;
        }
        // If we didn't find anything, fail.
        if (bestStream < 0) {
            ppDevices[_id]->Release();
            CoTaskMemFree(ppDevices);
            return false;
        }
        // We found the best available stream and format, configure the output format.
        IMFMediaType* typeOut = NULL;
        if (!SUCCEEDED(MFCreateMediaType(&typeOut))) {
            ppDevices[_id]->Release();
            CoTaskMemFree(ppDevices);
            return false;
        }
        // We require RGB24, interlaced, at the best possible size and framerate.
        typeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        typeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24);
        typeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
        MFSetAttributeRatio(typeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
        MFSetAttributeSize(typeOut, MF_MT_FRAME_SIZE, bestFormat.width,
                           bestFormat.height);
        MFSetAttributeRatio(typeOut, MF_MT_FRAME_RATE,
                            min(framerate, (int)bestFormat.framerate), 1);
        typeOut->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, 1);
        typeOut->SetUINT32(MF_MT_SAMPLE_SIZE, 3 * bestFormat.width * bestFormat.height);
        typeOut->SetUINT32(MF_MT_DEFAULT_STRIDE, 3 * bestFormat.width);

        // Set the selected stream and the output format.
        if (!SUCCEEDED(videoReader->SetStreamSelection(
              (DWORD)MF_SOURCE_READER_ALL_STREAMS, false))
            || !SUCCEEDED(videoReader->SetStreamSelection((DWORD)bestStream, true))
            || !SUCCEEDED(
              videoReader->SetCurrentMediaType((DWORD)bestStream, NULL, typeOut))) {
            videoReader->Release();
            videoReader = NULL;
            ppDevices[_id]->Release();
            CoTaskMemFree(ppDevices);
            return false;
        }
        // Store infos for callback.
        selectedStream                = (DWORD)bestStream;
        captureFormat                 = SRWebcamFormat(typeOut);
        WCHAR* deviceFriendlyName     = NULL;
        UINT32 deviceFriendlyNameSize = 0;
        ppDevices[_id]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                                           &deviceFriendlyName,
                                           &deviceFriendlyNameSize);
        // copy device name
        if (device_name && deviceFriendlyName && deviceFriendlyNameSize > 0) {
            wcstombs(device_name, deviceFriendlyName, device_name_len);
        }
        // Clean up.
        if (deviceFriendlyName) {
            CoTaskMemFree(deviceFriendlyName);
        }
        ppDevices[_id]->Release();
        CoTaskMemFree(ppDevices);
        return true;
    }

    void start()
    {
        // Schedule first sample.
        if (FAILED(
              videoReader->ReadSample(selectedStream, 0, NULL, NULL, NULL, NULL))) {
            videoReader->Release();
            videoReader = NULL;
        }
    }

    void stop()
    {
        videoReader->Release();
        videoReader = NULL;
    }

  public:
    sr_webcam_device* _parent = NULL;
    int _id                   = -1;
    SRWebcamFormat captureFormat;

  private:
    SRWebcamMFContext& context;
    IMFSourceReader* videoReader;
    DWORD selectedStream;
    long refCount = 0;
};

int sr_webcam_open(sr_webcam_device* device)
{
    // Already setup.
    if (device->stream) {
        return -1;
    }
    SRWebcamVideoStreamMF* stream = new SRWebcamVideoStreamMF();
    stream->_parent               = device;
    bool res = stream->setupWith(device->deviceId, device->framerate, device->width,
                                 device->height, device->user_friendly_name,
                                 sizeof(device->user_friendly_name) - 1);
    if (!res) {
        device->stream = NULL;
        return -1;
    }
    device->stream    = stream;
    device->width     = stream->captureFormat.width;
    device->height    = stream->captureFormat.height;
    device->framerate = (int)(stream->captureFormat.framerate);
    device->deviceId  = stream->_id;
    return 0;
}

void sr_webcam_start(sr_webcam_device* device)
{
    if (device->stream && device->running == 0) {
        SRWebcamVideoStreamMF* stream = (SRWebcamVideoStreamMF*)(device->stream);
        stream->start();
        device->running = 1;
    }
}

void sr_webcam_stop(sr_webcam_device* device)
{
    if (device->stream && device->running == 1) {
        SRWebcamVideoStreamMF* stream = (SRWebcamVideoStreamMF*)(device->stream);
        stream->stop();
        device->running = 0;
    }
}

void sr_webcam_delete(sr_webcam_device* device)
{
    if (device->running == 1) {
        sr_webcam_stop(device);
    }

    // freeing here causes segfault in webcam callback thread (it accesses device ptr)
    // free(device);
}

#endif
