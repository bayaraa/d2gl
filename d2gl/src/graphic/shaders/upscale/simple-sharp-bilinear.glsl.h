#pragma once

"#ifdef VERTEX\n"
"layout(location=0) in vec2 Position;"
"layout(location=1) in vec2 TexCoord;"
"uniform mat4 u_MVP;"
"out vec2 v_TexCoord;"
"void main()"
"{"
  "gl_Position=u_MVP*vec4(Position,0,1);"
  "v_TexCoord=TexCoord;"
"}"
"\n#elif FRAGMENT\n"
"layout(location=0) out vec4 FragColor;layout(std140) uniform ubo_Metrics{vec2 u_OutSize;vec2 u_TexSize;vec2 u_RelSize;};"
"uniform sampler2D u_Texture;"
"in vec2 v_TexCoord;"
"void main()"
"{"
  "vec2 u=v_TexCoord*u_TexSize,n=max(floor(u_OutSize/u_TexSize),vec2(1)),l=floor(u),v=fract(u),e=.5-.5/n,P=v-.5,d=(P-clamp(P,-e,e))*n+.5;"
  "FragColor=vec4(texture(u_Texture,(l+d)/u_TexSize).xyz,1);"
"}"
"\n#endif"