#ifndef VIDEO_PLAYER_SOFT
#define VIDEO_PLAYER_SOFT

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>

#include "video_player.h"
#include "sav1.h"

namespace godot {

class VideoPlayerSoft : public VideoPlayer {
    GDCLASS(VideoPlayerSoft, VideoPlayer)

public:
    VideoPlayerSoft();
    ~VideoPlayerSoft();

    void play() override;
    void stop() override;
    bool is_playing() override;
    void update_frame() override;

protected:
    static void _bind_methods();

private:
    double time_passed;
    Ref<ImageTexture> image_texture;
    Ref<Image> image;

    Sav1Settings settings;
    Sav1Context context = {0};

    bool create_video_context();
    void process_video_frame();
    void process_audio_frame();
};

}

#endif // VIDEO_PLAYER_SOFT

