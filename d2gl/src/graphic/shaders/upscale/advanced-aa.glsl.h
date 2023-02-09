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
"vec3 y=vec3(1.,1.,1.);"
"void main()"
"{"
  "float u=u_RelSize.x*.5,w=u_RelSize.y*.5;"
  "vec4 e,v,z,r;"
  "e.xy=v_TexCoord+vec2(-u,0);"
  "v.xy=v_TexCoord+vec2(u,0);"
  "z.xy=v_TexCoord+vec2(0,-w);"
  "r.xy=v_TexCoord+vec2(0,w);"
  "e.zw=v_TexCoord+vec2(-u,-w);"
  "v.zw=v_TexCoord+vec2(-u,w);"
  "z.zw=v_TexCoord+vec2(u,-w);"
  "r.zw=v_TexCoord+vec2(u,w);"
  "vec3 d=texture(u_Texture,e.zw).xyz,a=texture(u_Texture,z.xy).xyz,l=texture(u_Texture,z.zw).xyz,s=texture(u_Texture,e.xy).xyz,n=texture(u_Texture,v_TexCoord).xyz,x=texture(u_Texture,v.xy).xyz,t=texture(u_Texture,v.zw).xyz,o=texture(u_Texture,r.xy).xyz,T=texture(u_Texture,r.zw).xyz;"
  "float m=dot(abs(d-T),y)+1e-4,c=dot(abs(l-t),y)+1e-4,i=dot(abs(s-x),y)+1e-4,C=dot(abs(a-o),y)+1e-4,f=.5*(i+C),F=.5*(m+c);"
  "vec3 b=(i*(a+o)+C*(s+x)+f*n)/(2.5*(i+C)),S=(m*(l+t)+c*(d+T)+F*n)/(2.5*(m+c));"
  "f=dot(abs(b-n),y)+1e-4;"
  "F=dot(abs(S-n),y)+1e-4;"
  "FragColor=vec4((f*S+F*b)/(f+F),1.);"
"}",

/* GLSL Compute */
nullptr,