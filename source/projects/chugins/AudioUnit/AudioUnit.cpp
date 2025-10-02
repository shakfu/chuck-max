/*----------------------------------------------------------------------------
 ChucK AudioUnit Chugin

 Allows loading and using macOS AudioUnit plugins in ChucK.

 Copyright (c) 2025 CCRMA, Stanford University.  All rights reserved.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 U.S.A.
 -----------------------------------------------------------------------------*/

#include "chugin.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreMIDI/CoreMIDI.h>
#endif

// Forward declarations
CK_DLL_CTOR(audiounit_ctor);
CK_DLL_DTOR(audiounit_dtor);
CK_DLL_TICK(audiounit_tick);

CK_DLL_MFUN(audiounit_load);
CK_DLL_MFUN(audiounit_open);
CK_DLL_MFUN(audiounit_close);
CK_DLL_MFUN(audiounit_list);
CK_DLL_MFUN(audiounit_set_param);
CK_DLL_MFUN(audiounit_get_param);
CK_DLL_MFUN(audiounit_get_param_name);
CK_DLL_MFUN(audiounit_get_param_count);
CK_DLL_MFUN(audiounit_set_preset);
CK_DLL_MFUN(audiounit_get_preset);
CK_DLL_MFUN(audiounit_get_preset_count);
CK_DLL_MFUN(audiounit_bypass);
CK_DLL_MFUN(audiounit_send_midi);
CK_DLL_MFUN(audiounit_note_on);
CK_DLL_MFUN(audiounit_note_off);
CK_DLL_MFUN(audiounit_control_change);
CK_DLL_MFUN(audiounit_program_change);
CK_DLL_MFUN(audiounit_is_music_device);
CK_DLL_MFUN(audiounit_get_midi_device);

t_CKINT audiounit_data_offset = 0;

#ifdef __APPLE__

class AudioUnitWrapper
{
public:
    AudioUnitWrapper(t_CKFLOAT sampleRate)
        : m_audioUnit(NULL)
        , m_sampleRate(sampleRate)
        , m_numInputs(1)
        , m_numOutputs(1)
        , m_bypass(false)
        , m_midiClient(0)
        , m_midiDestination(0)
        , m_sampleTime(0)
        , m_bufferList(NULL)
        , m_currentInput(0.0)
    {
        // Allocate buffers for non-interleaved stereo
        // Non-interleaved means separate buffers for left and right channels
        m_outputBufferData[0] = (float*)calloc(1, sizeof(float));
        m_outputBufferData[1] = (float*)calloc(1, sizeof(float));

        // Allocate AudioBufferList with space for 2 buffers
        m_bufferList = (AudioBufferList*)malloc(offsetof(AudioBufferList, mBuffers) + 2 * sizeof(AudioBuffer));
        m_bufferList->mNumberBuffers = 2;  // 2 buffers for stereo non-interleaved
        m_bufferList->mBuffers[0].mNumberChannels = 1;
        m_bufferList->mBuffers[0].mDataByteSize = sizeof(float);
        m_bufferList->mBuffers[0].mData = m_outputBufferData[0];

        m_bufferList->mBuffers[1].mNumberChannels = 1;
        m_bufferList->mBuffers[1].mDataByteSize = sizeof(float);
        m_bufferList->mBuffers[1].mData = m_outputBufferData[1];
    }

    ~AudioUnitWrapper()
    {
        close();
        if(m_outputBufferData[0]) free(m_outputBufferData[0]);
        if(m_outputBufferData[1]) free(m_outputBufferData[1]);
        if(m_bufferList) free(m_bufferList);
    }

    bool load(const char* name)
    {
        // Parse the name to extract type, subtype, and manufacturer
        // Format: "type:subtype:manufacturer" or "name"
        // For now, we'll support common formats

        if(strstr(name, "aufx") || strstr(name, "aumu") || strstr(name, "aumf"))
        {
            // Four-char code format
            return loadByFourCharCode(name);
        }
        else
        {
            // Search by name
            return loadByName(name);
        }
    }

    bool open(UInt32 type, UInt32 subType, UInt32 manufacturer)
    {
        close();

        AudioComponentDescription desc;
        desc.componentType = type;
        desc.componentSubType = subType;
        desc.componentManufacturer = manufacturer;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;

        AudioComponent component = AudioComponentFindNext(NULL, &desc);
        if(!component)
        {
            fprintf(stderr, "[AudioUnit]: Could not find AudioUnit component\n");
            return false;
        }

        OSStatus status = AudioComponentInstanceNew(component, &m_audioUnit);
        if(status != noErr)
        {
            fprintf(stderr, "[AudioUnit]: Could not instantiate AudioUnit (error %d)\n", (int)status);
            return false;
        }

        // Get component info
        AudioComponentDescription foundDesc;
        AudioComponentGetDescription(component, &foundDesc);
        m_componentType = foundDesc.componentType;

        // Set up stream format
        AudioStreamBasicDescription streamFormat;
        streamFormat.mSampleRate = m_sampleRate;
        streamFormat.mFormatID = kAudioFormatLinearPCM;
        streamFormat.mFormatFlags = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
        streamFormat.mBytesPerPacket = sizeof(Float32);
        streamFormat.mFramesPerPacket = 1;
        streamFormat.mBytesPerFrame = sizeof(Float32);
        streamFormat.mChannelsPerFrame = 2;
        streamFormat.mBitsPerChannel = 32;

        // Set format for output
        status = AudioUnitSetProperty(m_audioUnit,
                                     kAudioUnitProperty_StreamFormat,
                                     kAudioUnitScope_Output,
                                     0,
                                     &streamFormat,
                                     sizeof(streamFormat));

        // Set format for input (for effects only, not for MusicDevice)
        if(m_componentType != kAudioUnitType_MusicDevice &&
           m_componentType != kAudioUnitType_Generator)
        {
            AudioUnitSetProperty(m_audioUnit,
                               kAudioUnitProperty_StreamFormat,
                               kAudioUnitScope_Input,
                               0,
                               &streamFormat,
                               sizeof(streamFormat));
        }

        // Set maximum frames per slice
        UInt32 maxFrames = 1;
        AudioUnitSetProperty(m_audioUnit,
                           kAudioUnitProperty_MaximumFramesPerSlice,
                           kAudioUnitScope_Global,
                           0,
                           &maxFrames,
                           sizeof(maxFrames));

        // Set up render callback to provide input for all AudioUnit types
        AURenderCallbackStruct callbackStruct;
        callbackStruct.inputProc = renderCallback;
        callbackStruct.inputProcRefCon = this;

        AudioUnitSetProperty(m_audioUnit,
                           kAudioUnitProperty_SetRenderCallback,
                           kAudioUnitScope_Input,
                           0,
                           &callbackStruct,
                           sizeof(callbackStruct));

        // Initialize
        status = AudioUnitInitialize(m_audioUnit);
        if(status != noErr)
        {
            fprintf(stderr, "[AudioUnit]: Could not initialize AudioUnit (error %d)\n", (int)status);
            AudioComponentInstanceDispose(m_audioUnit);
            m_audioUnit = NULL;
            return false;
        }

        // Cache parameter info
        cacheParameters();

        // Create virtual MIDI destination if this is a MusicDevice
        if(m_componentType == kAudioUnitType_MusicDevice)
        {
            // Get the component name
            CFStringRef name = NULL;
            AudioComponentCopyName(component, &name);
            char nameBuffer[256] = "Unknown";
            if(name)
            {
                CFStringGetCString(name, nameBuffer, sizeof(nameBuffer), kCFStringEncodingUTF8);
                CFRelease(name);
            }

            createVirtualMIDIDestination(nameBuffer);
        }

        return true;
    }

    void close()
    {
        destroyVirtualMIDIDestination();

        if(m_audioUnit)
        {
            AudioUnitUninitialize(m_audioUnit);
            AudioComponentInstanceDispose(m_audioUnit);
            m_audioUnit = NULL;
        }
        m_parameters.clear();
    }

    SAMPLE tick(SAMPLE input)
    {
        if(!m_audioUnit || m_bypass)
            return input;

        // Store input for the render callback
        m_currentInput = input;

        // Render
        AudioTimeStamp timeStamp;
        memset(&timeStamp, 0, sizeof(AudioTimeStamp));
        timeStamp.mFlags = kAudioTimeStampSampleTimeValid;
        timeStamp.mSampleTime = m_sampleTime++;

        UInt32 frameCount = 1;
        AudioUnitRenderActionFlags flags = 0;

        OSStatus status = AudioUnitRender(m_audioUnit,
                                        &flags,
                                        &timeStamp,
                                        0,  // output bus
                                        frameCount,
                                        m_bufferList);

        if(status != noErr)
        {
            static bool errorLogged = false;
            if(!errorLogged) {
                fprintf(stderr, "[AudioUnit]: Render error %d (type=%c%c%c%c)\n",
                       (int)status,
                       (char)(m_componentType >> 24),
                       (char)(m_componentType >> 16),
                       (char)(m_componentType >> 8),
                       (char)m_componentType);
                errorLogged = true;
            }
            // For MusicDevice, even on error, return silence rather than input
            if(m_componentType == kAudioUnitType_MusicDevice)
                return 0.0;
            return input;
        }

        // Get output (mono from left channel)
        return m_outputBufferData[0][0];
    }

    bool setParameter(t_CKINT index, t_CKFLOAT value)
    {
        if(!m_audioUnit || index < 0 || index >= m_parameters.size())
            return false;

        AudioUnitParameterID paramID = m_parameters[index].paramID;
        AudioUnitSetParameter(m_audioUnit,
                            paramID,
                            kAudioUnitScope_Global,
                            0,
                            value,
                            0);
        return true;
    }

    t_CKFLOAT getParameter(t_CKINT index)
    {
        if(!m_audioUnit || index < 0 || index >= m_parameters.size())
            return 0.0;

        AudioUnitParameterID paramID = m_parameters[index].paramID;
        AudioUnitParameterValue value;
        AudioUnitGetParameter(m_audioUnit,
                            paramID,
                            kAudioUnitScope_Global,
                            0,
                            &value);
        return value;
    }

    const char* getParameterName(t_CKINT index)
    {
        if(index < 0 || index >= m_parameters.size())
            return "";
        return m_parameters[index].name.c_str();
    }

    t_CKINT getParameterCount()
    {
        return m_parameters.size();
    }

    void setBypass(bool bypass)
    {
        m_bypass = bypass;
        if(m_audioUnit)
        {
            UInt32 bypassValue = bypass ? 1 : 0;
            AudioUnitSetProperty(m_audioUnit,
                               kAudioUnitProperty_BypassEffect,
                               kAudioUnitScope_Global,
                               0,
                               &bypassValue,
                               sizeof(bypassValue));
        }
    }

    // MIDI methods
    bool sendMIDI(t_CKINT status, t_CKINT data1, t_CKINT data2)
    {
        if(!m_audioUnit || !isMusicDevice())
            return false;

        OSStatus result = MusicDeviceMIDIEvent(m_audioUnit,
                                              (UInt32)status,
                                              (UInt32)data1,
                                              (UInt32)data2,
                                              0);  // offsetSampleFrame = 0 for immediate
        return (result == noErr);
    }

    bool noteOn(t_CKINT pitch, t_CKINT velocity)
    {
        // MIDI note on: 0x90 (note on, channel 0)
        return sendMIDI(0x90, pitch, velocity);
    }

    bool noteOff(t_CKINT pitch)
    {
        // MIDI note off: 0x80 (note off, channel 0)
        return sendMIDI(0x80, pitch, 0);
    }

    bool controlChange(t_CKINT cc, t_CKINT value)
    {
        // MIDI control change: 0xB0 (CC, channel 0)
        return sendMIDI(0xB0, cc, value);
    }

    bool programChange(t_CKINT program)
    {
        // MIDI program change: 0xC0 (program change, channel 0)
        return sendMIDI(0xC0, program, 0);
    }

    bool isMusicDevice() const
    {
        return m_audioUnit && m_componentType == kAudioUnitType_MusicDevice;
    }

    const char* getMIDIDeviceName() const
    {
        return m_midiDestinationName.c_str();
    }

    static void listAudioUnits()
    {
        AudioComponentDescription desc;
        desc.componentType = 0;
        desc.componentSubType = 0;
        desc.componentManufacturer = 0;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;

        AudioComponent component = NULL;
        int count = 0;

        fprintf(stderr, "\n[AudioUnit]: Available AudioUnits:\n");
        fprintf(stderr, "----------------------------------------\n");

        while((component = AudioComponentFindNext(component, &desc)))
        {
            AudioComponentDescription foundDesc;
            AudioComponentGetDescription(component, &foundDesc);

            CFStringRef name = NULL;
            AudioComponentCopyName(component, &name);

            char nameBuffer[256];
            if(name)
            {
                CFStringGetCString(name, nameBuffer, sizeof(nameBuffer), kCFStringEncodingUTF8);
                CFRelease(name);
            }
            else
            {
                strcpy(nameBuffer, "Unknown");
            }

            const char* typeStr = "Unknown";
            if(foundDesc.componentType == kAudioUnitType_Effect)
                typeStr = "Effect";
            else if(foundDesc.componentType == kAudioUnitType_MusicEffect)
                typeStr = "MusicEffect";
            else if(foundDesc.componentType == kAudioUnitType_MusicDevice)
                typeStr = "Instrument";
            else if(foundDesc.componentType == kAudioUnitType_Generator)
                typeStr = "Generator";
            else if(foundDesc.componentType == kAudioUnitType_Mixer)
                typeStr = "Mixer";

            fprintf(stderr, "%3d. [%s] %s\n", ++count, typeStr, nameBuffer);
            fprintf(stderr, "     Type: '%c%c%c%c' SubType: '%c%c%c%c' Mfr: '%c%c%c%c'\n",
                   (char)(foundDesc.componentType >> 24),
                   (char)(foundDesc.componentType >> 16),
                   (char)(foundDesc.componentType >> 8),
                   (char)foundDesc.componentType,
                   (char)(foundDesc.componentSubType >> 24),
                   (char)(foundDesc.componentSubType >> 16),
                   (char)(foundDesc.componentSubType >> 8),
                   (char)foundDesc.componentSubType,
                   (char)(foundDesc.componentManufacturer >> 24),
                   (char)(foundDesc.componentManufacturer >> 16),
                   (char)(foundDesc.componentManufacturer >> 8),
                   (char)foundDesc.componentManufacturer);
        }

        fprintf(stderr, "----------------------------------------\n");
        fprintf(stderr, "Total: %d AudioUnits\n\n", count);
    }

private:
    struct ParameterInfo
    {
        AudioUnitParameterID paramID;
        std::string name;
    };

    bool loadByFourCharCode(const char* str)
    {
        // Parse format like "aufx:dely:appl" (type:subtype:manufacturer)
        char typeStr[5] = {0}, subTypeStr[5] = {0}, mfrStr[5] = {0};

        if(sscanf(str, "%4s:%4s:%4s", typeStr, subTypeStr, mfrStr) == 3)
        {
            UInt32 type = *(UInt32*)typeStr;
            UInt32 subType = *(UInt32*)subTypeStr;
            UInt32 mfr = *(UInt32*)mfrStr;

            return open(type, subType, mfr);
        }

        return false;
    }

    bool loadByName(const char* name)
    {
        AudioComponentDescription desc;
        desc.componentType = 0;
        desc.componentSubType = 0;
        desc.componentManufacturer = 0;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;

        AudioComponent component = NULL;

        while((component = AudioComponentFindNext(component, &desc)))
        {
            CFStringRef cfName = NULL;
            AudioComponentCopyName(component, &cfName);

            if(cfName)
            {
                char nameBuffer[256];
                CFStringGetCString(cfName, nameBuffer, sizeof(nameBuffer), kCFStringEncodingUTF8);
                CFRelease(cfName);

                if(strcasestr(nameBuffer, name))
                {
                    AudioComponentDescription foundDesc;
                    AudioComponentGetDescription(component, &foundDesc);
                    return open(foundDesc.componentType,
                              foundDesc.componentSubType,
                              foundDesc.componentManufacturer);
                }
            }
        }

        fprintf(stderr, "[AudioUnit]: Could not find AudioUnit named '%s'\n", name);
        return false;
    }

    void cacheParameters()
    {
        m_parameters.clear();

        if(!m_audioUnit)
            return;

        UInt32 paramListSize = 0;
        OSStatus status = AudioUnitGetPropertyInfo(m_audioUnit,
                                                  kAudioUnitProperty_ParameterList,
                                                  kAudioUnitScope_Global,
                                                  0,
                                                  &paramListSize,
                                                  NULL);

        if(status != noErr || paramListSize == 0)
            return;

        int numParams = paramListSize / sizeof(AudioUnitParameterID);
        AudioUnitParameterID* paramList = (AudioUnitParameterID*)malloc(paramListSize);

        status = AudioUnitGetProperty(m_audioUnit,
                                     kAudioUnitProperty_ParameterList,
                                     kAudioUnitScope_Global,
                                     0,
                                     paramList,
                                     &paramListSize);

        if(status == noErr)
        {
            for(int i = 0; i < numParams; i++)
            {
                AudioUnitParameterInfo paramInfo;
                UInt32 paramInfoSize = sizeof(paramInfo);

                status = AudioUnitGetProperty(m_audioUnit,
                                            kAudioUnitProperty_ParameterInfo,
                                            kAudioUnitScope_Global,
                                            paramList[i],
                                            &paramInfo,
                                            &paramInfoSize);

                if(status == noErr)
                {
                    ParameterInfo info;
                    info.paramID = paramList[i];

                    if(paramInfo.flags & kAudioUnitParameterFlag_HasCFNameString)
                    {
                        char nameBuffer[256];
                        CFStringGetCString(paramInfo.cfNameString,
                                         nameBuffer,
                                         sizeof(nameBuffer),
                                         kCFStringEncodingUTF8);
                        info.name = nameBuffer;
                        CFRelease(paramInfo.cfNameString);
                    }
                    else if(paramInfo.name[0] != 0)
                    {
                        info.name = paramInfo.name;
                    }
                    else
                    {
                        char buf[32];
                        snprintf(buf, sizeof(buf), "Param %d", i);
                        info.name = buf;
                    }

                    m_parameters.push_back(info);
                }
            }
        }

        free(paramList);
    }

    // Render callback - provides input for AudioUnit rendering
    static OSStatus renderCallback(void *inRefCon,
                                   AudioUnitRenderActionFlags *ioActionFlags,
                                   const AudioTimeStamp *inTimeStamp,
                                   UInt32 inBusNumber,
                                   UInt32 inNumberFrames,
                                   AudioBufferList *ioData)
    {
        AudioUnitWrapper* wrapper = (AudioUnitWrapper*)inRefCon;

        if(ioData && wrapper)
        {
            // For effects, provide the current input sample
            // For MusicDevice, provide silent input
            SAMPLE inputValue = (wrapper->m_componentType == kAudioUnitType_MusicDevice ||
                                wrapper->m_componentType == kAudioUnitType_Generator)
                                ? 0.0 : wrapper->m_currentInput;

            for(UInt32 i = 0; i < ioData->mNumberBuffers; i++)
            {
                float* buffer = (float*)ioData->mBuffers[i].mData;
                for(UInt32 j = 0; j < inNumberFrames; j++)
                {
                    buffer[j] = inputValue;
                }
            }
        }
        return noErr;
    }

    // MIDI callback - called when MIDI data is received
    static void midiReadCallback(const MIDIPacketList *pktlist,
                                void *refCon,
                                void *connRefCon)
    {
        AudioUnitWrapper* wrapper = (AudioUnitWrapper*)refCon;
        if(!wrapper || !wrapper->m_audioUnit)
            return;

        const MIDIPacket *packet = &pktlist->packet[0];
        for(UInt32 i = 0; i < pktlist->numPackets; i++)
        {
            // Parse MIDI message
            if(packet->length >= 1)
            {
                UInt8 status = packet->data[0];
                UInt8 data1 = packet->length >= 2 ? packet->data[1] : 0;
                UInt8 data2 = packet->length >= 3 ? packet->data[2] : 0;

                // Forward to AudioUnit
                wrapper->sendMIDI(status, data1, data2);
            }

            packet = MIDIPacketNext(packet);
        }
    }

    void createVirtualMIDIDestination(const char* auName)
    {
        destroyVirtualMIDIDestination();

        // Create MIDI client
        CFStringRef clientName = CFStringCreateWithFormat(NULL, NULL,
            CFSTR("ChucK AudioUnit MIDI Client"));
        OSStatus status = MIDIClientCreate(clientName, NULL, NULL, &m_midiClient);
        CFRelease(clientName);

        if(status != noErr)
        {
            fprintf(stderr, "[AudioUnit]: Failed to create MIDI client (error %d)\n", (int)status);
            return;
        }

        // Create virtual destination
        CFStringRef destName = CFStringCreateWithFormat(NULL, NULL,
            CFSTR("ChucK AudioUnit: %s"), auName);
        status = MIDIDestinationCreate(m_midiClient,
                                       destName,
                                       midiReadCallback,
                                       this,
                                       &m_midiDestination);

        if(status == noErr)
        {
            char nameBuf[256];
            CFStringGetCString(destName, nameBuf, sizeof(nameBuf), kCFStringEncodingUTF8);
            m_midiDestinationName = nameBuf;
            fprintf(stderr, "[AudioUnit]: Created virtual MIDI destination: %s\n", nameBuf);
        }
        else
        {
            fprintf(stderr, "[AudioUnit]: Failed to create MIDI destination (error %d)\n", (int)status);
            MIDIClientDispose(m_midiClient);
            m_midiClient = 0;
        }

        CFRelease(destName);
    }

    void destroyVirtualMIDIDestination()
    {
        if(m_midiDestination)
        {
            MIDIEndpointDispose(m_midiDestination);
            m_midiDestination = 0;
        }

        if(m_midiClient)
        {
            MIDIClientDispose(m_midiClient);
            m_midiClient = 0;
        }

        m_midiDestinationName.clear();
    }

    AudioUnit m_audioUnit;
    t_CKFLOAT m_sampleRate;
    UInt32 m_componentType;
    int m_numInputs;
    int m_numOutputs;
    bool m_bypass;

    AudioBufferList* m_bufferList;  // Dynamically allocated for multiple buffers
    float* m_outputBufferData[2];  // Separate buffers for non-interleaved stereo

    std::vector<ParameterInfo> m_parameters;

    // MIDI support
    MIDIClientRef m_midiClient;
    MIDIEndpointRef m_midiDestination;
    std::string m_midiDestinationName;

    // Sample time counter for rendering
    UInt64 m_sampleTime;

    // Current input sample for effects processing
    SAMPLE m_currentInput;
};

#else // !__APPLE__

// Stub implementation for non-macOS platforms
class AudioUnitWrapper
{
public:
    AudioUnitWrapper(t_CKFLOAT sampleRate) {}
    ~AudioUnitWrapper() {}
    bool load(const char* name) { return false; }
    bool open(UInt32 type, UInt32 subType, UInt32 manufacturer) { return false; }
    void close() {}
    SAMPLE tick(SAMPLE input) { return input; }
    bool setParameter(t_CKINT index, t_CKFLOAT value) { return false; }
    t_CKFLOAT getParameter(t_CKINT index) { return 0.0; }
    const char* getParameterName(t_CKINT index) { return ""; }
    t_CKINT getParameterCount() { return 0; }
    void setBypass(bool bypass) {}
    bool sendMIDI(t_CKINT status, t_CKINT data1, t_CKINT data2) { return false; }
    bool noteOn(t_CKINT pitch, t_CKINT velocity) { return false; }
    bool noteOff(t_CKINT pitch) { return false; }
    bool controlChange(t_CKINT cc, t_CKINT value) { return false; }
    bool programChange(t_CKINT program) { return false; }
    bool isMusicDevice() const { return false; }
    const char* getMIDIDeviceName() const { return ""; }
    static void listAudioUnits() {
        fprintf(stderr, "[AudioUnit]: AudioUnits are only available on macOS\n");
    }
};

#endif // __APPLE__

// ChucK DLL Query
CK_DLL_QUERY(AudioUnit)
{
    QUERY->setname(QUERY, "AudioUnit");

    QUERY->begin_class(QUERY, "AudioUnit", "UGen");
    QUERY->doc_class(QUERY, "Load and use macOS AudioUnit plugins in ChucK. "
                            "AudioUnits can be effects, instruments, or generators. "
                            "This chugin is macOS-only.");
    QUERY->add_ex(QUERY, "effects/AudioUnit.ck");

    QUERY->add_ctor(QUERY, audiounit_ctor);
    QUERY->add_dtor(QUERY, audiounit_dtor);

    QUERY->add_ugen_func(QUERY, audiounit_tick, NULL, 1, 1);

    QUERY->add_mfun(QUERY, audiounit_load, "int", "load");
    QUERY->add_arg(QUERY, "string", "name");
    QUERY->doc_func(QUERY, "Load an AudioUnit by name or four-char code (e.g., 'aufx:dely:appl'). "
                          "Returns 1 on success, 0 on failure.");

    QUERY->add_mfun(QUERY, audiounit_open, "int", "open");
    QUERY->add_arg(QUERY, "string", "type");
    QUERY->add_arg(QUERY, "string", "subtype");
    QUERY->add_arg(QUERY, "string", "manufacturer");
    QUERY->doc_func(QUERY, "Open an AudioUnit by four-char codes. "
                          "Returns 1 on success, 0 on failure.");

    QUERY->add_mfun(QUERY, audiounit_close, "void", "close");
    QUERY->doc_func(QUERY, "Close the currently loaded AudioUnit.");

    QUERY->add_mfun(QUERY, audiounit_list, "void", "list");
    QUERY->doc_func(QUERY, "List all available AudioUnits on the system.");

    QUERY->add_mfun(QUERY, audiounit_set_param, "void", "setParam");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->add_arg(QUERY, "float", "value");
    QUERY->doc_func(QUERY, "Set a parameter value by index.");

    QUERY->add_mfun(QUERY, audiounit_get_param, "float", "getParam");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->doc_func(QUERY, "Get a parameter value by index.");

    QUERY->add_mfun(QUERY, audiounit_get_param_name, "string", "paramName");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->doc_func(QUERY, "Get a parameter name by index.");

    QUERY->add_mfun(QUERY, audiounit_get_param_count, "int", "paramCount");
    QUERY->doc_func(QUERY, "Get the number of parameters available.");

    QUERY->add_mfun(QUERY, audiounit_bypass, "void", "bypass");
    QUERY->add_arg(QUERY, "int", "bypass");
    QUERY->doc_func(QUERY, "Bypass the AudioUnit (1 = bypass, 0 = active).");

    // MIDI methods
    QUERY->add_mfun(QUERY, audiounit_send_midi, "int", "sendMIDI");
    QUERY->add_arg(QUERY, "int", "status");
    QUERY->add_arg(QUERY, "int", "data1");
    QUERY->add_arg(QUERY, "int", "data2");
    QUERY->doc_func(QUERY, "Send raw MIDI message to AudioUnit (for MusicDevice types). Returns 1 on success.");

    QUERY->add_mfun(QUERY, audiounit_note_on, "int", "noteOn");
    QUERY->add_arg(QUERY, "int", "pitch");
    QUERY->add_arg(QUERY, "int", "velocity");
    QUERY->doc_func(QUERY, "Send MIDI note-on message (channel 0). Returns 1 on success.");

    QUERY->add_mfun(QUERY, audiounit_note_off, "int", "noteOff");
    QUERY->add_arg(QUERY, "int", "pitch");
    QUERY->doc_func(QUERY, "Send MIDI note-off message (channel 0). Returns 1 on success.");

    QUERY->add_mfun(QUERY, audiounit_control_change, "int", "controlChange");
    QUERY->add_arg(QUERY, "int", "cc");
    QUERY->add_arg(QUERY, "int", "value");
    QUERY->doc_func(QUERY, "Send MIDI control change message (channel 0). Returns 1 on success.");

    QUERY->add_mfun(QUERY, audiounit_program_change, "int", "programChange");
    QUERY->add_arg(QUERY, "int", "program");
    QUERY->doc_func(QUERY, "Send MIDI program change message (channel 0). Returns 1 on success.");

    QUERY->add_mfun(QUERY, audiounit_is_music_device, "int", "isMusicDevice");
    QUERY->doc_func(QUERY, "Check if loaded AudioUnit is a MusicDevice (instrument). Returns 1 if true.");

    QUERY->add_mfun(QUERY, audiounit_get_midi_device, "string", "getMIDIDeviceName");
    QUERY->doc_func(QUERY, "Get the name of the virtual MIDI destination (if MusicDevice).");

    audiounit_data_offset = QUERY->add_mvar(QUERY, "int", "@au_data", false);

    QUERY->end_class(QUERY);

    return TRUE;
}

// Implementation
CK_DLL_CTOR(audiounit_ctor)
{
    OBJ_MEMBER_INT(SELF, audiounit_data_offset) = 0;

    AudioUnitWrapper* wrapper = new AudioUnitWrapper(API->vm->srate(VM));

    OBJ_MEMBER_INT(SELF, audiounit_data_offset) = (t_CKINT)wrapper;
}

CK_DLL_DTOR(audiounit_dtor)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    if(wrapper)
    {
        delete wrapper;
        OBJ_MEMBER_INT(SELF, audiounit_data_offset) = 0;
    }
}

CK_DLL_TICK(audiounit_tick)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);

    if(wrapper)
        *out = wrapper->tick(in);
    else
        *out = in;

    return TRUE;
}

CK_DLL_MFUN(audiounit_load)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    std::string name = GET_NEXT_STRING_SAFE(ARGS);

    RETURN->v_int = wrapper ? wrapper->load(name.c_str()) : 0;
}

CK_DLL_MFUN(audiounit_open)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    std::string type = GET_NEXT_STRING_SAFE(ARGS);
    std::string subtype = GET_NEXT_STRING_SAFE(ARGS);
    std::string mfr = GET_NEXT_STRING_SAFE(ARGS);

#ifdef __APPLE__
    if(wrapper && type.length() == 4 && subtype.length() == 4 && mfr.length() == 4)
    {
        UInt32 typeCode = *(UInt32*)type.c_str();
        UInt32 subtypeCode = *(UInt32*)subtype.c_str();
        UInt32 mfrCode = *(UInt32*)mfr.c_str();

        RETURN->v_int = wrapper->open(typeCode, subtypeCode, mfrCode);
    }
    else
#endif
    {
        RETURN->v_int = 0;
    }
}

CK_DLL_MFUN(audiounit_close)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    if(wrapper)
        wrapper->close();
}

CK_DLL_MFUN(audiounit_list)
{
    AudioUnitWrapper::listAudioUnits();
}

CK_DLL_MFUN(audiounit_set_param)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT index = GET_NEXT_INT(ARGS);
    t_CKFLOAT value = GET_NEXT_FLOAT(ARGS);

    if(wrapper)
        wrapper->setParameter(index, value);
}

CK_DLL_MFUN(audiounit_get_param)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT index = GET_NEXT_INT(ARGS);

    RETURN->v_float = wrapper ? wrapper->getParameter(index) : 0.0;
}

CK_DLL_MFUN(audiounit_get_param_name)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT index = GET_NEXT_INT(ARGS);

    const char* name = wrapper ? wrapper->getParameterName(index) : "";
    RETURN->v_string = API->object->create_string(VM, name, false);
}

CK_DLL_MFUN(audiounit_get_param_count)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    RETURN->v_int = wrapper ? wrapper->getParameterCount() : 0;
}

CK_DLL_MFUN(audiounit_bypass)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT bypass = GET_NEXT_INT(ARGS);

    if(wrapper)
        wrapper->setBypass(bypass != 0);
}

CK_DLL_MFUN(audiounit_send_midi)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT status = GET_NEXT_INT(ARGS);
    t_CKINT data1 = GET_NEXT_INT(ARGS);
    t_CKINT data2 = GET_NEXT_INT(ARGS);

    RETURN->v_int = (wrapper && wrapper->sendMIDI(status, data1, data2)) ? 1 : 0;
}

CK_DLL_MFUN(audiounit_note_on)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT pitch = GET_NEXT_INT(ARGS);
    t_CKINT velocity = GET_NEXT_INT(ARGS);

    RETURN->v_int = (wrapper && wrapper->noteOn(pitch, velocity)) ? 1 : 0;
}

CK_DLL_MFUN(audiounit_note_off)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT pitch = GET_NEXT_INT(ARGS);

    RETURN->v_int = (wrapper && wrapper->noteOff(pitch)) ? 1 : 0;
}

CK_DLL_MFUN(audiounit_control_change)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT cc = GET_NEXT_INT(ARGS);
    t_CKINT value = GET_NEXT_INT(ARGS);

    RETURN->v_int = (wrapper && wrapper->controlChange(cc, value)) ? 1 : 0;
}

CK_DLL_MFUN(audiounit_program_change)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    t_CKINT program = GET_NEXT_INT(ARGS);

    RETURN->v_int = (wrapper && wrapper->programChange(program)) ? 1 : 0;
}

CK_DLL_MFUN(audiounit_is_music_device)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    RETURN->v_int = (wrapper && wrapper->isMusicDevice()) ? 1 : 0;
}

CK_DLL_MFUN(audiounit_get_midi_device)
{
    AudioUnitWrapper* wrapper = (AudioUnitWrapper*)OBJ_MEMBER_INT(SELF, audiounit_data_offset);
    const char* name = wrapper ? wrapper->getMIDIDeviceName() : "";
    RETURN->v_string = API->object->create_string(VM, name, false);
}