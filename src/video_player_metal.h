#ifndef VIDEO_PLAYER_METAL
#define VIDEO_PLAYER_METAL

#include "video_player.h"

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>

namespace godot {

class VideoPlayerMetal : public VideoPlayer {
    GDCLASS(VideoPlayerMetal, VideoPlayer)

public:
    VideoPlayerMetal();
    ~VideoPlayerMetal();

    void play() override;
    void stop() override;
    bool is_playing() override;
    void update_frame() override;

protected:
    static void _bind_methods();

private:
    void create_textures();
    void resize_textures(int width, int height);
    bool create_video_context();
    void process_video_frame();
    void process_audio_frame();
    void cleanup();
    void print_last_error();

private:
    static constexpr int BUFFER_COUNT = 2;
    Ref<ImageTexture> image_textures[BUFFER_COUNT];
    Ref<Image> image;

    
    //RID texture_rids[BUFFER_COUNT];
    int current_texture_index = 0;
    bool initialized = false;
    bool playing = false;
    String last_error;
    int64_t started_ns = 0;
};

}

#endif // VIDEO_PLAYER_METAL

