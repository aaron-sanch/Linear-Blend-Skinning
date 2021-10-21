#include "Skeleton.h"

using namespace std;

Skeleton::Skeleton()
{
}

Skeleton::Skeleton(std::string DATA_DIR, const std::string fname)
{
	int index = 0;
	base_frame_mats = make_shared<vector<glm::mat4>>();
	mat_list = make_shared<vector<glm::mat4>>();
	inv_mat_list = make_shared<vector<glm::mat4>>();
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
		if (index == 3) {
			int frame_no, bone_no;
			stringstream ss(line);
			ss >> frame_no >> bone_no;
			frames = frame_no;
			bones = bone_no;
		}
		else if (index >= 4) {
			double qx, qy, qz, qw, px, py, pz;
			stringstream ss(line);
			// get all our values
			while (ss >> qx >> qy >> qz >> qw >> px >> py >> pz) {
				// These arent doing anything right now, just creating them
				glm::quat q = glm::quat(qw, qx, qy, qz);
				glm::vec3 p = glm::vec3(px, py, pz);
				if (index == 4) {
					glm::mat4 M = mat4_cast(q);
					M[3] = glm::vec4(p, 1.0f);
					base_frame_mats->push_back(M);
					inv_mat_list->push_back(glm::inverse(M));
					

				}
				else {
					glm::mat4 M = mat4_cast(q);
					M[3] = glm::vec4(p, 1.0f);
					mat_list->push_back(M);
					
				}
			}
		}
		index++;
	}
	in.close();
}

Skeleton::~Skeleton()
{
}

void Skeleton::draw_bones(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> MV, std::shared_ptr<MatrixStack> P, int frame_no)
{
	// Draw each bone
	for (int i = 0; i < bones; i++) {
		MV->pushMatrix();
			if (frame_no == 0) {
				MV->multMatrix(base_frame_mats->at(i));
			}
			else {
				MV->multMatrix(mat_list->at((bones * (frame_no - 1)) + i));
			}
			prog->bind();
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
			float gridSizeHalf = 200.0f;
			int gridNx = 11;
			int gridNz = 11;
			glLineWidth(1);
			glColor3f(0.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 5.0f);
			glEnd();

			glLineWidth(1);
			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINES);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, 5.0f, 0.0f);
			glEnd();

			glLineWidth(1);
			glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_LINES);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(5.0f, 0.0f, 0.0f);
			glEnd();
		MV->popMatrix();
	}
}
