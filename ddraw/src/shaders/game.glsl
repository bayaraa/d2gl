#ifdef VERTEX

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;

uniform mat4 u_MVP;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_MVP * vec4(Position, 0.0, 1.0);
	v_TexCoord = TexCoord;
}

// =============================================================
#elif FRAGMENT

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColorMap;

layout(std140) uniform ubo_Colors {
	vec4 u_Palette[256];
};

uniform sampler2D u_Texture;

in vec2 v_TexCoord;

void main()
{
	float red = texture(u_Texture, v_TexCoord).r;
	FragColor = u_Palette[int(red * 255)];
}

#endif
