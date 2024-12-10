#include "video_sprite.h"
#include <godot_cpp/core/class_db.hpp>


using namespace godot;

void VideoSprite::_bind_methods() {
}

VideoSprite::VideoSprite() {
    time_passed = 0.0;
}

VideoSprite::~VideoSprite() {
    // clean-up
}

void VideoSprite::_ready() {
    image = Image::create(3840, 2160, false, Image::FORMAT_RGBA8);
    image->fill(Color(1.0, 1.0, 1.0, 1.0));
    texture = ImageTexture::create_from_image(image);
    set_texture(texture);
}

void VideoSprite::_process(double delta) {
    time_passed += delta;

    Vector2 new_position = Vector2(10.0 + (10.0 * sin(time_passed * 2.0)), 10.0 + (10.0 * cos(time_passed * 1.5)) );
    set_position(new_position);
    image->fill(Color((cos(time_passed) + 1.0) / 2.0, 1.0, (sin(time_passed) + 1.0) / 2.0, 1.0));
    texture->update(image);
}
