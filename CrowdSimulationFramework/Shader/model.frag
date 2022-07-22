#version 450 core 

layout(location = 0) out vec4 FragColor;

//in vec2 TexCoord;
in vec3 color;

//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

//uniform int hasTextureDiffuse;
//uniform int hasTextureSpecular;

void main()
{
	 FragColor = vec4(color, 1.0);
	 
//		 if (hasTextureDiffuse == 1) {
//			 vec4 diffuse = texture(texture_diffuse1, TexCoord);
//		
//			 FragColor = (vec4( color, 1.0 ) * diffuse);
//		 }
//
//		 if (hasTextureSpecular == 1) {
//			 vec4 specular = texture(texture_specular1, TexCoord).rgba;
//			 FragColor += specular;
//		 }	
	//	 color = vec4( Color, 1.0 ); 

}
