#version 330 core
in vec3 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;

uniform sampler2D mySampler;
uniform float TIME;

void main()
{
    vec2 movedUV = fragmentUV;// + vec2(TIME, TIME);
    vec4 textureColor = texture(mySampler, movedUV);

    if (textureColor.a <= 0.0)
        discard;
    
    color = fragmentColor * textureColor;
}
