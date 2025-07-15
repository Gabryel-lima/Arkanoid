#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;

void main()
{
    float offset = 1.0 / 300.0; // Ajuste para mais/menos blur
    vec4 result = vec4(0.0);

    // 9-sample box blur
    result += texture(texture0, fragTexCoord + vec2(-offset, -offset)) * 0.0625;
    result += texture(texture0, fragTexCoord + vec2( 0.0,   -offset)) * 0.125;
    result += texture(texture0, fragTexCoord + vec2( offset, -offset)) * 0.0625;

    result += texture(texture0, fragTexCoord + vec2(-offset,  0.0)) * 0.125;
    result += texture(texture0, fragTexCoord) * 0.25;
    result += texture(texture0, fragTexCoord + vec2( offset,  0.0)) * 0.125;

    result += texture(texture0, fragTexCoord + vec2(-offset,  offset)) * 0.0625;
    result += texture(texture0, fragTexCoord + vec2( 0.0,    offset)) * 0.125;
    result += texture(texture0, fragTexCoord + vec2( offset,  offset)) * 0.0625;

    finalColor = result;
}