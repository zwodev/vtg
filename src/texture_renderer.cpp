#include "texture_renderer.h"

using namespace godot;

TextureRenderer::TextureRenderer() : 
    target_tex_rid(RID()), 
    render_target(RID()), 
    canvas_item(RID()), 
    output_texture(nullptr), 
    material(nullptr), 
    size(512, 512) 
{}

TextureRenderer::~TextureRenderer() {
    RenderingServer *rs = RenderingServer::get_singleton();
    // Properly free GPU resources on destruction
    if (canvas_item.is_valid()) {
        rs->free_rid(canvas_item);
        canvas_item = RID();
    }
    if (render_target.is_valid()) {
        rs->free_rid(render_target);
        render_target = RID();
    }
    if (target_tex_rid.is_valid()) {
        rs->free_rid(target_tex_rid);
        target_tex_rid = RID();
    }
    // ShaderMaterial and Texture2D will be cleaned by Ref smart pointers automatically
}

void TextureRenderer::initialize(const Ref<Texture2D> &source_texture, Vector2i tex_size) 
{
    RenderingServer *rs = RenderingServer::get_singleton();

    size = tex_size;

    // Create offscreen render target texture
    target_tex_rid = rs->texture_create(size.x, size.y,
                                        RenderingServer::TEXTURE_TYPE_2D,
                                        RenderingServer::IMAGE_FORMAT_RGBA8,
                                        RenderingServer::TEXTURE_FLAG_FILTER | RenderingServer::TEXTURE_FLAG_RENDER_TARGET);

    // Create render target and attach texture
    render_target = rs->render_target_create(size.x, size.y, RenderingServer::RENDER_BUFFER_COLOR, 1);
    rs->render_target_set_color_texture(render_target, target_tex_rid);

    // Create CanvasItem used for issuing draw commands
    canvas_item = rs->canvas_item_create();

    // Create ShaderMaterial with custom fragment shader for color inversion
    material.instantiate();
    Ref<Shader> shader = memnew(Shader);
    shader->set_code(R"(
        shader_type canvas_item;
        uniform sampler2D source_texture;

        void fragment() {
            vec4 tex_color = texture(source_texture, UV);
            COLOR = vec4(vec3(1.0) - tex_color.rgb, tex_color.a);
        }
    )");
    material->set_shader(shader);
    material->set_shader_parameter("source_texture", source_texture);

    // Wrap render target texture RID in Texture2D resource for use in Godot
    output_texture.instantiate();
    output_texture->set_rid(target_tex_rid);
}

void TextureRenderer::update_render() 
{
    RenderingServer *rs = RenderingServer::get_singleton();

    rs->render_target_begin(render_target);
    rs->viewport_clear(render_target, Color(0, 0, 0, 1), true);

    // Draw full rectangle covering target with shader material
    Rect2 rect(Vector2(0, 0), Vector2(size.x, size.y));
    rs->canvas_item_add_rectangle(canvas_item, rect, Color(1, 1, 1, 1));
    rs->canvas_item_set_material(canvas_item, material->get_rid());

    // Execute draw call on the render target using the canvas item
    rs->render_target_draw(canvas_item);

    rs->render_target_end();
}

Ref<Texture2D> TextureRenderer::get_output_texture() const 
{
    return output_texture;
}

