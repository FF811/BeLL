
#include <string>
#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "functions.h"
#include <gl/GLU.h>

//draws a postfix-function string scaled by integer scale
void drawfunction(std::string fct, int scale)
{

	float x = -scale;
	float y = -scale;
	float resolution;
	if (d3d(fct))

	{	
		resolution = -x / 20;
		while (x < scale)
		{
			glBegin(GL_TRIANGLE_STRIP);
			y = -scale;
			while (y < scale) {
				
			
				
					glVertex3f(x, y, to_value(fct, x, y, true));
					glVertex3f(x + resolution,y, to_value(fct, x + resolution, y, true));
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

			glVertex3f(x, to_value(fct, x, y, false), 0);
			x = x + resolution;

		}
		glEnd();
	}
}

//draws a coordinate systen from - to + scale
void drawcoordinates(int scale)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glLineWidth(2.0);
glBegin(GL_LINES);
//
//ACHSEN
//
glColor3f(1, 1, 0);
glVertex3f(-scale, 0, 0); glVertex3f(scale, 0, 0);
glColor3f(1, 0, 1);
glVertex3f(0, -scale, 0); glVertex3f(0, scale, 0);
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