#pragma once
#ifndef _DIR_LIGHT_H
#define _DIR_LIGHT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string.h>
#include "shader.h"

struct Dir_Light_Defaults
{
	static constexpr glm::vec3 DIRECTION = glm::vec3(-0.2f, -1.0f, -0.3f);
	static constexpr glm::vec3 AMBIENT = glm::vec3(0.05f, 0.05f, 0.05f);
	static constexpr glm::vec3 DIFFUSE = glm::vec3(0.4f, 0.4f, 0.4f);
	static constexpr glm::vec3 SPECULAR = glm::vec3(0.5f, 0.5f, 0.5f);
};
class DirLight
{
public:
	enum Dir_Light_Vector_Attributes
	{
		DIRECTION,
		AMBIENT,
		DIFFUSE,
		SPECULAR
	};

	DirLight(glm::vec3 direction = Dir_Light_Defaults::DIRECTION,
			glm::vec3 ambient = Dir_Light_Defaults::AMBIENT,
			glm::vec3 diffuse = Dir_Light_Defaults::DIFFUSE,
			glm::vec3 specular = Dir_Light_Defaults::SPECULAR);

private:
	glm::vec3 mDirection;
	glm::vec3 mAmbient;
	glm::vec3 mDiffuse;
	glm::vec3 mSpecular;

public:
	void setLight(Shader& shader, unsigned int pos);
	void setVec3(Dir_Light_Vector_Attributes attrib, glm::vec3 value);
};
#endif // !_DIR_LIGHT
