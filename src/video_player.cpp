#include "video_player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "memory.h"


using namespace godot;

void VideoPlayer::_bind_methods() {
    // properties
    ClassDB::bind_method(D_METHOD("set_file_name", "file_name"), &VideoPlayer::set_file_name);
    ClassDB::bind_method(D_METHOD("get_file_name"), &VideoPlayer::get_file_name);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "file_name"), "set_file_name", "get_file_name");
    
    // methods
    ClassDB::bind_method(D_METHOD("set_looping"), &VideoPlayer::set_looping);
    ClassDB::bind_method(D_METHOD("is_looping"), &VideoPlayer::is_looping);
    ClassDB::bind_method(D_METHOD("play"), &VideoPlayer::play);
    ClassDB::bind_method(D_METHOD("pause"), &VideoPlayer::pause);
    ClassDB::bind_method(D_METHOD("stop"), &VideoPlayer::stop);
    ClassDB::bind_method(D_METHOD("is_playing"), &VideoPlayer::is_playing);
    ClassDB::bind_method(D_METHOD("update_frame"), &VideoPlayer::update_frame);
    ClassDB::bind_method(D_METHOD("get_texture"), &VideoPlayer::get_texture);
}

VideoPlayer::VideoPlayer() {
}

VideoPlayer::~VideoPlayer() {
}

void VideoPlayer::set_looping(bool p_looping) {
    looping = p_looping;
}

bool VideoPlayer::is_looping() const {
    return looping;
}

void VideoPlayer::set_file_name(const String& p_file_name) {
    file_name = p_file_name;
}

String VideoPlayer::get_file_name() const {
    return file_name;
}

Ref<Texture2D> VideoPlayer::get_texture() {
    return texture;
}



