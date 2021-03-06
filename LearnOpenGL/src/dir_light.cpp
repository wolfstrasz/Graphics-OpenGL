#include "dir_light.h"


DirLight::DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	mDirection = direction;
	mAmbient = ambient;
	mDiffuse = diffuse;
	mSpecular = specular;
}

void DirLight::setLight(Shader & shader, unsigned int pos)
{
	std::string index = std::to_string(pos);
	shader.setVec3("dirLights[" + index + "].direction",mDirection);
	shader.setVec3("dirLights[" + index + "].ambient",	mAmbient);
	shader.setVec3("dirLights[" + index + "].diffuse",	mDiffuse);
	shader.setVec3("dirLights[" + index + "].specular",	mSpecular);
}

void DirLight::setVec3(Dir_Light_Vector_Attributes attrib, glm::vec3 value)
{
	if (attrib == DIRECTION)
		mDirection = value;
	if (attrib == AMBIENT)
		mAmbient == value;
	if (attrib == DIFFUSE)
		mDiffuse = value;
	if (attrib == SPECULAR)
		mSpecular = value;
}
