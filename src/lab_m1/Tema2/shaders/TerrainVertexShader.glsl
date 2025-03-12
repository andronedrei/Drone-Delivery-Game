#version 330 core

// Atribute de intrare
layout(location = 0) in vec3 v_position;
// layout(location = 1) in vec3 v_normal;
// layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniforme
uniform mat4 View;
uniform mat4 Projection;
uniform float max_height;

// Output c?tre Fragment Shader
out vec3 frag_color;
out float noise_factor;

// Func?ie simplificat? de zgomot care returneaz? valori între -1 ?i 1
float noise(vec3 x) 
{
    // Partea întreag? a coordonatei
    vec3 p = floor(x);
    // Partea frac?ionar? a coordonatei
    vec3 f = fract(x);
    
    f = f * f * (3.0 - 2.0 * f);
    
    // Generam valori semi-aleatoare
    float hash = fract(sin(dot(p, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
    
    return mix(-1.0, 1.0, hash);
}

void main()
{
    frag_color = v_color;

    // Calcularea factorului de zgomot folosind func?ia simplificat?
    noise_factor = noise(v_position);
    float h = noise_factor * max_height;

    gl_Position = Projection * View * vec4(v_position.x, v_position.y + h, v_position.z, 1.0);
}
