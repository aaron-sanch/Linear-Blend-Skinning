#pragma once
#ifndef TEXTURE_MATRIX_H
#define TEXTURE_MATRIX_H

#include <glm/fwd.hpp>
#include <glm/gtx/string_cast.hpp>

class TextureMatrix
{
public:
	enum Type
	{
		BODY,
		MOUTH,
		EYES,
		BROWS,
		NONE
	};
	
	TextureMatrix();
	virtual ~TextureMatrix();
	void setType(const std::string &str);
	void update(unsigned int key);
	glm::mat3 getMatrix() const { return T; }
	
private:
	Type type;
	glm::mat3 T;
};

#endif
