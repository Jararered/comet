#version 330

in vec2 v_TextureCoordinates;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_Texture, v_TextureCoordinates);
}
