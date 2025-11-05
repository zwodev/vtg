#include "video_texture.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "memory.h"


using namespace godot;

void VideoTexture::_bind_methods() {
    // properties
    ClassDB::bind_method(D_METHOD("set_file_name", "file_name"), &VideoTexture::set_file_name);
    ClassDB::bind_method(D_METHOD("get_file_name"), &VideoTexture::get_file_name);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "file_name"), "set_file_name", "get_file_name");
    
    // methods
    ClassDB::bind_method(D_METHOD("play"), &VideoTexture::play);
    ClassDB::bind_method(D_METHOD("stop"), &VideoTexture::stop);
    ClassDB::bind_method(D_METHOD("is_playing"), &VideoTexture::is_playing);
    ClassDB::bind_method(D_METHOD("update_frame"), &VideoTexture::update_frame);
}

void VideoTexture::set_file_name(const String& p_file_name) {
    file_name = p_file_name;
}

String VideoTexture::get_file_name() const {
    return file_name;
}

bool VideoTexture::create_video_context() {
    if (file_name.is_empty()) {
        UtilityFunctions::print("VideoSprite: File name is empty.");
        return false;
    }

    CharString char_file_name = file_name.utf8();
    sav1_default_settings(&settings, (char*) char_file_name.get_data());
    settings.desired_pixel_format = SAV1_PIXEL_FORMAT_ORIG;
    //settings.desired_pixel_format = SAV1_PIXEL_FORMAT_RGB;
    
    int success = sav1_create_context(&context, &settings);
    if (success < 0) {
        UtilityFunctions::print("Error creating context!");
        return false;
    }
    
    return true;
}

void VideoTexture::play() {
    if (!context.is_initialized) {
         if (!create_video_context()) {
            return;
         }
    }

    sav1_start_playback(&context);
}

void VideoTexture::stop() {
    if (!context.is_initialized)
        return;

    sav1_stop_playback(&context);
}

bool VideoTexture::is_playing() {
    if (!context.is_initialized)
        return false;

    int is_paused;
    int success = sav1_is_playback_paused(&context, &is_paused);
    if (success < 0) {
        return false;
    }

    return (is_paused == 0); 
}

VideoTexture::VideoTexture() {
    image = Image::create(1920, 1080, false, Image::FORMAT_RG8);
    image->fill(Color(1.0, 1.0, 1.0, 1.0));
    set_image(image);
}

VideoTexture::~VideoTexture() {
    sav1_destroy_context(&context);
}

void VideoTexture::process_video_frame() {    
    Sav1VideoFrame *sav1_frame;
    sav1_get_video_frame(&context, &sav1_frame);
    if (sav1_frame->pixel_format == SAV1_PIXEL_FORMAT_ORIG) {
        size_t width = sav1_frame->width;
        size_t height = sav1_frame->height;
        if ((width != image->get_width()) || (height != image->get_height()) || image->get_format() != Image::FORMAT_RG8) {
            image = Image::create(width, height, false, Image::FORMAT_RG8);
            image->fill(Color(1.0, 1.0, 1.0, 1.0));
            set_image(image);
        }
        memcpy((void*)image->ptrw(), (void*)sav1_frame->data, width * height * 2);
    }
    else {
        size_t width = sav1_frame->width;
        size_t height = sav1_frame->height;
        if ((width != image->get_width()) || (height != image->get_height()) || image->get_format() != Image::FORMAT_RGB8) {
            image = Image::create(width, height, false, Image::FORMAT_RGB8);
            image->fill(Color(1.0, 1.0, 1.0, 1.0));
            set_image(image);
        }
        memcpy((void*)image->ptrw(), (void*)sav1_frame->data, width * height * 3);
    }
    this->update(image);
}

void VideoTexture::process_audio_frame() {
    Sav1AudioFrame *sav1_frame;
    sav1_get_audio_frame(&context, &sav1_frame);
    // do not use audio frame for now
}

void VideoTexture::update_frame() {
    if (!context.is_initialized)
         return;

    if (!is_playing()) 
        return;
    
    int isAtEnd = 0;
    sav1_is_playback_at_file_end(&context, &isAtEnd);
    if (isAtEnd) {
        sav1_seek_playback(&context, 0, SAV1_SEEK_MODE_FAST);
        return;
    }

    // video frame
    int video_frame_ready;
    int success = sav1_get_video_frame_ready(&context, &video_frame_ready);
    if (success < 0) 
        UtilityFunctions::print("Error getting frame!");

    if (video_frame_ready) {
        process_video_frame();
    }

    // audio frame
    int audio_frame_ready;
    sav1_get_audio_frame_ready(&context, &audio_frame_ready);
    if (audio_frame_ready) {
        process_audio_frame();
    }
}
