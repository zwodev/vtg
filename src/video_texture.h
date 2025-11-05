#ifndef VIDEO_TEXTURE
#define VIDEO_TEXTURE

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include "sav1.h"

namespace godot {

class VideoTexture : public ImageTexture {
    GDCLASS(VideoTexture, ImageTexture)

private:
    double time_passed;
    Ref<Image> image;

    Sav1Settings settings;
    Sav1Context context = {0};
    String file_name;

    bool create_video_context();
    void process_video_frame();
    void process_audio_frame();

protected:
    static void _bind_methods();

public:
    VideoTexture();
    ~VideoTexture();

    void update_frame();

    void set_file_name(const String& p_file_name);
    String get_file_name() const;

    void play();
    void stop();
    bool is_playing();
};

}

#endif // VIDEO_TEXTURE

