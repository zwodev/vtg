#ifndef TEXTURE_RENDERER
#define TEXTURE_RENDERER

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/variant/vector2i.hpp>

namespace godot {

class TextureRenderer {
private:
    RID target_tex_rid;
    RID render_target;
    RID canvas_item;
    Ref<Texture2D> output_texture;
    Ref<ShaderMaterial> material;
    Vector2i size;

public:
    TextureRenderer();
    ~TextureRenderer();

    void initialize(const Ref<Texture2D> &source_texture, Vector2i tex_size);
    void update_render();

    Ref<Texture2D> get_output_texture() const;
};

}
#endif // TEXTURE_RENDERER

