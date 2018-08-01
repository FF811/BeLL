
#include <string>
#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "functions.h"
#include <gl/GLU.h>

void drawfunction(std::string fctsh, int scale)
{

	float x = -scale;
	float y = -scale;
	float value;
	if (d3d(fctsh))

	{
		while (x < scale)
		{
			glBegin(GL_TRIANGLE_STRIP);
			y = -scale;
			while (y < scale) {
				value = to_value(fctsh, x, y, true);
				if (value<scale && value>-scale)
				{
					glVertex3f(x, y, value);
					glVertex3f(x + scale / 20 ,y, to_value(fctsh, x + scale / 20, y, true));
					y = y + scale/20;
				}
				
			}
			x = x + scale / 20;
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
glVertex3f(-scale, 0, 0); glVertex3f(scale, 0, 0);
glColor3f(1, 0, 0);
glVertex3f(0, -scale, 0); glVertex3f(0, scale, 0);
glColor3f(0, 0, 1);
glVertex3f(0, 0, -scale); glVertex3f(0, 0, scale);
glEnd(); 
}