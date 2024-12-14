#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 color;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	// FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
	vec4 texColor = vec4(color, 1.0f) * texture(texture1, TexCoord);
	// if (texColor.a < 0.1)
		// discard;
	FragColor = texColor;
}