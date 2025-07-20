#shader vertex
#version 460 core
layout (location = 0) in vec3 in_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(in_position, 1.0);
}

#shader fragment
#version 460 core
out vec4 out_fragment_color;

uniform vec3 light_color;

void main()
{
    out_fragment_color = vec4(light_color, 1.0);
}