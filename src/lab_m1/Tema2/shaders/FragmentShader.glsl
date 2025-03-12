#version 330

// Input
in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;

uniform float transparency;

void main()
{
    out_color = vec4(frag_color, transparency);
}