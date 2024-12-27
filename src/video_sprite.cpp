#include "video_sprite.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "memory.h"


using namespace godot;

void VideoSprite::_bind_methods() {
    // properties
    ClassDB::bind_method(D_METHOD("set_file_name", "file_name"), &VideoSprite::set_file_name);
    ClassDB::bind_method(D_METHOD("get_file_name"), &VideoSprite::get_file_name);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "file_name"), "set_file_name", "get_file_name");
    
    // methods
    ClassDB::bind_method(D_METHOD("play"), &VideoSprite::play);
}

void VideoSprite::set_file_name(const String& p_file_name) {
    file_name = p_file_name;
}

String VideoSprite::get_file_name() const {
    return file_name;
}

bool VideoSprite::create_video_context() {
    if (file_name.is_empty()) {
        UtilityFunctions::print("VideoSprite: File name is empty.");
        return false;
    }

    CharString char_file_name = file_name.utf8();
    sav1_default_settings(&settings, (char*) char_file_name.get_data());
    settings.desired_pixel_format = SAV1_PIXEL_FORMAT_ORIG;
    
    int success = sav1_create_context(&context, &settings);
    if (success < 0) {
        UtilityFunctions::print("Error creating context!");
        return false;
    }
    
    return true;
}

void VideoSprite::play() {
    if (!context.is_initialized) {
         if (!create_video_context()) {
         return;
         }
    }

    sav1_start_playback(&context);
}

VideoSprite::VideoSprite() {
}

VideoSprite::~VideoSprite() {
    sav1_destroy_context(&context);
}

void VideoSprite::_ready() {
    image = Image::create(1920, 1080, false, Image::FORMAT_RG8);
    image->fill(Color(1.0, 1.0, 1.0, 1.0));
    texture = ImageTexture::create_from_image(image);
    set_texture(texture);
}

void VideoSprite::process_video_frame() {
    Sav1VideoFrame *sav1_frame;
    sav1_get_video_frame(&context, &sav1_frame);
    if (sav1_frame->pixel_format == SAV1_PIXEL_FORMAT_ORIG) {
        size_t width = sav1_frame->width;
        size_t height = sav1_frame->height;
        if ((width != image->get_width()) || (height != image->get_height()) || image->get_format() != Image::FORMAT_RG8) {
            image = Image::create(width, height, false, Image::FORMAT_RG8);
            image->fill(Color(1.0, 1.0, 1.0, 1.0));
            texture = ImageTexture::create_from_image(image);
            set_texture(texture);
        }
        memcpy((void*)image->ptrw(), (void*)sav1_frame->data, width * height * 2);
    }
    else {
        size_t width = sav1_frame->width;
        size_t height = sav1_frame->height;
        if ((width != image->get_width()) || (height != image->get_height()) || image->get_format() != Image::FORMAT_RGBA8) {
            image = Image::create(width, height, false, Image::FORMAT_RGBA8);
            image->fill(Color(1.0, 1.0, 1.0, 1.0));
            texture = ImageTexture::create_from_image(image);
            set_texture(texture);
        }
        memcpy((void*)image->ptrw(), (void*)sav1_frame->data, width * height * 4);
    }
    texture->update(image);
}

void VideoSprite::process_audio_frame() {
    Sav1AudioFrame *sav1_frame;
    sav1_get_audio_frame(&context, &sav1_frame);
    // do not use audio frame for now
}

void VideoSprite::_process(double delta) {
    if (!context.is_initialized) {
         return;
    }

    // video frame
    int frame_ready;
    int success = sav1_get_video_frame_ready(&context, &frame_ready);
    if (success < 0) 
        UtilityFunctions::print("Error getting frame!");

    if (frame_ready) {
        process_video_frame();
    }

    // audio frame
    sav1_get_audio_frame_ready(&context, &frame_ready);
    if (frame_ready) {
        process_audio_frame();
    }
}
