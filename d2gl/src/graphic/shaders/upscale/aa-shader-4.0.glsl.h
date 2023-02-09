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
"vec3 u=vec3(1.,1.,1.);"
"vec3 t(sampler2D e,vec2 x,vec4 y)"
"{"
  "vec3 a=texture(e,x+y.zw).xyz,v=texture(e,x+y.xw).xyz,w=texture(e,x+y.xy).xyz,s=texture(e,x+y.zy).xyz;"
  "float t=dot(abs(a-w),u)+.001,r=dot(abs(s-v),u)+.001;"
  "return.5*(r*(a+w)+t*(s+v))/(t+r);"
"}"
"void main()"
"{"
  "vec2 e=4.*u_TexSize,a=1./e;"
  "vec4 v=vec4(a,-a);"
  "vec2 y=v_TexCoord*e,x=fract(y),r=vec2(a.x,0.),w=vec2(0.,a.y),z=vec2(a.x,a.y),s=vec2(-a.x,a.y),n=floor(y)*a;"
  "vec3 o=t(u_Texture,n-z,v),l=t(u_Texture,n-w,v),c=t(u_Texture,n-s,v),T=t(u_Texture,n-r,v),d=t(u_Texture,n,v),i=t(u_Texture,n+r,v),b=t(u_Texture,n+s,v),f=t(u_Texture,n+w,v),m=t(u_Texture,n+z,v),F,S,C,D;"
  "float p,g;"
  "p=dot(abs(o-d),u)+.001;"
  "g=dot(abs(l-T),u)+.001;"
  "F=(g*(o+d)+p*(l+T))/(p+g);"
  "p=dot(abs(l-i),u)+.001;"
  "g=dot(abs(c-d),u)+.001;"
  "S=(g*(l+i)+p*(c+d))/(p+g);"
  "p=dot(abs(T-f),u)+.001;"
  "g=dot(abs(b-d),u)+.001;"
  "C=(g*(T+f)+p*(b+d))/(p+g);"
  "p=dot(abs(d-m),u)+.001;"
  "g=dot(abs(i-f),u)+.001;"
  "D=(g*(d+m)+p*(i+f))/(p+g);"
  "vec3 R=.5*((D*x.x+C*(1.-x.x))*x.y+(S*x.x+F*(1.-x.x))*(1.-x.y));"
  "FragColor=vec4(R,1.);"
"}",

/* GLSL Compute */
nullptr,