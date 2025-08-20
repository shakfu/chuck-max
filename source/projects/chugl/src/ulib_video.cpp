/*
TODO
- destructor cleanup
    - what happens when video texture is still referenced, but video itself is not?
        - don't destroy the texture, just decrement texture refcount.
        - video texture will no longer be updated, frozen on last decoded video frame

- try to make seek sample-accurate for audio
- Video.load, allow switching video file dynamically
    - but what about the texture? what should be behavior if new video has different
dimensions/aspect?
- default constructor
- implement frame-skip in plm_decode so that high playback rates do not decode all
intermediate frames
- SIMD optimize decoding
- Support HAP
    - https://github.com/keijiro/KlakHap/blob/master/Plugin/Source/KlakHap.cpp
- Support YCbCr textures


webcam todo
- in destructor of webcam, decrement refcount on R_WebcamData
- test cleanup sr_webcam_delete in component_free(), see if it still segfaults
    - if so, never call sr_webcam_delete, in Webcam destructor just remove from R_Webcam
Arena

behaviour:
- webcam.freeze() stops updating texture and decrements R_WebcamData refcount
- if R_WebcamData refcount reaches 0, call sr_webcam_stop()
- if R_WebcamData refcount goes from 0 to 1, call sr_webcam_start()
- Webcam destructor decrements R_WebcamData refcount
*/

#include "ulib_helper.h"

#include "sg_command.h"
#include "sg_component.h"

#include "core/log.h"

#include <pl/pl_mpeg.h>

// Note: azaday 11/19/24: fixing a video memory crash
// video doesn't extend SG_Component so it doesn't have a component_offset_id
// adding a sepecial case member variable `video_component_offset_id` to store the
// Scenegraph ID of the video component
static t_CKUINT video_component_offset_id = 0;
#define GET_VIDEO(ckobj) SG_GetVideo(OBJ_MEMBER_UINT(ckobj, video_component_offset_id))
#define GET_WEBCAM(ckobj) SG_GetWebcam(OBJ_MEMBER_UINT(ckobj, component_offset_id))

CK_DLL_CTOR(video_ctor);
CK_DLL_CTOR(video_ctor_with_path);
CK_DLL_TICKF(video_tick_multichannel);

// video metadata
CK_DLL_MFUN(video_get_framerate);
CK_DLL_MFUN(video_get_samplerate);
CK_DLL_MFUN(video_get_length);

CK_DLL_MFUN(video_get_texture_rgba);
CK_DLL_MFUN(video_get_width_texels);
CK_DLL_MFUN(video_get_height_texels);

// video playback
CK_DLL_MFUN(video_get_time);

// manipulation
CK_DLL_MFUN(video_seek);
CK_DLL_MFUN(video_set_rate);
CK_DLL_MFUN(video_get_rate);
CK_DLL_MFUN(video_set_loop);
CK_DLL_MFUN(video_get_loop);

//
// webcam
//
CK_DLL_CTOR(webcam_ctor);
CK_DLL_CTOR(webcam_ctor_with_device_id);
CK_DLL_CTOR(webcam_ctor_with_device_id_and_format);

CK_DLL_MFUN(webcam_set_capture);
CK_DLL_MFUN(webcam_get_capture);
CK_DLL_MFUN(webcam_set_freeze);
CK_DLL_MFUN(webcam_get_freeze);

CK_DLL_MFUN(webcam_get_width);
CK_DLL_MFUN(webcam_get_height);
CK_DLL_MFUN(webcam_get_aspect);
CK_DLL_MFUN(webcam_get_fps);
CK_DLL_MFUN(webcam_get_texture);
CK_DLL_MFUN(webcam_get_device_id);
CK_DLL_MFUN(webcam_get_device_name);

void ulib_video_query(Chuck_DL_Query* QUERY)
{
    { // video
        BEGIN_CLASS(SG_CKNames[SG_COMPONENT_VIDEO], "UGen_Multi");
        DOC_CLASS(
          "ChuGL Video object. Currently supports decoding MPEG1 Video and MP2 Audio. "
          "Wraps the plmpeg library, which you can find here for more documentation: "
          "https://github.com/phoboslab/pl_mpeg/ (see link for an example on how to "
          "encode "
          "videos into a supported format with ffmpeg). This is a hybrid "
          "graphics/audio "
          "object. "
          "Video is a stereo UGen which may be connected dac for audio output. "
          "The video texture may be accessed with the `texture()` member function, "
          "which "
          "will be updated synchronously with audio. For sample-accurate audio "
          "manipulation, we recommend controlling the audio data separately with "
          "SndBuf. You can convert videos to the MPEG format with the following "
          "command-line ffmpeg command: `ffmpeg -i input.mp4 -c:v mpeg1video -q:v 0 "
          "-c:a mp2 -format mpeg output.mpg`");
        ADD_EX("basic/video.ck");

        video_component_offset_id = MVAR("int", "@component_ptr", false);

        QUERY->add_ugen_funcf(QUERY, video_tick_multichannel, NULL,
                              0, // 0 channels in
                              2  // stereo out
        );

        CTOR(video_ctor);
        DOC_FUNC(
          "Default video constructor. Currently videos are not mutable -- i.e. you "
          "cannot "
          "change the video file after creation. This default constructor will create "
          "an "
          "empty video object and default to a static magenta video texture."
          "Use the alternate constructor Video(string path) instead");

        CTOR(video_ctor_with_path);
        ARG("string", "path");
        DOC_FUNC(
          "Alternate video constructor. Opens a video file at the given path. "
          "Currently "
          "only supports MPEG1 video and MP2 audio. If the video file cannot be "
          "opened, "
          "the "
          "video object will default to a static magenta video texture.");

        MFUN(video_get_framerate, "float", "framerate");
        DOC_FUNC("Get the framerate of the video.");

        MFUN(video_get_framerate, "float", "fps");
        DOC_FUNC("Get the framerate of the video.");

        MFUN(video_get_samplerate, "int", "samplerate");
        DOC_FUNC("Get the samplerate of the video's audio stream");

        MFUN(video_get_length, "dur", "duration");
        DOC_FUNC("Get total length of the file as a duration.");

        MFUN(video_get_texture_rgba, SG_CKNames[SG_COMPONENT_TEXTURE], "texture");
        DOC_FUNC("Get the RGBA texture of the video.");

        MFUN(video_get_width_texels, "int", "width");
        DOC_FUNC("Get the width of the video in texels.");

        MFUN(video_get_height_texels, "int", "height");
        DOC_FUNC("Get the height of the video in texels.");

        // MFUN(video_get_aspect_ratio, "float", "aspect");
        // DOC_FUNC("Get the video pixel aspect ratio");

        MFUN(video_get_time, "dur", "timestamp");
        DOC_FUNC(
          "Get the current video playhead duration in seconds (time since video "
          "start)");

        MFUN(video_set_rate, "void", "rate");
        ARG("float", "rate");
        DOC_FUNC(
          "Set the playback rate of the video. 1.0 is normal speed. Due to limitations "
          "in "
          "the MPEG encoding format, rate cannot be set to less than 0.0. Reverse "
          "playback "
          "is NOT supported. Negative rates will be clamped to 0");

        MFUN(video_get_rate, "void", "rate");
        DOC_FUNC(
          "Get the playback rate of the video. 1.0 is normal speed. Negative rates are "
          "not "
          "supported and will be clamped to 0");

        MFUN(video_get_loop, "int", "loop");
        DOC_FUNC("Get whether the video is looping. Default false");

        MFUN(video_set_loop, "void", "loop");
        ARG("int", "loop");
        DOC_FUNC("Set whether the video should loop. Default false");

        MFUN(video_seek, "void", "seek");
        ARG("dur", "time");
        DOC_FUNC(
          "Seek to a specific time in the video. Negative values and values greater "
          "than "
          "the video length will wrap around. Note that seek is *not* sample-accurate; "
          "due "
          "to the nature of MPEG encoding (and the demands of real-time performance), "
          "the "
          "seek will be to the nearest keyframe, which in practice is within a few "
          "frames "
          "of the target time. For sample-accurate seeking and audio manipulation, we "
          "recommend loading the audio data separately into a SndBuf");

        END_CLASS();
    }

    { // webcam
        BEGIN_CLASS(SG_CKNames[SG_COMPONENT_WEBCAM], SG_CKNames[SG_COMPONENT_BASE]);
        DOC_CLASS(
          "ChuGL Webcam class. Opens a webcam at any device id 0-7, and "
          "updates a texture with the webcam feed. The webcam texture may be "
          "accessed with the `.texture()` member function");
        ADD_EX("basic/webcam.ck");
        ADD_EX("deep/webcam_echo.ck");

        CTOR(webcam_ctor);
        DOC_FUNC(
          "Create a webcam object with default device id 0. On laptops this is "
          "usually the built-in webcam.");

        CTOR(webcam_ctor_with_device_id);
        ARG("int", "device_id");
        DOC_FUNC(
          "Create a webcam object with a specific device id. On laptops, device id 0 "
          "is usually the built-in webcam.");

        CTOR(webcam_ctor_with_device_id_and_format);
        ARG("int", "device_id");
        ARG("int", "width");
        ARG("int", "height");
        ARG("int", "fps");
        DOC_FUNC(
          "Create a webcam object with a specific device id and format. Will fallback "
          "to the nearest supported width/height/fps. Choose device_id 0 for laptop "
          "builtin webcam.");

        MFUN(webcam_get_width, "int", "width");
        DOC_FUNC("Get the width of the webcam in pixels.");

        MFUN(webcam_get_height, "int", "height");
        DOC_FUNC("Get the height of the webcam in pixels.");

        MFUN(webcam_get_aspect, "float", "aspect");
        DOC_FUNC("Get the aspect ratio of the webcam.");

        MFUN(webcam_get_fps, "int", "fps");
        DOC_FUNC("Get the framerate of the webcam in frames per second.");

        MFUN(webcam_get_fps, "int", "framerate");
        DOC_FUNC("Get the framerate of the webcam in frames per second.");

        MFUN(webcam_get_texture, SG_CKNames[SG_COMPONENT_TEXTURE], "texture");
        DOC_FUNC("Get the RGBA texture of the webcam.");

        MFUN(webcam_get_device_id, "int", "deviceID");
        DOC_FUNC("Get the device id of this webcam.");

        MFUN(webcam_get_device_name, "string", "deviceName");
        DOC_FUNC("Get the user-friendly name of this webcam.");

        MFUN(webcam_set_capture, "void", "capture");
        ARG("int", "capture");
        DOC_FUNC(
          "disable or enable webcam capture. If disabled, this webcam corresponding to "
          "this device id will stop reading new data and the webcam texture will "
          "not be updated.");

        MFUN(webcam_get_capture, "int", "capture");
        DOC_FUNC("Get whether the webcam is capturing frames.");

        MFUN(webcam_set_freeze, "void", "freeze");
        ARG("int", "freeze");
        DOC_FUNC(
          "disable or enable webcam texture updates. If enabled, the webcam texture "
          "will not be updated with new data, but the webcam will continue to capture "
          "frames. Other webcam objects using the same device id will still be "
          "updated.");

        MFUN(webcam_get_freeze, "int", "freeze");
        DOC_FUNC("Get whether the webcam texture is frozen (not updating).");

        END_CLASS();
    }
}

// (Chuck_Object* SELF, SAMPLE* in, SAMPLE* out, t_CKUINT nframes,CK_DL_API API)
CK_DLL_TICKF(video_tick_multichannel)
{
    ASSERT(nframes == 1); // TODO ask Ge/Nick if this is ever not 1?

    SG_Video* video = GET_VIDEO(SELF);

    // handle default unitialized video
    if (!video->plm) {
        out[0] = 0;
        out[1] = 0;
        return TRUE;
    }

    while (video->samples == NULL
           || video->audio_playhead >= (float)video->samples->count - 1) {

        if (video->samples) {
            // decrement the playhead by the number of samples in the last audio frame
            video->audio_playhead -= video->samples->count;
            // save last audio samples
            video->last_audio_samples[0]
              = video->samples->interleaved[video->samples->count * 2 - 2];
            video->last_audio_samples[1]
              = video->samples->interleaved[video->samples->count * 2 - 1];
        }

        video->samples = plm_decode_audio(video->plm);
        if (!video->samples) break;
        // log_info("decoded 1 audio frame with time %f, count %d",
        // video->samples->time,
        //          video->samples->count);
    }

    if (video->samples) {
        ASSERT(video->audio_playhead <= video->samples->count - 1.0f);
        // TODO linear interpolation
        float integer{};
        float fract = modff(video->audio_playhead, &integer);

        if (video->audio_playhead < 0) {
            // if less than zero, we interpolate between the last samples of the
            // previous audio frame, and the first sample of the current one
            ASSERT(video->audio_playhead >= -1.f);

            out[0] = video->last_audio_samples[0] * -fract
                     + video->samples->interleaved[0] * (1 + fract);
            out[1] = video->last_audio_samples[1] * -fract
                     + video->samples->interleaved[1] * (1 + fract);

        } else {
            out[0] = video->samples->interleaved[(int)integer * 2 + 0] * (1 - fract)
                     + video->samples->interleaved[(int)integer * 2 + 2] * fract;
            out[1] = video->samples->interleaved[(int)integer * 2 + 1] * (1 - fract)
                     + video->samples->interleaved[(int)integer * 2 + 3] * fract;
        }

        video->audio_playhead += video->rate;
    } else {
        // end of audio stream
        out[0] = 0;
        out[1] = 0;
    }

    return TRUE;
}

// audio decode callback, used by seek
static void ulib_video_on_audio(plm_t* mpeg, plm_samples_t* samples, void* user)
{
    // plm_samples_t should be valid until the next audio callback
    SG_Video* video = SG_GetVideo((intptr_t)user);

    // log_error("decoding audio samples %d", samples->count);

    { // update the audio samples
        video->samples        = samples;
        video->audio_playhead = 0;
    }
}

CK_DLL_CTOR(video_ctor)
{
    log_warn(
      "Creating default empty Video object, you probably want to use the `Video(string "
      "path)` constructor instead");

    SG_Texture* video_texture_rgba = SG_GetTexture(g_builtin_textures.magenta_pixel_id);
    SG_Video* video                = SG_CreateVideo(SELF, video_component_offset_id);

    video->path_OWNED            = NULL;
    video->video_texture_rgba_id = video_texture_rgba->id;

    // intentionally do NOT push this to graphics thread
}

CK_DLL_CTOR(video_ctor_with_path)
{
    const char* path = chugin_copyCkString(GET_NEXT_STRING(ARGS));

    plm_t* plm = NULL;

    // TODO destroy plm in destructor

    SG_Texture* video_texture_rgba = SG_GetTexture(g_builtin_textures.magenta_pixel_id);

    SG_Video* video              = SG_CreateVideo(SELF, video_component_offset_id);
    video->video_texture_rgba_id = video_texture_rgba->id;

    // Initialize plmpeg, load the video file, install decode callbacks
    {
        plm = plm_create_with_filename(path);
        if (!plm) {
            log_warn("Could not open MPG video '%s'", path);
            log_warn(" |- Defaulting to magenta texture");
            return;
        }

        // probe first 5MB of file for video or audio streams
        if (!plm_probe(plm, 5000 * 1024)) {
            plm_destroy(plm);
            log_warn("No MPEG video or audio streams found in %s", path);
            plm = NULL;
            return;
        }
    }

    //
    // at this point plm is valid
    //

    // save video metadata
    video->framerate     = (float)plm_get_framerate(plm);
    video->samplerate    = plm_get_samplerate(plm);
    video->duration_secs = (float)plm_get_duration(plm);
    // log_info("Opened %s - framerate: %f, samplerate: %d, duration: %f", path,
    //          video->framerate, video->samplerate, video->duration_secs);

    // initialize audio
    if (plm_get_num_audio_streams(plm) > 0) {
        plm_set_audio_decode_callback(plm, ulib_video_on_audio,
                                      (void*)(intptr_t)video->id);

        plm_set_audio_enabled(plm, TRUE);
        plm_set_video_enabled(plm, TRUE);
        plm_set_audio_stream(plm, 0); // TODO support multiple audio streams?

        // Adjust the audio lead time according to the audio_spec buffer size
        // TODO hardcoded to 512 samples, change to bufsize when chugin headers adds
        // it
        plm_set_audio_lead_time(plm, (double)512 / (double)API->vm->srate(VM));

        // warn if sample rate doesn't match
        if (plm_get_samplerate(plm) != API->vm->srate(VM)) {
            log_warn(
              "Video %s: Audio stream samplerate for (%d) does not match VM samplerate "
              "(%d)",
              path, plm_get_samplerate(plm), API->vm->srate(VM));
        }
    }

    // create the rgb video texture (TODO support YcbCr later)
    SG_TextureDesc desc = {};
    desc.width          = plm_get_width(plm);
    desc.height         = plm_get_height(plm);
    desc.dimension      = WGPUTextureDimension_2D;
    desc.format         = WGPUTextureFormat_RGBA8Unorm;
    desc.usage          = WGPUTextureUsage_All; // TODO: restrict usage?
    desc.gen_mips       = false;                // no mipmaps for video

    video_texture_rgba = SG_CreateTexture(&desc, NULL, SHRED, true);

    // init remaining video fields
    video->plm                   = plm;
    video->path_OWNED            = path;
    video->video_texture_rgba_id = video_texture_rgba->id;

    CQ_PushCommand_VideoUpdate(video);
}

CK_DLL_MFUN(video_get_framerate)
{
    RETURN->v_float = GET_VIDEO(SELF)->framerate;
}

CK_DLL_MFUN(video_get_samplerate)
{
    RETURN->v_int = GET_VIDEO(SELF)->samplerate;
}

CK_DLL_MFUN(video_get_length)
{
    RETURN->v_dur = SG_Video::audioFrames(GET_VIDEO(SELF));
}

CK_DLL_MFUN(video_get_texture_rgba)
{
    SG_Video* video  = GET_VIDEO(SELF);
    RETURN->v_object = SG_GetTexture(video->video_texture_rgba_id)->ckobj;
}

CK_DLL_MFUN(video_get_width_texels)
{
    SG_Video* video = GET_VIDEO(SELF);
    SG_Texture* tex = SG_GetTexture(video->video_texture_rgba_id);
    RETURN->v_uint  = tex->desc.width;
}

CK_DLL_MFUN(video_get_height_texels)
{
    SG_Video* video = GET_VIDEO(SELF);
    SG_Texture* tex = SG_GetTexture(video->video_texture_rgba_id);
    RETURN->v_uint  = tex->desc.height;
}

// CK_DLL_MFUN(video_get_aspect_ratio)
// {
//     SG_Video* video = GET_VIDEO(SELF);
//     RETURN->v_float = plm_get_pixel_aspect_ratio(video->plm);
// }

CK_DLL_MFUN(video_get_time)
{
    plm_t* plm = GET_VIDEO(SELF)->plm;
    if (plm) {
        RETURN->v_dur = plm_get_time(plm) * API->vm->srate(VM);
    } else {
        RETURN->v_dur = 0;
    }
}

CK_DLL_MFUN(video_set_rate)
{
    SG_Video* video = GET_VIDEO(SELF);
    video->rate     = GET_NEXT_FLOAT(ARGS);
    video->rate     = MAX(0.0, video->rate);

    if (video->plm) {
        CQ_PushCommand_VideoRate(video->id, video->rate, plm_get_loop(video->plm));
    }
}

CK_DLL_MFUN(video_get_rate)
{
    RETURN->v_float = GET_VIDEO(SELF)->rate;
}

CK_DLL_MFUN(video_set_loop)
{
    SG_Video* video = GET_VIDEO(SELF);
    if (!video->plm) return;

    plm_set_loop(video->plm, GET_NEXT_INT(ARGS));
    CQ_PushCommand_VideoRate(video->id, video->rate, plm_get_loop(video->plm));
}

CK_DLL_MFUN(video_get_loop)
{
    plm_t* plm = GET_VIDEO(SELF)->plm;
    if (!plm) {
        RETURN->v_int = 0;
    } else {
        RETURN->v_int = plm_get_loop(plm);
    }
}

CK_DLL_MFUN(video_seek)
{
    SG_Video* video = GET_VIDEO(SELF);
    if (!video->plm) return;

    // get time and wrap around video length (allows negative indexing)
    int time_samples = (int)GET_NEXT_DUR(ARGS);
    while (time_samples < 0) {
        time_samples += SG_Video::audioFrames(video);
    }
    time_samples = time_samples % SG_Video::audioFrames(video);

    double time_seconds = (double)time_samples / API->vm->srate(VM);

    // log_error("seeking to %f seconds from %f seconds", time_seconds,
    //           plm_get_time(video->plm));

    double out_seek_time = 0;
    int ret              = plm_seek_audio(video->plm, time_seconds, &out_seek_time);

    if (!ret) {
        log_warn("Could not seek to target time %f in MPG video '%s'", time_seconds,
                 video->path_OWNED);
        return;
    }

    // else seek succeeded, tell video to seek as well
    // log_error("targeted seek time %f, actual seek time %f, difference: %f",
    //           time_seconds, out_seek_time, time_seconds - out_seek_time);

    CQ_PushCommand_VideoSeek(video->id, out_seek_time);
}

// =================================================================================================
// webcam
// =================================================================================================

// currently unsupported on linux.
// logs warning if Webcam constructor is called on linux
static void ulib_webcam_validate()
{
#if defined(_WIN32)
#elif defined(__APPLE__)
#elif defined(__EMSCRIPTEN__)
#else /* anything else, this will need more care for non-Linux platforms */
    log_warn("Webcam is not supported on this platform");
#endif
}

CK_DLL_CTOR(webcam_ctor)
{
    ulib_webcam_validate();
    // SG_CreateWebcam(SELF, SHRED, 0, 640, 480, 60);
    // default to really high resolution (should fallback to largest supported)
    SG_CreateWebcam(SELF, SHRED, 0, 4096, 4096, 60);
}

CK_DLL_CTOR(webcam_ctor_with_device_id)
{
    ulib_webcam_validate();
    int device_id = GET_NEXT_INT(ARGS);
    SG_CreateWebcam(SELF, SHRED, device_id, 640, 480, 60);
}

CK_DLL_CTOR(webcam_ctor_with_device_id_and_format)
{
    ulib_webcam_validate();
    int device_id = GET_NEXT_INT(ARGS);
    int width     = GET_NEXT_INT(ARGS);
    int height    = GET_NEXT_INT(ARGS);
    int fps       = GET_NEXT_INT(ARGS);
    SG_CreateWebcam(SELF, SHRED, device_id, width, height, fps);
}

CK_DLL_MFUN(webcam_get_width)
{
    SG_Texture* tex = SG_GetTexture(GET_WEBCAM(SELF)->texture_id);
    ASSERT(tex);
    RETURN->v_int = tex->desc.width;
}

CK_DLL_MFUN(webcam_get_height)
{
    SG_Texture* tex = SG_GetTexture(GET_WEBCAM(SELF)->texture_id);
    ASSERT(tex);
    RETURN->v_int = tex->desc.height;
}

CK_DLL_MFUN(webcam_get_aspect)
{
    SG_Texture* tex = SG_GetTexture(GET_WEBCAM(SELF)->texture_id);
    ASSERT(tex);
    RETURN->v_float = (float)tex->desc.width / (float)tex->desc.height;
}

CK_DLL_MFUN(webcam_get_fps)
{
    RETURN->v_int = GET_WEBCAM(SELF)->fps;
}

CK_DLL_MFUN(webcam_get_texture)
{
    RETURN->v_object = SG_GetTexture(GET_WEBCAM(SELF)->texture_id)->ckobj;
}

CK_DLL_MFUN(webcam_get_device_id)
{
    RETURN->v_int = GET_WEBCAM(SELF)->device_id;
}

CK_DLL_MFUN(webcam_get_device_name)
{
    // return new string (no ref count needed; chuck VM function call mechanism will
    // take it from here)
    RETURN->v_string = chugin_createCkString(GET_WEBCAM(SELF)->device_name, false);
}

CK_DLL_MFUN(webcam_set_capture)
{
    SG_Webcam* webcam = GET_WEBCAM(SELF);
    webcam->capture   = GET_NEXT_INT(ARGS);
    CQ_PushCommand_WebcamUpdate(webcam);
}

CK_DLL_MFUN(webcam_get_capture)
{
    RETURN->v_int = GET_WEBCAM(SELF)->capture;
}

CK_DLL_MFUN(webcam_set_freeze)
{
    SG_Webcam* webcam = GET_WEBCAM(SELF);
    webcam->freeze    = GET_NEXT_INT(ARGS);
    CQ_PushCommand_WebcamUpdate(webcam);
}

CK_DLL_MFUN(webcam_get_freeze)
{
    RETURN->v_int = GET_WEBCAM(SELF)->freeze;
}
