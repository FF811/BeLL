
#include <string>
#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "functions.h"
#include <gl/GLU.h>

void drawfunction(std::string fctsh, int scale)
{

	float x = -2;
	float y = -2;
	if (d3d(fctsh))

	{
		while (x < 2)
		{
			glBegin(GL_TRIANGLE_STRIP);
			y = -2;
			while (y < 2) {
				glVertex3f(x, y, to_value(fctsh, x, y, true));
				glVertex3f(x + 0.4, y, to_value(fctsh, x + 0.4, y, true));
				y = y + 0.4;
			}
			x = x + 0.4;
			glEnd();
		}
	}
	else {
		glBegin(GL_LINE_STRIP);
		while (x < 2)
		{

			glVertex3f(x, to_value(fctsh, x, y, false), 0);
			x = x + 0.05;

		}
		glEnd();
	}
}

void drawcoordinates(int scale)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glLineWidth(2.0);
glBegin(GL_LINES);

glColor3f(0, 1, 0);
glVertex3f(-10, 0, 0); glVertex3f(10, 0, 0);
glColor3f(1, 0, 0);
glVertex3f(0, -10, 0); glVertex3f(0, 10, 0);
glColor3f(0, 0, 1);
glVertex3f(0, 0, -10); glVertex3f(0, 0, 10);
glEnd(); 
}