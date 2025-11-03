shader_type canvas_item;

uniform sampler2D camera_y;
uniform sampler2D camera_cbcr;

void fragment() {
    float y = texture(camera_y, UV).r;
    vec2 cbcr = texture(camera_cbcr, UV).rg;
    float cb = cbcr.r - 0.5;
    float cr = cbcr.g - 0.5;

    float r = y + 1.402 * cr;
    float g = y - 0.344136 * cb - 0.714136 * cr;
    float b = y + 1.772 * cb;

    COLOR = vec4(r, g, b, 1.0);
}