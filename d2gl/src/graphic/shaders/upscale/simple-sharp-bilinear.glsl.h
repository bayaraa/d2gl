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
"layout(location=0) out vec4 FragColor;layout(std140) uniform ubo_Metrics{vec2 u_OutSize;vec2 u_TexSize;vec2 u_RelSize;};"
"uniform sampler2D u_Texture;"
"in vec2 v_TexCoord;"
"void main()"
"{"
  "vec2 u=v_TexCoord*u_TexSize,e=max(floor(u_OutSize/u_TexSize),vec2(1.,1.)),v=floor(u),f=fract(u),r=.5-.5/e,t=f-.5,n=(t-clamp(t,-r,r))*e+.5,o=v+n;"
  "FragColor=vec4(texture(u_Texture,o/u_TexSize).xyz,1.);"
"}",

/* GLSL Compute */
nullptr,