#version 330

in vec3 v_Position;
in vec2 v_TextureCoordinates;
in vec3 v_Normal;
flat in vec2 v_TextureTile;

uniform sampler2D texture0;
uniform vec3 u_OverlayColor;

out vec4 FragColor;

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // ambient
    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = vec3(0.5, 1.0, 0.75);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec4 result = vec4((ambient + diffuse) / 2.0, 1.0);
    vec4 transparency = vec4(1.0, 1.0, 1.0, 1.0);

    vec2 textureCoordinates = v_TextureCoordinates;
    if (v_TextureTile.x >= 0.0)
    {
        const vec2 tileSize = vec2(1.0 / 16.0, 1.0 / 16.0);
        vec2 tileOrigin = v_TextureTile * tileSize;
        vec2 tiledCoordinates = fract(v_TextureCoordinates);
        textureCoordinates = tileOrigin + tiledCoordinates * tileSize;
    }

    vec4 textureColors = texture(texture0, textureCoordinates);

    if (textureColors.a == 0.0)
    {
        discard;
    }

    FragColor = textureColors * result * transparency + vec4(u_OverlayColor, 0.0);
}
