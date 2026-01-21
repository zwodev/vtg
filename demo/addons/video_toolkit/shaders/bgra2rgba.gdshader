shader_type canvas_item;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	
	vec3 color = texture(TEXTURE, vec2(UV.x, UV.y)).rgb;
	
	COLOR = vec4(color.b, color.g, color.r, 1.0);

	
}
