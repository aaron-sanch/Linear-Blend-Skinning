#include <iostream>
#include <fstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "ShapeSkin.h"
#include "GLSL.h"
#include "Program.h"
#include "TextureMatrix.h"

using namespace std;
using namespace glm;

ShapeSkin::ShapeSkin() :
	prog(NULL),
	elemBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0),
	bones(0),
	vertices(0),
	max_influences(0)
{
	T = make_shared<TextureMatrix>();
	weight_list = make_shared<vector<float>>();
	bone_list = make_shared<vector<float>>();
}

ShapeSkin::~ShapeSkin()
{
}

void ShapeSkin::setTextureMatrixType(const std::string& meshName)
{
	T->setType(meshName);
}

void ShapeSkin::loadMesh(const string& meshName)
{
	// Load geometry
	// This works only if the OBJ file has the same indices for v/n/t.
	// In other words, the 'f' lines must look like:
	// f 70/70/70 41/41/41 67/67/67
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	string warnStr, errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnStr, &errStr, meshName.c_str());
	if (!rc) {
		cerr << errStr << endl;
	}
	else {
		posBuf = attrib.vertices;
		norBuf = attrib.normals;
		texBuf = attrib.texcoords;
		assert(posBuf.size() == norBuf.size());
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			const tinyobj::mesh_t& mesh = shapes[s].mesh;
			size_t index_offset = 0;
			for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
				size_t fv = mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = mesh.indices[index_offset + v];
					elemBuf.push_back(idx.vertex_index);
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}

void ShapeSkin::loadAttachment(const std::string& filename)
{
	// TODO
}

void ShapeSkin::init()
{
	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);

	// Send the normal array to the GPU
	glGenBuffers(1, &norBufID);
	glBindBuffer(GL_ARRAY_BUFFER, norBufID);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);

	// Send the texcoord array to the GPU
	glGenBuffers(1, &texBufID);
	glBindBuffer(GL_ARRAY_BUFFER, texBufID);
	glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), &texBuf[0], GL_STATIC_DRAW);

	// Send the element array to the GPU
	glGenBuffers(1, &elemBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elemBuf.size() * sizeof(unsigned int), &elemBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLSL::checkError(GET_FILE_LINE);
}

void ShapeSkin::update(int k, std::shared_ptr<Skeleton> skeleton)
{
	// TODO: CPU skinning calculations.
	// After computing the new positions and normals, send the new data
	// to the GPU by copying and pasting the relevant code from the
	// init() function.

	std::vector<float> currPosBuf;
	std::vector<float> currNorBuf;

	// find all of the Mj(k) * Mj(0)-1 matrices for k
	vector<glm::mat4> mult_mat_list;
	mult_mat_list.clear();
	for (int j = 0; j < bones; j++) {
		if (k != 0) {
			glm::mat4 M = skeleton->get_mat_list()->at((k - 1) * bones + j) * skeleton->get_inv_mat_list()->at(j);
			mult_mat_list.push_back(M);
		}
	}
	if (k != 0) {

		for (int i = 0; i < vertices; i++) {
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 nor = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec4 p1 = glm::vec4(posBuf.at(i * 3), posBuf.at(i * 3 + 1), posBuf.at(i * 3 + 2), 1.0f);
			glm::vec4 n1 = glm::vec4(norBuf.at(i * 3), norBuf.at(i * 3 + 1), norBuf.at(i * 3 + 2), 0.0f);
			for (int j = 0; j < max_influences; j++) {
				glm::vec4 p2 = weight_list->at(i * max_influences + j) * mult_mat_list.at(bone_list->at(i * max_influences + j)) * p1;
				pos += glm::vec3(p2.x, p2.y, p2.z);
				glm::vec4 n2 = weight_list->at(i * max_influences + j) * (mult_mat_list.at(bone_list->at(i * max_influences + j)) * n1);
				nor += glm::vec3(n2.x, n2.y, n2.z);
			}
			nor = glm::normalize(nor);
			currPosBuf.push_back(pos.x);
			currPosBuf.push_back(pos.y);
			currPosBuf.push_back(pos.z);
			currNorBuf.push_back(nor.x);
			currNorBuf.push_back(nor.y);
			currNorBuf.push_back(nor.z);
		}
		glBindBuffer(GL_ARRAY_BUFFER, posBufID);
		glBufferData(GL_ARRAY_BUFFER, currPosBuf.size() * sizeof(float), &currPosBuf[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, currNorBuf.size() * sizeof(float), &currNorBuf[0], GL_DYNAMIC_DRAW);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, posBufID);
		glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_DYNAMIC_DRAW);
	}

	GLSL::checkError(GET_FILE_LINE);
}

void ShapeSkin::draw(int k) const
{
	assert(prog);

	// Send texture matrix
	glUniformMatrix3fv(prog->getUniform("T"), 1, GL_FALSE, glm::value_ptr(T->getMatrix()));

	int h_pos = prog->getAttribute("aPos");
	glEnableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	int h_nor = prog->getAttribute("aNor");
	glEnableVertexAttribArray(h_nor);
	glBindBuffer(GL_ARRAY_BUFFER, norBufID);
	glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	int h_tex = prog->getAttribute("aTex");
	glEnableVertexAttribArray(h_tex);
	glBindBuffer(GL_ARRAY_BUFFER, texBufID);
	glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	// Draw
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemBufID);
	glDrawElements(GL_TRIANGLES, (int)elemBuf.size(), GL_UNSIGNED_INT, (const void*)0);

	glDisableVertexAttribArray(h_nor);
	glDisableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLSL::checkError(GET_FILE_LINE);
}

void ShapeSkin::load_vertex_weights(std::string DATA_DIR, std::string fname)
{
	int index = 0;
	string filename = DATA_DIR + fname;
	ifstream in;
	in.open(filename);
	if (!in.good()) {
		cout << "Cannot read " << filename << endl;
		return;
	}
	cout << "Loading " << filename << endl;

	string line;
	while (1) {
		getline(in, line);
		if (in.eof()) {
			break;
		}
		if (line.empty()) {
			index++;
			continue;
		}
		// Skip comments
		if (line.at(0) == '#') {
			index++;
			continue;
		}
		// Parse lines and create the value
		if (index == 4) {
			stringstream ss(line);
			ss >> vertices >> bones >> max_influences;
		}
		else if (index > 4) {
			stringstream ss(line);
			int influences, bone_no, count = 0;
			float weight = 0;
			ss >> influences;
			
			for (int i = 0; i < influences; i++) {
				ss >> bone_no;
				ss >> weight;
				weight_list->push_back(weight);
				bone_list->push_back(bone_no);
			}
			for (int i = influences; i < max_influences; i++) {
				weight_list->push_back(0);
				bone_list->push_back(0);
			}
			
		}
		index++;
	
	}
	in.close();
}
