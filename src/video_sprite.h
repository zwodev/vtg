#ifndef VIDEO_SPRITE
#define VIDEO_SPRITE

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include "sav1.h"

namespace godot {

class VideoSprite : public Sprite2D {
    GDCLASS(VideoSprite, Sprite2D)

private:
    double time_passed;
    Ref<Image> image;
    Ref<ImageTexture> texture;
    Sav1Settings settings;
    Sav1Context context = {0};
    String file_name;

    bool create_video_context();
    void process_video_frame();
    void process_audio_frame();

protected:
    static void _bind_methods();

public:
    VideoSprite();
    ~VideoSprite();

    void _ready() override;
    void _process(double delta) override;

    void set_file_name(const String& p_file_name);
    String get_file_name() const;

    void play();

};

}

#endif // VIDEO_SPRITE

