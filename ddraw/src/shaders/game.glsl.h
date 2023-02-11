#pragma once

/* GLSL Vertex */
"#version 330\n"
"layout(location=0) in vec2 Position;"
"layout(location=1) in vec2 TexCoord;"
"uniform mat4 u_MVP;"
"out vec2 v_TexCoord;"
"void main()"
"{"
  "gl_Position=u_MVP*vec4(Position,0.,1.),v_TexCoord=TexCoord;"
"}",

/* GLSL Fragment */
"#version 330\n"
"layout(location=0) out vec4 FragColor;"
"layout(location=1) out vec4 FragColorMap;layout(std140) uniform ubo_Colors{vec4 u_Palette[256];};"
"uniform sampler2D u_Texture;"
"in vec2 v_TexCoord;"
"void main()"
"{"
  "float u=texture(u_Texture,v_TexCoord).x;"
  "FragColor=u_Palette[int(u*255)];"
"}",

/* GLSL Compute */
nullptr,