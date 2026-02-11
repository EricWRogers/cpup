#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform float time;
void main()
{
   gl_Position = vec4(aPos, 1.0);
   gl_Position.x += sin(time) * 0.5f;
   gl_Position.y += cos(time) * 0.5f;
   ourColor = abs(gl_Position.rgb);
   TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}