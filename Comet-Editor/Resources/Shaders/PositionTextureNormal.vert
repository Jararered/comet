#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 mvp;

out vec3 v_Position;
out vec2 v_TextureCoordinates;
out vec3 v_Normal;
flat out vec2 v_TextureTile;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1.0);
    v_Position = vertexPosition;
    v_Normal = vertexNormal;

    v_TextureTile = vec2(-1.0, -1.0);
    v_TextureCoordinates = vertexTexCoord;
    if (vertexTexCoord.x < 0.0)
    {
        const float greedyUvScale = 1024.0;
        vec2 encoded = -vertexTexCoord;
        v_TextureTile = floor(encoded) - vec2(1.0, 1.0);
        v_TextureCoordinates = fract(encoded) * greedyUvScale;
    }
}
