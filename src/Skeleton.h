#pragma once
#ifndef SKELETON_H
#define SKELETON_H

#include <memory>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Program.h"
#include "MatrixStack.h"

class Skeleton
{
private:
	//std::shared_ptr<std::vector<glm::quat>> quat_list;
	//std::shared_ptr<std::vector<glm::vec3>> pos_list;
	std::shared_ptr<std::vector<glm::mat4>> mat_list;
	std::shared_ptr<std::vector<glm::mat4>> base_frame_mats;
	std::shared_ptr<std::vector<glm::mat4>> inv_mat_list;
	int frames;
	int bones;
public:
	Skeleton();
	Skeleton(std::string DATA_DIR, const std::string fname);
	virtual ~Skeleton();

	std::shared_ptr<std::vector<glm::mat4>> get_mat_list() { return mat_list; }
	std::shared_ptr<std::vector<glm::mat4>> get_inv_mat_list() { return inv_mat_list; }
	std::shared_ptr<std::vector<glm::mat4>> get_base_frame_mats() { return base_frame_mats; }

	int get_frames() { return frames; }
	int get_bones() { return bones; }
	void draw_bones(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> MV, std::shared_ptr<MatrixStack> P, int frame_no);
};

#endif