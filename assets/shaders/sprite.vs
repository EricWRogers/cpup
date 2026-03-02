#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec2 vertexUV;

out vec3 fragmentPosition;
out vec4 fragmentColor;
out vec2 fragmentUV;

uniform mat4 P;
uniform float TIME;

void main()
{
    gl_Position = P * vec4(vertexPosition, 1.0);
    fragmentPosition = vertexPosition;
    fragmentColor = vertexColor;
    fragmentUV = vertexUV;
}
