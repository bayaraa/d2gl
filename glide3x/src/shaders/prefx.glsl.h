#pragma once

/* GLSL Vertex */
"#version 330\n"
  "layout(std140) uniform ubo_MVPs{mat4 u_mvp_game;mat4 u_mvp_upscale;mat4 u_mvp_movie;mat4 u_mvp_normal;};"
  "layout(location=0) in vec2 Position;"
  "layout(location=1) in vec2 TexCoord;"
  "layout(location=5) in ivec4 Flags;"
  "out vec2 v_TexCoord;"
  "flat out ivec4 v_Flags;"
  "void main()"
  "{"
  "gl_Position=u_mvp_normal*vec4(Position,0.,1.),v_TexCoord=TexCoord,v_Flags=Flags;"
  "}",

  /* GLSL Fragment */
  "#version 330\n"
  "uniform sampler2D u_Texture;"
  "uniform sampler2DArray u_LUTTexture;"
  "in vec2 v_TexCoord;"
  "flat in ivec4 v_Flags;"
  "layout(location=0) out vec4 FragColor;"
  "layout(location=1) out vec4 FragColorMap;"
  "vec4 v(vec4 v,vec4 F,float o)"
  "{"
  "return v.z<1.?mix(v,F,o):v;"
  "}"
  "vec4 v(vec4 o)"
  "{"
  "float F=(o.x*31.+.4999)/1024.,r=(o.y*31.+.4999)/32.,e=floor(o.z*31.)/32.+F,u=ceil(o.z+3.1e-5)/32.+F,x=clamp(max((o.z-e)/(u-e),0.),0.,32.);"
  "vec4 l=texture(u_LUTTexture,vec3(vec2(e,r),v_Flags.x)),n=texture(u_LUTTexture,vec3(vec2(u,r),v_Flags.x));"
  "return v(l,n,x);"
  "}"
  "void main()"
  "{"
  "FragColor=texture(u_Texture,v_TexCoord),FragColor=v(FragColor),FragColorMap=vec4(0.,0.,0.,0.);"
  "}",

  /* GLSL Compute */
  nullptr,
