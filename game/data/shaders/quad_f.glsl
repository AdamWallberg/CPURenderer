#version 330

uniform sampler2D uTexture;

in vec2 vTexCoords;

out vec4 fColor;

void main()
{
    fColor = texture(uTexture, vTexCoords);
    //fColor = vec4(1.0, 0.0, 1.0, 1.0);
}