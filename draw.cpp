
#include <string>
#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "functions.h"
#include <gl/GLU.h>

//draws a postfix-function string scaled by integer scale
void drawfunction(std::string fct, int scale, float red, float green, float blue)
{

	float x = -scale;
	float y = -scale;
	float resolution;
	glColor3f(red, green, blue);
	if (d3d(fct))
	{
		resolution = -x / 20;
		while (x < scale)
		{
			glBegin(GL_TRIANGLE_STRIP);
			y = -scale;
			glm::vec3 lA(x, y-resolution, to_value(fct, x, y - resolution, true));
			glm::vec3 lB(x + resolution, y - resolution, to_value(fct, x + resolution, y - resolution, true));

			while (y < scale) {

				glm::vec3 a(x, y, to_value(fct, x, y, true));
				glm::vec3 b(x + resolution, y, to_value(fct, x + resolution, y, true));

				glm::vec3 na = glm::normalize(glm::cross(a - lA, lB - lA));
				glm::vec3 nb = glm::normalize(glm::cross(a - lB, b - lB));

				glNormal3fv(&(na[0]));
				glVertex3fv(&(a[0]));

				glNormal3fv(&(nb[0]));
				glVertex3fv(&(b[0]));

				lA = a;
				lB = b;
				//glVertex3f(x, y, to_value(fct, x, y, true));
				//glVertex3f(x + resolution, y, to_value(fct, x + resolution, y, true));
				y = y + resolution;


			}
			x = x + resolution;
			glEnd();
		}
	}
	else {
		glBegin(GL_LINE_STRIP);
		resolution = -x / 50;
		while (x < scale)
		{

			glVertex3f(x, 0, to_value(fct, x, y, false));
			x = x + resolution;

		}
		glEnd();
	}
}

//draws a coordinate systen from - to + scale
void drawcoordinates(int scale, bool makeit3d)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	//
	//ACHSEN
	//
	glColor3f(1, 1, 0);
	glVertex3f(-scale, 0, 0); glVertex3f(scale, 0, 0);
	if (makeit3d) {
		glColor3f(1, 0, 1);
		glVertex3f(0, -scale, 0); glVertex3f(0, scale, 0);
	}
	glColor3f(0, 1, 1);
	glVertex3f(0, 0, -scale); glVertex3f(0, 0, scale);
	glEnd();
	//
	//evtl. Pfeile?
	//
	/*glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 0);
	glVertex3f(scale, 0, 0); glVertex3f(9*scale/10, 0, scale/20); glVertex3f(9*scale/10, 0, -scale/20);
	/*glColor3f(1, 0, 1);
	glVertex3f(0, scale, 0); glVertex3f(0,19 * scale / 20, scale / 40); glVertex3f(0, 19 * scale / 20, scale / 40);
	glColor3f(0, 1, 1);
	glVertex3f(0,0,scale); glVertex3f(0,scale/40,19 * scale / 20); glVertex3f(0, scale / 40,19 * scale / 20);
	glEnd();*/
}