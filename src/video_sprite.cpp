#include "video_sprite.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "memory.h"


using namespace godot;

void VideoSprite::_bind_methods() {
}

VideoSprite::VideoSprite() {
    time_passed = 0.0;
    sav1_default_settings(&settings, "/home/nils/Public/test.webm");
    settings.desired_pixel_format = SAV1_PIXEL_FORMAT_ORIG;
    int success = sav1_create_context(&context, &settings);
    if (success < 0) 
        UtilityFunctions::print("Error creating context!");
}

VideoSprite::~VideoSprite() {
    sav1_destroy_context(&context);
}

void VideoSprite::_ready() {
    sav1_start_playback(&context);
    image = Image::create(1920, 1080, false, Image::FORMAT_RG8);
    image->fill(Color(1.0, 1.0, 1.0, 1.0));
    texture = ImageTexture::create_from_image(image);
    set_texture(texture);
}

void VideoSprite::_process(double delta) {
    int frame_ready;

    // video frame
    int success = sav1_get_video_frame_ready(&context, &frame_ready);
    if (success < 0) 
        UtilityFunctions::print("Error getting frame!");

    if (frame_ready) {
        UtilityFunctions::print("Frame ready!");
        Sav1VideoFrame *sav1_frame;
        sav1_get_video_frame(&context, &sav1_frame);
        if (sav1_frame->pixel_format == SAV1_PIXEL_FORMAT_ORIG) {
            size_t width = sav1_frame->width;
            size_t height = sav1_frame->height;
            if ((width != image->get_width()) || (height != image->get_height()) || image->get_format() != Image::FORMAT_RG8) {
                image = Image::create(width, height, false, Image::FORMAT_RG8);
                image->fill(Color(1.0, 1.0, 1.0, 1.0));
                texture = ImageTexture::create_from_image(image);
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
            }
            memcpy((void*)image->ptrw(), (void*)sav1_frame->data, width * height * 4);
        }
        texture->update(image);
        // do not use video frame for now
        //UtilityFunctions::print("Frame ready!");
    }
    //else {
    //    UtilityFunctions::print("Frame not ready!!");
    //}

    // audio frame
    sav1_get_audio_frame_ready(&context, &frame_ready);
    if (frame_ready) {
        Sav1AudioFrame *sav1_frame;
        sav1_get_audio_frame(&context, &sav1_frame);
        // do not use audio frame for now
    }

    //time_passed += delta;
    //Vector2 new_position = Vector2(10.0 + (10.0 * sin(time_passed * 2.0)), 10.0 + (10.0 * cos(time_passed * 1.5)) );
    //set_position(new_position);
    //image->fill(Color((cos(time_passed) + 1.0) / 2.0, 1.0, (sin(time_passed) + 1.0) / 2.0, 1.0));
    //texture->update(image);
}
