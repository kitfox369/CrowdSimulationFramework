#version 450 core


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
//layout(location = 2) in vec2 aTexCoords;

//layout(location = 3) in ivec4 BoneIDs;
//layout(location = 4) in vec4 Weights;


//out vec2 TexCoord;
out vec3 color;

uniform mat4 mvp;
uniform mat4 modelView;
uniform mat3 NormalMatrix; 

uniform vec4 LightPosition; 
uniform vec3 LightIntensity; 

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shiness;    // Specular shininess factor (phong exponent)
};
uniform MaterialInfo Material;


vec3 ads(vec4 position, vec3 norm)
{
   vec3 s = normalize(vec3(LightPosition-position));

   float l = length(position);
   vec3 v = normalize(vec3(-position));
   vec3 r = reflect(-s,norm);


   //vec3 specular = Material.Ks;
//   if (dot(s, norm) > 0.0) {
//	    specular = LightIntensity * Material.Ks * pow(max(dot(r, v), 0.0),max(50.0, Material.Shiness));
//   }
  // return LightIntensity * (Material.Ka + Material.Kd * max(dot(s, norm), 0.0));

  return LightIntensity * (Material.Ka + Material.Kd * max(dot(s,norm),0.0)) + LightIntensity * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shiness);
}

void main() {

   
	 float l = length(aPos);
   vec3 v = normalize(vec3(-aPos));
   vec3 eyeNorm = normalize(NormalMatrix * aNormal);
   vec4 eyePosition = modelView * vec4(aPos,1.0);
   color = ads(eyePosition,eyeNorm);


   //TexCoord = aTexCoords;
   gl_Position = mvp * vec4(aPos, 1.0f);

}