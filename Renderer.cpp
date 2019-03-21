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
#include "Renderer.h"

// GLU gives us the ability to draw simple geometric shapes
#include <gl/GLU.h>

void draw_texture(GLuint tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glColor3f(1 , 1, 1);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex4f(-1.0f, -1.0f, 0.0f, 1);

	glTexCoord2f(1, 0);
	glVertex4f(1.0f, -1.0f, 0.0f, 1);

	glTexCoord2f(1, 1);
	glVertex4f(1.0f, 1.0f, 0.0f, 1);

	glTexCoord2f(0, 1);
	glVertex4f(-1.0f, 1.0f, 0.0f, 1);

	glEnd();

}

void ScreenR::regen_fbo(int w, int h)
{
	w *= 2;
	h *= 2;
	if (!fbo) // ...... initialisierung von fbo und tex und drb
	{
		glGenFramebuffers(1, &fbo);
		glGenTextures(2, tex);
		glGenRenderbuffers(1, &drb);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Allocate memory for texture
	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// Poor filtering. Needed !
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	// Allocate memory for renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, drb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex[0], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, drb);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	//Check correctness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("foooo!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

};

void ViveR::regen_fbo(int w, int h)
{
	w = ovr.getWidth();
	h = ovr.getHeight();
	if (!fbo) // ...... initialisierung von fbo und tex und drb
	{
		glGenFramebuffers(1, &fbo);
		glGenTextures(2, tex);
		glGenRenderbuffers(1, &drb);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Allocate memory for texture
	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// Poor filtering. Needed !
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	// Allocate memory for renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, drb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex[0], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, drb);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

								   //Check correctness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("foooo!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

};

void Render::render_texture(int screenmode)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, tex[0]);

	glColor3f(1, 1, 1);
	

	//pattern for drawing only every second line (1,3,5,...)
	GLubyte pattern1[32][4] = { { 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 }
	};

	//pattern for drawing only every second line (2,4,6,...)
	GLubyte pattern2[32][4] = { { 0,0,0,0 }, { 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 },{ 0,0,0,0 },
	{ 255,255,255,255 }
	};

	if (screenmode == 0)
	{
		draw_texture(tex[0]);
	}
	if (screenmode == 1)
	{
		glColorMask(true, false, false, false);
		draw_texture(tex[0]);
		glColorMask(false, true, true, false);
		draw_texture(tex[1]);
		glColorMask(true, true, true, true);
	}
	if (screenmode == 2)

	{
		glEnable(GL_POLYGON_STIPPLE);
		glPolygonStipple((GLubyte*)pattern1);
		draw_texture(tex[0]);
		glPolygonStipple((GLubyte*)pattern2);
		draw_texture(tex[1]);
		glDisable(GL_POLYGON_STIPPLE);
	}
	if (screenmode == 3)
	{
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		glBegin(GL_QUADS);

		glTexCoord2f(0, 0);
		glVertex4f(-1.0f, -0.5f, 0.0f, 1);

		glTexCoord2f(1, 0);
		glVertex4f(0.0f, -0.5f, 0.0f, 1);

		glTexCoord2f(1, 1);
		glVertex4f(0.0f, 0.5f, 0.0f, 1);

		glTexCoord2f(0, 1);
		glVertex4f(-1.0f, 0.5f, 0.0f, 1);

		glEnd();
		glBindTexture(GL_TEXTURE_2D, tex[1]);

		glBegin(GL_QUADS);

		glTexCoord2f(0, 0);
		glVertex4f(0.0f, -0.5f, 0.0f, 1);

		glTexCoord2f(1, 0);
		glVertex4f(1.0f, -0.5f, 0.0f, 1);

		glTexCoord2f(1, 1);
		glVertex4f(1.0f, 0.5f, 0.0f, 1);

		glTexCoord2f(0, 1);
		glVertex4f(0.0f, 0.5f, 0.0f, 1);

		glEnd();
	}

		//disable the polarisation
		
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glBindTexture(GL_TEXTURE_2D, 0);

	
}

void ScreenR::set_matrices(float angle, float aspect, float nearplane, float farplane, float eyedistance, int distance)
{
	float x = eyedistance / 2.0f;
	//int paradis = distance;
	int paradis = 1;
	float o = (eyedistance / 2 * 0.1) / paradis;
	float top = tan(glm::radians(angle) / 2)*0.1f,
	bottom = -top,
	left = (aspect * bottom) + o,
	right = -(aspect * bottom) + o;
	pmat[0] = glm::frustum(left, right, bottom, top, nearplane, farplane);
	mvmat[0] = glm::lookAt(glm::vec3(-x, -distance, 0), glm::vec3(-x, 0, 0), glm::vec3(0, 0, 1));
	pmat[1] = glm::frustum( -right,-left, bottom, top, nearplane,farplane);
	mvmat[1] = glm::lookAt(glm::vec3(+x, -distance, 0), glm::vec3(+x, 0, 0), glm::vec3(0, 0, 1));
}
