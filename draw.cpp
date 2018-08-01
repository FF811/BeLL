
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
	float resolution = 0.4;
	if (d3d(fctsh))

	{
		while (x < scale)
		{
			glBegin(GL_TRIANGLE_STRIP);
			y = -scale;
			while (y < scale) {
				
			
				
					glVertex3f(x, y, to_value(fctsh, x, y, true));
					glVertex3f(x + resolution,y, to_value(fctsh, x + resolution, y, true));
					y = y + resolution;
				
				
			}
			x = x + resolution;
			glEnd();
		}
	}
	else {
		glBegin(GL_LINE_STRIP);
		while (x < scale)
		{

			glVertex3f(x, to_value(fctsh, x, y, false), 0);
			x = x + resolution;

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