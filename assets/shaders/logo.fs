#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;
void main()
{
   vec4 imageColor = texture(MAIN_TEXTURE, TexCoord);

   if (imageColor.a <= 0.0f)
      discard;
   
   FragColor = imageColor * COLOR;
}