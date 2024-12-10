#ifndef VIDEO_SPRITE
#define VIDEO_SPRITE

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>

namespace godot {

class VideoSprite : public Sprite2D {
    GDCLASS(VideoSprite, Sprite2D)

private:
    double time_passed;
    Ref<Image> image;
    Ref<ImageTexture> texture;

protected:
static void _bind_methods();

public:
    VideoSprite();
    ~VideoSprite();

    void _ready();
    void _process(double delta) override;

};

}

#endif // VIDEO_SPRITE

