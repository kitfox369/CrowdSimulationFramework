#version 450 core 

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;
in vec3 color;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;


void main()
{
	 FragColor = mix(texture(texture_diffuse1,TexCoord),vec4(color,1.0),0.2);
}
