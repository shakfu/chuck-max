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
    {
        // Allocate input/output buffers
        m_inputBuffer.mNumberChannels = 2;
        m_inputBuffer.mDataByteSize = sizeof(float) * 2;
        m_inputBuffer.mData = calloc(2, sizeof(float));

        m_outputBuffer.mNumberChannels = 2;
        m_outputBuffer.mDataByteSize = sizeof(float) * 2;
        m_outputBuffer.mData = calloc(2, sizeof(float));

        m_bufferList.mNumberBuffers = 1;
        m_bufferList.mBuffers[0] = m_outputBuffer;
    }

    ~AudioUnitWrapper()
    {
        close();
        if(m_inputBuffer.mData) free(m_inputBuffer.mData);
        if(m_outputBuffer.mData) free(m_outputBuffer.mData);
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

        // Set format for input (for effects)
        AudioUnitSetProperty(m_audioUnit,
                           kAudioUnitProperty_StreamFormat,
                           kAudioUnitScope_Input,
                           0,
                           &streamFormat,
                           sizeof(streamFormat));

        // Set maximum frames per slice
        UInt32 maxFrames = 1;
        AudioUnitSetProperty(m_audioUnit,
                           kAudioUnitProperty_MaximumFramesPerSlice,
                           kAudioUnitScope_Global,
                           0,
                           &maxFrames,
                           sizeof(maxFrames));

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

        return true;
    }

    void close()
    {
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

        // Set up input
        float* inputData = (float*)m_inputBuffer.mData;
        inputData[0] = input;
        inputData[1] = input;

        // Render
        AudioTimeStamp timeStamp;
        memset(&timeStamp, 0, sizeof(AudioTimeStamp));
        timeStamp.mFlags = kAudioTimeStampSampleTimeValid;
        timeStamp.mSampleTime = 0;

        UInt32 frameCount = 1;
        AudioUnitRenderActionFlags flags = 0;

        OSStatus status = AudioUnitRender(m_audioUnit,
                                        &flags,
                                        &timeStamp,
                                        0,
                                        frameCount,
                                        &m_bufferList);

        if(status != noErr)
            return input;

        // Get output (mono from left channel)
        float* outputData = (float*)m_outputBuffer.mData;
        return outputData[0];
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

    AudioUnit m_audioUnit;
    t_CKFLOAT m_sampleRate;
    UInt32 m_componentType;
    int m_numInputs;
    int m_numOutputs;
    bool m_bypass;

    AudioBuffer m_inputBuffer;
    AudioBuffer m_outputBuffer;
    AudioBufferList m_bufferList;

    std::vector<ParameterInfo> m_parameters;
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