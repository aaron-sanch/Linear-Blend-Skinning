#include <iostream>
#include <fstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "TextureMatrix.h"

using namespace std;
using namespace glm;

TextureMatrix::TextureMatrix()
{
	type = Type::NONE;
	T = mat3(1.0f);
}

TextureMatrix::~TextureMatrix()
{
	
}

void TextureMatrix::setType(const string &str)
{
	if(str.find("Body") != string::npos) {
		type = Type::BODY;
	} else if(str.find("Mouth") != string::npos) {
		type = Type::MOUTH;
	} else if(str.find("Eyes") != string::npos) {
		type = Type::EYES;
	} else if(str.find("Brows") != string::npos) {
		type = Type::BROWS;
	} else {
		type = Type::NONE;
	}
}

void TextureMatrix::update(unsigned int key)
{
	// Update T here
	
	if(type == Type::BODY) {
		// Do nothing
	} else if(type == Type::MOUTH) {
		// TODO
		if (key == 'm') {
			glm::vec3 e = T[2];
			float x = e.x;
			e[0] = fmod(x + .1, .3);
			T[2] = e;
		}
		else if (key == 'M') {
			glm::vec3 e = T[2];
			float y = e.y;
			e[1] = fmod(y + .1, 1);
			T[2] = e;
		}
	} else if(type == Type::EYES) {
		// TODO
		if (key == 'e') {
			glm::vec3 e = T[2];
			float x = e.x;
			e[0] = fmod(x + .1, .3);
			T[2] = e;
		}
		else if (key == 'E') {
			glm::vec3 e = T[2];
			float y = e.y;
			e[1] = fmod(y + .1, 1);
			T[2] = e;
		}
	} else if(type == Type::BROWS) {
		// TODO
		if (key == 'b') {
			glm::vec3 e = T[2];
			float y = e.y;
			e[1] = fmod(y + .1, 1);
			T[2] = e;
		}
	}
}
