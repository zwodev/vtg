#ifndef VIDEO_PLAYER
#define VIDEO_PLAYER

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/texture2d.hpp>

namespace godot {

class VideoPlayer : public RefCounted {
    GDCLASS(VideoPlayer, RefCounted)

public:
    VideoPlayer();
    virtual ~VideoPlayer() = 0;

    void set_file_name(const String& p_file_name);
    String get_file_name() const;

    Ref<Texture2D> get_texture();

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool is_playing() = 0;
    virtual void update_frame() = 0;
    

protected:
    String file_name;
    Ref<Texture2D> texture;
    static void _bind_methods();

};

}

#endif // VIDEO_PLAYER

