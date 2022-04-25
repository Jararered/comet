#version 460

in vec3 v_Position;
in vec2 v_TextureCoordinates;
in vec3 v_Normal;
in float v_Transparency;

uniform sampler2D u_Texture;
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
    vec4 transparency = vec4(1.0, 1.0, 1.0, v_Transparency);

    vec4 textureColors = texture(u_Texture, v_TextureCoordinates);

    if (textureColors.a == 0.0)
    {
        discard;
    }

    FragColor = textureColors * result * transparency + vec4(u_OverlayColor, 0.0);
}