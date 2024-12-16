shader_type canvas_item;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	float x = floor(UV.x / TEXTURE_PIXEL_SIZE.x);
	float y = floor(UV.y / TEXTURE_PIXEL_SIZE.y);

	// get Y
	int Y_ix = int(mod(x, 2.0));
	int Y_iy = int(mod(y, 2.0));
	float Y_u = (UV.x + float(Y_iy)) * 0.5;
	float Y_v = UV.y * 0.5;
	vec2 pixel = texture(TEXTURE, vec2(Y_u, Y_v)).rg;
	float Y = pixel[Y_ix];

	// Get chroma (I420)
	x = floor(x / 2.0);
	y = floor(y / 2.0);
	int UV_ix = int(mod(x, 2.0));
	int UV_iy = int(mod(y, 4.0));
	float UV_u = (UV.x + float(UV_iy)) * 0.25;
	float UV_v = UV.y * 0.125;

	// Get U
	vec2 U_pixel = texture(TEXTURE, vec2(UV_u, 0.5 + UV_v)).rg;
	float U = U_pixel[UV_ix] - 0.5;

	// Get V
	vec2 V_pixel = texture(TEXTURE, vec2(UV_u, 0.625 + UV_v)).rg;
	float V = V_pixel[UV_ix] - 0.5;

	// YUV to RGB
	float r = Y + (1.403 * V);
	float g = Y - (0.344 * U) - (0.714 * V);
	float b = Y + (1.770 * U);

	COLOR = vec4(r, g, b, 1.0);

	// DEBUGGING
	//COLOR = vec4(float(Y_iy) * 0.8, 0, 0, 1.0);
	//COLOR = vec4(U, U, U, 1.0);
}
