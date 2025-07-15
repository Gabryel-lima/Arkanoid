// assets/shaders/invert.fs
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;

void main()
{
    vec4 color = texture(texture0, fragTexCoord);
    finalColor = vec4(1.0 - color.rgb, color.a); // Inverte as cores
}