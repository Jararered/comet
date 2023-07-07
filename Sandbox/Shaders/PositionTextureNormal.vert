#version 330

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TextureCoordinates;
layout (location = 2) in vec3 a_Normal;

uniform ivec3 u_Index;
uniform mat4 u_ProjMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelMatrix;

out vec3 v_Position;
out vec2 v_TextureCoordinates;
out vec3 v_Normal;

void main()
{
    vec3 pos = vec3(float(u_Index.x) * 16.0 + a_Position.x, a_Position.y, float(u_Index.z) * 16.0 + a_Position.z);
    gl_Position = u_ProjMatrix * u_ViewMatrix * u_ModelMatrix * vec4(pos, 1.0);

    v_Position = a_Position;
    v_TextureCoordinates = a_TextureCoordinates;
    v_Normal = a_Normal;
}