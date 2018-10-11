#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <fstream>
#include "openvr_integration.h"
#include <openvr.h>
// GLM provides matries and vectors
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "functions.h"
#include "draw.h"
#include <Eigen/Core>

// GLU gives us the ability to draw simple geometric shapes
#include <gl/GLU.h>

class Render
{
public:
	Render::Render() {
		tex[0] = 0;
		tex[1] = 0;
		fbo = 0;
		drb = 0;
	}	
	glm::mat4 pmat[2];
	glm::mat4 mvmat[2];
	GLuint tex[2];
	GLuint fbo;
	GLuint drb;
	
	void render_texture(int screenmode);
};

class ViveR : public Render
{
public:
	COpenVR ovr;
	void regen_fbo(int w, int h);
};

class ScreenR :public Render
{
public:
	void regen_fbo(int w, int h);
	void set_matrices(float angle, float aspect, float nearplane, float farplane, float eyedistance, int distance, bool makeit3d);
};