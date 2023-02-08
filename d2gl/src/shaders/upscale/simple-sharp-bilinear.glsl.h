#pragma once

/* GLSL Vertex */
"#version 330\n"
  "layout(binding = 0, std140) uniform ubo_MVPs{mat4 u_mvp_game;mat4 u_mvp_upscale;mat4 u_mvp_movie;mat4 u_mvp_normal;};"
  "layout(location=0) in vec2 Position;"
  "layout(location=1) in vec2 TexCoord;"
  "layout(location=0) out vec2 v_TexCoord;"
  "void main()"
  "{"
  "gl_Position=u_mvp_upscale*vec4(Position,0.,1.),v_TexCoord=TexCoord;"
  "}",

  /* GLSL Fragment */
  "#version 330\n"
  "layout(binding = 1, std140) uniform ubo_Sizes{vec2 u_OutSize;vec2 u_TexSize;vec2 u_RelSize;};"
  "layout(binding=2) uniform sampler2D u_Texture;"
  "layout(location=0) in vec2 v_TexCoord;"
  "layout(location=0) out vec4 FragColor;"
  "void main()"
  "{"
  "vec2 u=v_TexCoord*u_TexSize,e=max(floor(u_OutSize/u_TexSize),vec2(1.,1.)),v=floor(u),n=fract(u),r=.5-.5/e,l=n-.5,d=(l-clamp(l,-r,r))*e+.5,g=v+d;"
  "FragColor=vec4(texture(u_Texture,g/u_TexSize).xyz,1.);"
  "}",

  /* GLSL Compute */
  nullptr,
