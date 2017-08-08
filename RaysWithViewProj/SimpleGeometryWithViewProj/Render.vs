#version 450 core
precision highp float;

layout (location=0) in vec3 in_Position;
layout (location=1) in vec4 in_Color;
layout (location=2) in float in_Size;

uniform mat4 view;
uniform mat4 projection;

out vec4 color;

void main() 
{
  gl_Position = projection * view  * vec4(in_Position, 1.0);	
  color = in_Color;
  gl_PointSize = in_Size;
}