#include "video_player_metal.h"
#include "VideoDecoderMetal.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include "memory.h"


using namespace godot;

VideoPlayerMetal::VideoPlayerMetal() {
    create_textures();
    resize_textures(1920, 1080);
}

VideoPlayerMetal::~VideoPlayerMetal() {
    cleanup();
}

void VideoPlayerMetal::_bind_methods() {
    VideoPlayer::_bind_methods();
}

void VideoPlayerMetal::create_textures() {
    for (size_t i = 0; i < BUFFER_COUNT; ++i) {
        image_textures[i] = Ref<ImageTexture>(memnew(godot::ImageTexture));
    }
}

void VideoPlayerMetal::resize_textures(int width, int height) {
    image = Image::create(width, height, false, Image::FORMAT_RGBA8);
    image->fill(Color(1.0, 1.0, 1.0, 1.0));
    for (size_t i = 0; i < BUFFER_COUNT; ++i) {
        image_textures[i]->set_image(image);
    }
}

void VideoPlayerMetal::cleanup() {
    if (initialized) {
        swift_cleanup_decoder();
        initialized = false;
    }
}

bool VideoPlayerMetal::create_video_context() {
    cleanup();

    int video_width = 0;
    int video_height = 0;
    
    // Initialize decoder with just the path to get video size
    swift_initialize_decoder(file_name.utf8().get_data(), nullptr, 0, 0, 0);
    const char* err_str = swift_get_last_error();
    if (err_str) {
        UtilityFunctions::print("create_video_context (dummy) error: " + String(err_str));
        return false;
    }

    swift_get_video_size(&video_width, &video_height);
    UtilityFunctions::print("Width: ", video_width);
    UtilityFunctions::print("Width: ", video_height);

    if (video_width <= 0 || video_height <= 0) {
        last_error = "Failed to get video size from decoder";
        return false;
    }

    auto rs = RenderingServer::get_singleton();

    resize_textures(video_width, video_height);

    void* metal_textures[BUFFER_COUNT];
    for (int i = 0; i < BUFFER_COUNT; i++) {
        RenderingServer *rs = RenderingServer::get_singleton();
        RID tex_rid = image_textures[i]->get_rid();
        metal_textures[i] = (void*)rs->texture_get_native_handle(tex_rid);
        if (!metal_textures[i]) {
            //last_error = "Failed to get native Metal texture handle";
            UtilityFunctions::print("Failed to get native Metal texture handle!");
            return false;
        }
    }

    // Now initialize decoder fully with textures and size
    swift_initialize_decoder(file_name.utf8().get_data(), metal_textures, BUFFER_COUNT, video_width, video_height);
    err_str = swift_get_last_error();
    if (err_str) {
        UtilityFunctions::print("create_video_context (real) error: " + String(err_str));
        return false;
    }

    initialized = true;
    current_texture_index = 0;
    last_error = "";
    return true;
}

void VideoPlayerMetal::play() {
    if (!initialized) {
         if (!create_video_context()) {
            return;
         }
    }
    playing = !playing;
}

void VideoPlayerMetal::stop() {
    if (!initialized)
        return;

    swift_cleanup_decoder();
    initialized = false;
    playing = false;
}

bool VideoPlayerMetal::is_playing() {
    return playing;
}

void VideoPlayerMetal::process_video_frame() {    

}

void VideoPlayerMetal::process_audio_frame() {
    // do not use audio frame for now
}

void VideoPlayerMetal::print_last_error() {
    const char* err_str = swift_get_last_error();
    String error_string = err_str ? String(err_str) : "Unknown native error";
    UtilityFunctions::print("Decoder error: " + error_string);
} 

void VideoPlayerMetal::update_frame() {
    if (!initialized)
        return;

    if (!playing) {
        return;
    }

    int64_t now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    if (started_ns == 0) {
        started_ns = now_ns;
    }

    int64_t elapsed_ns = now_ns - started_ns;
    int frame_ready = 0;
    int index = swift_request_decode(elapsed_ns, &frame_ready);
    texture = image_textures[(index+1) % 2];
}
