#version 330 core

// Inputs from Vertex Shader
in vec3 frag_color;
in float noise_factor;

// Uniforms
uniform vec3 color_mix_1;
uniform vec3 color_mix_2;

// Output Color
layout(location = 0) out vec4 out_color;

void main()
{
    // Blend colors based on noise_factor (height)
    vec3 mixed_color = mix(color_mix_1, color_mix_2, noise_factor);

    // Output the final color with full opacity
    out_color = vec4(mixed_color, 1.0); // Use 'mixed_color' instead of 'final_color'
}

