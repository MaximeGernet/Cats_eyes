#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform int type;
uniform float transparency;
uniform int override_transparency;

void main()
{
	if(type == 1)		// Texture from font
	{
		vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
		color = vec4(textColor, 1.0) * sampled;
	}
	else if(type == 2) 	// Texture from RGB array
	{
		color = texture(text, TexCoords);
		if(override_transparency == 1)
			color.w = transparency;
		//color = vec4(1.0f);
	}
	else if(type == 3)	// Line
	{
		color = vec4(0.5f, 0.0f, 1.0f, 1.0f);
	}
}




