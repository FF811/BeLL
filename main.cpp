#if 1

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
using namespace glm;
#include "Renderer.h"

// GLU gives us the ability to draw simple geometric shapes
#include <gl/GLU.h>

 /****************************************************************************
 * WINDOW-RELATED CALLBACKS                                                 *
 ****************************************************************************/
ScreenR scr;
ViveR vive;
//nanogui stuff
nanogui::Screen *screen = nullptr;
nanogui::Window *w = nullptr;
nanogui::Window *w2 =nullptr;
nanogui::CheckBox *cb1 = nullptr;
nanogui::CheckBox *cb2 = nullptr;
nanogui::Window *w3 =  nullptr;
nanogui::TextBox *t = nullptr;
nanogui::Widget *fun[10] = { nullptr };
nanogui::Window *winwin =  nullptr ;
nanogui::Slider *zoomslide = nullptr;
nanogui::Button *bf[10] = { nullptr };
nanogui::CheckBox *bfsupp[10] = { nullptr };
nanogui::Button *b = nullptr;
nanogui::CheckBox *vrcb = nullptr;
nanogui::ComboBox *screening = nullptr;
//format of the window
int width, height;
//counts number of functions
int functionnumber = 0;
//wired or 3d
bool wired,makeit3d = true;
//to move or not to move that's the question
bool moveit = false;
//for calculating the movement
float counterz, diffz, counterxy, diffxy;
//rotating angle
float sumz=0,sumxy = 0;
//camera distance to (0|0|0)
float distanz= 11;
//eye distance
float eyedistance = 0.065;
//array for saving all functions
std::string fct[10] = { "0 "};
//array for saving the function's colors
float red[10] = { 1.0,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6, }, green[10] = { 1.0,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6, }, blue[10] = { 1.0,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6, };
//parallax distance
float paradis ;
int screenmode=0;
bool makeitvr = false;
//projection matrices
glm::mat4 p[2];
//rtt stuff



GLuint resolution[2] = { 0,0 };

//Frustum-function
static void frustum()
{
	float top = tan(glm::radians(60.0f) / 2)*0.1f,
		bottom = -top,
		left = width / height * bottom,
		right = -left;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	p[0] = glm::frustum(left, right, bottom, top, 0.1f, 100.0f);
	glLoadMatrixf(glm::value_ptr(p[0]));
}
	
	/* This function is registered as the framebuffer size callback for GLFW,
* so GLFW will call this during glfwPollEvents() whenever the window is resized. */
static void callback_Resize(GLFWwindow *win, int wi, int h)
{
	scr.regen_fbo(wi, h);
	vive.regen_fbo(wi, h);
	screen->resizeCallbackEvent(wi, h);
	void* user_pointer = glfwGetWindowUserPointer(win);
	glViewport(0, 0, 2*wi, 2*h);
	resolution[0] = 2 * wi;
	resolution[1] = 2 * h;
	printf("new framebuffer size izz da: %dx%d pixels\n", wi, h);
	
	height = h;
	width = wi;
	w3->setPosition(Eigen::Vector2i(220, height - 70));
	w->setPosition(Eigen::Vector2i(10, 10));
	w2->setPosition(Eigen::Vector2i(15, 88));
	//frustum();
}




/* This function is registered as the keyboard callback for GLFW, so GLFW
* will call this during glfwPollEvents() whenever a key is pressed. */
static void callback_Keyboard(GLFWwindow *win, int key, int scancode, int action, int mods)
{
	void* user_pointer = glfwGetWindowUserPointer(win);


	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
	{
		glfwSetWindowShouldClose(win, true);
	}

	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		wired=true;
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		wired=false;
	}
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
	{
		if (distanz > 1) { distanz--; }
		zoomslide->setValue((distanz - 1) / 20);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
	{
		if (distanz < 21) { distanz++; }
		zoomslide->setValue((distanz -1)/20);
	}
	screen->keyCallbackEvent(key, scancode, action, mods);


}

static void callback_mousewheel(GLFWwindow *win, double xoffset, double yoffset)
{
	distanz = distanz - yoffset;
	if (distanz > 21) { distanz = 21; }
	if (distanz < 1) { distanz = 1; }
	zoomslide->setValue((distanz - 1) / 20);
}

/* This function is registered as the mouse button callback for GLFW, so GLFW
* will call this during glfwPollEvents() whenever a button is pressed. */
static void callback_MouseButton(GLFWwindow *win, int button, int action, int mods)
{
	void* user_pointer = glfwGetWindowUserPointer(win);

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		double xpos, ypos;
		glfwGetCursorPos(win, &xpos, &ypos);
		if (xpos > 210 && ypos < height - 70)
		{
			moveit = true;
			diffxy = 0;
			diffz = 0;
			std::cout << "Drehung um um x: " << sumxy << "; um z: " << sumz << std::endl;
		}
		if (action == GLFW_RELEASE)
		{
			moveit = false;
		}
	}
	screen->mouseButtonCallbackEvent(button, action, mods);
}


/* This function is registered as the mouse move  callback for GLFW, so GLFW
* will call this during glfwPollEvents() whenever the cursor is moved. */
static void callback_CursorMove(GLFWwindow *win, double x, double y)
{
	void* user_pointer = glfwGetWindowUserPointer(win);
	screen->cursorPosCallbackEvent(x, y);
		diffxy = y - counterxy;
		diffz = x - counterz;
	counterxy = y;
	counterz = x;
	if (moveit)
	{
		sumz = sumz + diffz;
		if (sumz >= 360) { sumz = sumz - 360; };
		if (sumz <= -360) { sumz = sumz + 360; };
		sumxy = sumxy + diffxy;
		if (sumxy >= 360) { sumxy = sumxy - 360; };
		if (sumxy <= -360) { sumxy = sumxy + 360; };
	}

	w->setPosition(Eigen::Vector2i(10, 10));
	w3->setPosition(Eigen::Vector2i(220, height - 70));
	w2->setPosition(Eigen::Vector2i(15, 88));
}

// This function will create an opengl window with a given width and height.
// The user pointer will be stored in said window and can be accessed using glfwGetWindowUserPointer.
GLFWwindow* open_window(int w, int h, void* user_pointer = nullptr)
{

	GLFWwindow* win = nullptr;

	// initialize GLFW library 
	printf("initializing GLFW\n");
	if (!glfwInit()) {
		printf("Failed to initialze GLFW\n");
		return nullptr;
	}

	/* request a OpenGL 4.3 compatibility profile context */
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	/* create the window and the gl context */
	printf("creating window and OpenGL context\n");
	win = glfwCreateWindow(w, h, "BeLL", NULL, NULL);

	if (!win) {
		printf("failed to get window with OpenGL 4.3 core context\n");
		return false;
	}

	/* store a pointer to our application context in GLFW's window data.
	 * This allows us to access our data from within the callbacks */
	glfwSetWindowUserPointer(win, user_pointer);

	/* register our callbacks */
	glfwSetFramebufferSizeCallback(win, callback_Resize);
	glfwSetKeyCallback(win, callback_Keyboard);
	glfwSetMouseButtonCallback(win, callback_MouseButton);
	glfwSetCursorPosCallback(win, callback_CursorMove);
	glfwSetCharCallback(win,
		[](GLFWwindow *, unsigned int codepoint) {
		screen->charCallbackEvent(codepoint);
	}

	);
	glfwSetScrollCallback(win, callback_mousewheel);


	/* make the context the current context (of the current thread) */
	glfwMakeContextCurrent(win);

	/* initialize glad (our opengl loader) */
	printf("initializing glad\n");
	if (!gladLoadGL()) {
		printf("failed to intialize glad\n");
		return false;
	}

	return win;
}

// A global quadric used to draw the example sphere
GLUquadric* quadric;

void draw_plot()
{
	if (wired) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	//calculate rotation angle
	glLineWidth(4.0);
	if (makeit3d)
	{
		glRotatef(sumz, 0, 0, 1);
		glRotatef(sumxy, cos(sumz*pi / 180), -sin(sumz*pi / 180), 0);
	}

	//draw functions
	for (int i = 0; i < functionnumber; i++)
	{
		if (bfsupp[i]->checked())
		{
			if (d3d(fct[i]) == makeit3d)	drawfunction(fct[i], distanz, red[i], green[i], blue[i]);
		}
	};

	//draw coordinate system
	drawcoordinates(distanz);

}

// main display function. this will be called twice per frame.
bool display_funktion()
{

	if (!makeitvr)
	{
		//super sampling 
		glViewport(0, 0, resolution[0], resolution[1]);
		//set up frustum
		scr.set_matrices(60.0f, width / height, 0.1f, 100.0f, eyedistance, distanz, makeit3d);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(scr.pmat[0]));
		//lookat for 2D/3D
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(scr.mvmat[0]));

		//texture NULL
		glBindFramebuffer(GL_FRAMEBUFFER, scr.fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, scr.tex[0], 0);
		glClearColor(0.0, 0.0, 0.0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_plot();

		//set up frustum
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(scr.pmat[1]));

		//look at for 3D
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(scr.mvmat[1]));

		//texture EINS
		glBindFramebuffer(GL_FRAMEBUFFER, scr.fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, scr.tex[1], 0);
		glClearColor(0.0, 0.0, 0.0, 1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glPushMatrix();
		draw_plot();

		//clear nearly all
		glPopMatrix();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0, 0, 0, 1);

		//glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		//super sampling
		glViewport(0, 0, resolution[0] / 2, resolution[1] / 2);

		scr.render_texture(screenmode);
	}

	if (makeitvr)
	{
		sumz = 0;
		sumxy=0;
		//super sampling 
		glViewport(0, 0, vive.ovr.getWidth(), vive.ovr.getHeight());
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(vive.ovr.get_projection(0, 0.1f, 100.0f)));
		//lookat for 2D/3D
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(vive.ovr.get_view(0)));

		//texture NULL
		glBindFramebuffer(GL_FRAMEBUFFER, vive.fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, vive.tex[0], 0);
		glClearColor(0.0, 0.0, 0.0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//if (makeit3d) { /* Do something!  }


		glColor3f(1, 1, 1);

		draw_plot();

		//set up frustum
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(vive.ovr.get_projection(1, 0.1f, 100.0f)));

		//look at for 3D
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(vive.ovr.get_view(1)));

		//texture EINS
		glBindFramebuffer(GL_FRAMEBUFFER, vive.fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, vive.tex[1], 0);
		glClearColor(0.0, 0.0, 0.0, 1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glPushMatrix();
		draw_plot();

		//clear nearly all
		glPopMatrix();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0, 0, 0, 1);
		//glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		vive.ovr.composite(vive.tex[0], vive.tex[1]);
		//super sampling
		//glViewport(0, 0, resolution[0] / 2, resolution[1] / 2);
		vive.render_texture(screenmode);
	}
	return true;
}



/****************************************************************************
 * PROGRAM ENTRY POINT                                                      *
 ****************************************************************************/


int main(int argc, char **argv)
{
	width = 1080; height = 720;
	GLFWwindow *win = open_window(width, height);
	// Create a nanogui screen and pass the glfw pointer to initialize 
	screen = new nanogui::Screen();
	screen->initialize(win, false);

	// Create nanogui gui
	using namespace nanogui;
	bool enabled = true;
	

	/*************
	Fenster links
	*************/
#if 1
	w = new Window(screen, "");
	w->setFixedSize(Vector2i(200, 700));
	w->setPosition(Vector2i(10, 10));
	w->setLayout(new GroupLayout());

	

	/**************
	Label für 2D/3D
	***************/
	{
		new Label(w, "", "sans-bold");
		Widget *Dim = new Widget(w);
		Dim->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 4));

		Button *dimension = new Button(Dim, "3D");
		dimension->setFixedSize(Vector2i(20,20));
		dimension->setFontSize(18);
		dimension->setTooltip("Dimension festlegen (2D/3D)");
		dimension->setBackgroundColor(Color(142, 69, 15, 255));
		dimension->setCallback([&dimension] {if (dimension->caption() == "2D") { dimension->setCaption("3D"); makeit3d = true; }
		else { dimension->setCaption("2D"); makeit3d = false; } });

		//Combobox for screening

		screening = new ComboBox(Dim, {"Normal", "Anaglyph","3D-Bildschirm (Stereolines)", "Splitted" });
		screening->setTooltip("Abbildungsmethode wählen!");
		screening->setFontSize(15);
		screening->setFixedSize(Vector2i(150, 20));
		/*screening->setCallback( [] {if (screening->caption() == "Normal") { screenmode = 0; }
		if (screening->caption() == "Anaglyph") { screenmode = 1; }
		if (screening->caption() == "3D-Bildschirm (Stereolines)") { screenmode = 2; }
		});*/
	}

	/****************
	Label für EINGABE
	****************/

	{
		new Label(w, "", "sans-bold");
		Widget *Eingabe = new Widget(w);
		Eingabe->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0, 10));

		//add Textfeld
		TextBox *t = new TextBox(Eingabe);
		t->setEditable(true);
		t->setFixedSize(Vector2i(110, 20));
		t->setValue("x^3+y^3");
		t->setFontSize(20);
		t->setTooltip("Funktion eingeben");
		

		//Button neben Textfeld
		Button *a = new Button(Eingabe, "~");
		a->setFixedSize(Vector2i(20, 20));
		a->setFontSize(28);
		a->setTooltip("Vorlagen");
		

		//add Button
		b = new Button(Eingabe, "+");
		b->setFixedSize(Vector2i(20, 20));
		b->setFontSize(28);
		b->setTooltip("Funktion hinzufuegen");
		b->setCallback([&t] {
			
		
			
		fct[functionnumber] = t->value(); fct[functionnumber] = to_postfix(fct[functionnumber]);
		bf[functionnumber]->setCaption(t->value());
		bf[functionnumber]->setEnabled(true);
		bfsupp[functionnumber]->setEnabled(true);
		bfsupp[functionnumber]->setChecked(true);
		
		if (functionnumber < 9) {
			functionnumber++;
		}
		else functionnumber = 9;
		
		

		});

	}


	/***************************
	Fenster für Funktionsübersicht
	***************************/
	{
	
		w2 = new Window(w, "Funktionen");
		w2->setTooltip("Bisher hinzugefuegte Funktionen");
		w2->setFixedSize(Vector2i(180, 300));
		w2->setLayout(new GroupLayout());	
		
	}
	

	
	//Button for fnct 


	for (int i = 0; i < 10; i++)
	{
		Widget *Eingabe = new Widget(w2);
		Eingabe->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0, 10));
		bf[i] = new Button(Eingabe, "Funktion?");
		bf[i]->setFixedSize(Vector2i(120, 20));
		bf[i]->setVisible(true);
		bf[i]->setEnabled(false);
		bf[i]->setFontSize(15);
		bf[i]->setCallback([&] 
		{
			winwin = new Window(screen,"Funktion" );
			winwin->setFixedSize(Vector2i(125, 200));
			winwin->setPosition(Vector2i(220, 150));
			winwin->setLayout(new GroupLayout());
			winwin->setVisible(true);
			new Label(winwin, "" , "sans-bold");
			Widget *winner = new Widget(winwin);
			winner->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Maximum, 3, 1));

			Button *h1 = new Button(winner,"Maximum");
			h1->setFixedSize(Vector2i(88,20));
			h1->setCallback([] 
			{
				winwin->dispose();
			}
			);

			Button *h2 = new Button(winner, "Minimum");
			h2->setFixedSize(Vector2i(88, 20));
			h2->setCallback([]
			{
				winwin->dispose();
			}
			);

			Button *h3 = new Button(winner, "Farbe");
			h3->setFixedSize(Vector2i(88, 20));
			h3->setCallback([]
			{
				winwin->dispose();
			}
			);

			Button *h4 = new Button(winner, "y-Wert");
			h4->setFixedSize(Vector2i(88, 20));
			h4->setCallback([]
			{
				winwin->dispose();
				Window *valuewin = new Window(screen, "x Berechnung");
				valuewin->setFixedSize(Vector2i(130, 200));
				valuewin->setPosition(Vector2i(220, 150));
				valuewin->setLayout(new GroupLayout());
				valuewin->setVisible(true);
				valuewin->setFocused(true);
				new Label(valuewin, "", "sans-bold");
				Widget *valuewidget = new Widget(valuewin);
				valuewidget->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Maximum, 2, 6));

				TextBox *t1 = new TextBox(valuewidget, "0");
				t1->setEditable(true);
				t1->setFixedSize(Vector2i(88, 20));
				t1->setFontSize(16);
				t1->setTooltip("y-Wert eingeben");

				if (makeit3d)
				{
					TextBox *t2 = new TextBox(valuewidget, "0");
					t2->setEditable(true);
					t2->setFixedSize(Vector2i(88, 20));
					t2->setFontSize(16);
					t2->setTooltip("z-Wert eingeben");
				}

				Button *calc1 = new Button(valuewidget, "Berechnen");
				calc1->setFixedSize(Vector2i(88, 20));
				calc1->setFontSize(16);
				calc1->setTooltip("Berechnung des y-Wertes");

				TextBox *t3 = new TextBox(valuewidget, "Ergebnis");
				t3->setEditable(false);
				t3->setFixedSize(Vector2i(88, 29));
				t3->setFontSize(21);
				t3->setTooltip("Hier könnte Ihr Ergebnis stehen!");

				screen->performLayout();
			}
			);

			Button *h5 = new Button(winner, "x-Wert");
			h5->setFixedSize(Vector2i(88, 20));
			h5->setCallback([]
			{
				winwin->dispose();
				Window *valuewin = new Window(screen, "x Berechnung");
				valuewin->setFixedSize(Vector2i(130, 200));
				valuewin->setPosition(Vector2i(220, 150));
				valuewin->setLayout(new GroupLayout());
				valuewin->setVisible(true);
				new Label(valuewin, "", "sans-bold");
				Widget *valuewidget = new Widget(valuewin);
				valuewidget->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Maximum, 2, 6));

				TextBox *t1 = new TextBox(valuewidget, "0");
				t1->setEditable(true);
				t1->setFixedSize(Vector2i(88, 20));
				t1->setFontSize(16);
				t1->setTooltip("y-Wert eingeben");

				if (makeit3d)
				{
					TextBox *t2 = new TextBox(valuewidget, "0");
					t2->setEditable(true);
					t2->setFixedSize(Vector2i(88, 20));
					t2->setFontSize(16);
					t2->setTooltip("z-Wert eingeben");
				}

				Button *calc1 = new Button(valuewidget, "Berechnen");
				calc1->setFixedSize(Vector2i(88, 20));
				calc1->setFontSize(16);
				calc1->setTooltip("Berechnung des y-Wertes");

				TextBox *t3 = new TextBox(valuewidget, "Ergebnis");
				t3->setEditable(false);
				t3->setFixedSize(Vector2i(88, 29));
				t3->setFontSize(21);
				t3->setTooltip("Hier könnte Ihr Ergebnis stehen!");

				screen->performLayout();
			}
			);

			if (makeit3d) 
			{
				Button *h6 = new Button(winner, "z-Wert");
				h6->setFixedSize(Vector2i(88, 20));
				h6->setCallback([]
				{
					winwin->dispose();
					Window *valuewin = new Window(screen, "z Berechnung");
					valuewin->setFixedSize(Vector2i(130, 200));
					valuewin->setPosition(Vector2i(220, 150));
					valuewin->setLayout(new GroupLayout());
					valuewin->setVisible(true);
					valuewin->setFocused(true);
					new Label(valuewin, "", "sans-bold");
					Widget *valuewidget = new Widget(valuewin);
					valuewidget->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Maximum, 2, 6));

					TextBox *t1 = new TextBox(valuewidget, "0");
					t1->setEditable(true);
					t1->setFixedSize(Vector2i(88, 20));
					t1->setFontSize(16);
					t1->setTooltip("x-Wert eingeben");

					if (makeit3d)
					{
						TextBox *t2 = new TextBox(valuewidget, "0");
						t2->setEditable(true);
						t2->setFixedSize(Vector2i(88, 20));
						t2->setFontSize(16);
						t2->setTooltip("y-Wert eingeben");
					}

					Button *calc1 = new Button(valuewidget, "Berechnen");
					calc1->setFixedSize(Vector2i(88, 20));
					calc1->setFontSize(16);
					calc1->setTooltip("Berechnung des y-Wertes");

					TextBox *t3 = new TextBox(valuewidget, "Ergebnis");
					t3->setEditable(false);
					t3->setFixedSize(Vector2i(88, 29));
					t3->setFontSize(21);
					t3->setTooltip("Hier könnte Ihr Ergebnis stehen!");

					screen->performLayout();
				}
				);
			}


			screen->performLayout();
			});
		bfsupp[i] = new CheckBox(Eingabe, "~");
		bfsupp[i]->setFixedSize(Vector2i(20, 20));
		bfsupp[i]->setVisible(true);
		bfsupp[i]->setEnabled(false);
		bfsupp[i]->setFontSize(15);
	}	

		
	/*************************************
	Label für Funktionsbearbeitungsbuttons
	*************************************/
	{
		//Button für alles markieren
		Button *mark = new Button(w, "Alles markieren");
		mark->setFixedSize(Vector2i(180, 20));
		mark->setFontSize(18);
		mark->setTooltip("Alle bisher hinzugefuegten Funktionen markieren");
		mark->setCallback([] {for (int i = 0; i < 10; i++)
			if (bfsupp[i]->enabled()) {
				bfsupp[i]->setChecked(true);
			}
		});


		//Button für löschen
		Button *destroy = new Button(w, "Markierte loeschen");
		destroy->setFixedSize(Vector2i(180, 20));
		destroy->setTextColor(Color(255, 0, 0, 200));
		destroy->setFontSize(18);
		destroy->setTooltip("Loescht alle markierten Funktionen unwiederruflich");
		destroy->setCallback([] {int x=0;
			for (int i = 0; i < 10; i++)
				if (bfsupp[i]->checked()) {
					bfsupp[i]->setChecked(false);
					bfsupp[i]->setEnabled(false);
					bf[i]->setEnabled(false);
					bf[i]->setCaption("Funktion?");
					fct[i] = "0";
					if (functionnumber > 0) functionnumber--;
				}
			for (int i = 0; i < 10; i++)
			if (!bfsupp[i]->enabled()) 
				x++;
			else if(x>0) {
			bf[i-x]->setEnabled(true);
			bf[i-x]->setCaption(bf[i]->caption());
			bfsupp[i]->setEnabled(false);
			bfsupp[i-x]->setEnabled(true); 
			bfsupp[i - x]->setChecked(true);
			fct[i - x] = fct[i];
			fct[i] = "0";
			bf[i]->setCaption("Funktion?");
			bf[i]->setEnabled(false);
			}
		});


		using namespace std;
		//Button für speichern
		Button *save = new Button(w, "Markierte speichern");
		save->setFixedSize(Vector2i(180, 20));
		save->setTextColor(Color(0, 255, 0, 200));
		save->setFontSize(18);
		save->setTooltip("Speichert alle markierten Funktionen");
		save->setCallback([] {fstream f;
		f.open("saved.dat", ios::out);
		f << functionnumber << endl;
		for (int i = 0; i < 10; i++)
		if (bf[i]->caption() != "Funktion?")
		f << bf[i]->caption() << endl;
		f.close();	
		});
		//Button für Laden
		using namespace std;
		Button *load = new Button(w, "Gespeicherte laden");
		load->setFixedSize(Vector2i(180, 20));
		load->setFontSize(18);
		load->setTextColor(Color(14, 14, 130, 250));
		load->setTooltip("Oeffnet schon gespeicherte Funktionen");
		load->setCallback([] {ifstream f; 
		int r=-1;
		string s;
		f.open("saved.dat", ios::in); 
		std::getline(f, s);
		functionnumber = stoi(s);
		while (!f.eof())
		{
			r++;
			std::getline(f, s);
			if (s != "")
			{
				bf[r]->setCaption(s);
				fct[r] = s;
				bf[r]->setEnabled(true);
				bfsupp[r]->setEnabled(true);

			}
			
		}
		f.close();
		});

		/******************
		BearbeitungsLabel
		*******************/
		new Label(w, "", "sans-bold");
		Widget *Settings1 = new Widget(w);
		Settings1->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 4));

		Button *switcheyes = new Button(Settings1, "Switch Eyes");
		switcheyes->setFixedSize(Vector2i(80, 20));
		switcheyes->setFontSize(18);
		switcheyes->setTooltip("Augen tauschen!");
		switcheyes->setBackgroundColor(Color(60, 120, 12, 255));
		switcheyes->setCallback([&switcheyes] {eyedistance = -eyedistance; std::cout << "changed eyes" << std::endl; });

		new Label(w, "", "sans-bold");
		Widget *Settings2 = new Widget(w);
		Settings2->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 4));

		new Label (Settings2, "Augenabstand:","sans-bold");

		TextBox *eyedistanz = new TextBox(Settings2);
		eyedistanz->setEditable(true);
		eyedistanz->setFixedSize(Vector2i(40, 20));
		eyedistanz->setValue("0.06");
		eyedistanz->setFontSize(16);
		eyedistanz->setTooltip("Abstand der Augen in Metern");

		Button *distancebutton = new Button(Settings2, "#");
		distancebutton->setFixedSize(Vector2i(20, 20));
		distancebutton->setFontSize(18);
		distancebutton->setTooltip("Augenabstand bestätigen");
		distancebutton->setBackgroundColor(Color(60, 60, 60, 255));
		distancebutton->setCallback([&distancebutton, &eyedistanz] {eyedistance = stof(eyedistanz->value()); std::cout << "set eyedistance to " << eyedistanz->value() << std::endl; });
	
		new Label(w, "", "sans-bold");
		Widget *Settings3 = new Widget(w);
		Settings3->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 4));

		new Label(Settings3, "VR einschalten:", "sans-bold");

		vrcb = new CheckBox(Settings3,"Nürnberg");
		vrcb->setFixedSize(Vector2i(20, 20));
		vrcb->setVisible(true);
		vrcb->setEnabled(true);
		vrcb->setFontSize(15);
	}

#endif 
	/*************
	Fenster unten
	*************/
#if 1
	w3 = new nanogui::Window(screen, "");
	w3->setFixedSize(Vector2i(850, 60));
	w3->setPosition(Vector2i(220, height-70));
	w3->setLayout(new GroupLayout());

	//Label unten
	new Label(w3, "", "sans-bold");
	Widget *chgr = new Widget(w3);
	chgr->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0, 69));

	
		ToolButton *screenshot = new ToolButton (chgr,ENTYPO_ICON_CAMERA);
		screenshot->setFontSize(20);
		screenshot->setSize(Vector2i(50, 50));
		screenshot->setTextColor(Color(12, 100, 130, 255));
		screenshot->setCallback([] 
			{
			
			// get the device context of the screen
			HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);

			// and a device context to put it in
			HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

			int width = GetDeviceCaps(hScreenDC, HORZRES);
			int height = GetDeviceCaps(hScreenDC, VERTRES);

			// maybe worth checking these are positive values
			HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

			// get a new bitmap
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

			BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
			hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

			//WriteDIB(L"test.bmp", (HGLOBAL)hBitmap);

			// clean up
			DeleteDC(hMemoryDC);
			DeleteDC(hScreenDC);

			return hBitmap;

			});


		Button *cam_fnct = new Button(chgr, "Funktion bewegen");
		cam_fnct->setFixedSize(Vector2i(169,25));
		cam_fnct->setFontSize(20);
		cam_fnct->setCallback([&cam_fnct] {if (cam_fnct->caption() == "Funktion bewegen") cam_fnct->setCaption("Kamera bewegen"); 
									else cam_fnct->setCaption("Funktion bewegen"); });
	

		ToolButton *reset = new ToolButton(chgr, ENTYPO_ICON_EYE);
		reset->setSize(Vector2i(50, 50));
		reset->setTextColor(Color(142, 69, 188, 255));
	
			Button *zoomin = new Button(chgr, "+");
			zoomin->setFixedSize(Vector2i(20, 20));
			zoomin->setFontSize(18);
			zoomin->setTooltip("Reinzoomen");

			zoomin->setCallback([] {if (distanz > 1) { distanz--; }
			zoomslide->setValue((distanz - 1) / 20); });

			zoomslide = new Slider(chgr);
			zoomslide->setFixedWidth(169);
			zoomslide->setValue(0.5);
			zoomslide->setCallback([](float value){ distanz = (20 * value)+1; std::cout << value << std::endl; });
			
			Button *zoomout = new Button(chgr, "-");
			zoomout->setFixedSize(Vector2i(20, 20));
			zoomout->setFontSize(18);
			zoomout->setTooltip("Rauszoomen");
			zoomout->setCallback([] {if (distanz < 21) { distanz++; }
			zoomslide->setValue((distanz - 1) / 20); });

#endif
	//nanoGUI setVisible
	screen->setVisible(true);
	screen->performLayout();

	//enable light staff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//light values
	float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float position[] = { -2.0f, 2.0f, -2.0f, 1.0f };

	//set light
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glColor3f(1, 1, 1);

	//glClearColor(1, 0, 0, 1);
	quadric = gluNewQuadric();

	// set up example projection matrix

	frustum();

	// set up example model view matrix
	glMatrixMode(GL_MODELVIEW);
	glm::mat4 m = glm::lookAt(vec3(1,1,1), vec3(0, 0, 0), vec3(0, 0, 1));
	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(m));

	glPushMatrix();

	scr.regen_fbo(width,height);
	
	resolution[0] = width*2;
	resolution[1] = height*2;
	
	glEnable(GL_TEXTURE_2D);
	vive.ovr.enable();
	vive.regen_fbo(width, height);

	// the main loop. as long as the display funtion returns true and the window
	// should not be closed swap the buffers and poll events.*/
	while (display_funktion() && !glfwWindowShouldClose(win))
	{
	
		glfwPollEvents();
		// Draw nanogui 
		screen->drawContents();
		screen->drawWidgets();

		glfwSwapBuffers(win);
		vive.ovr.begin_cycle();
		makeitvr = vrcb->checked();
		if (screening->caption() == "Normal") { screenmode = 0; }
		if (screening->caption() == "Anaglyph") { screenmode = 1; }
		if (screening->caption() == "3D-Bildschirm (Stereolines)") { screenmode = 2; }
		if (screening->caption() == "Splitted") { screenmode = 3; }
	}
	
	// clean up!
	glfwDestroyWindow(win);
	glfwTerminate();
	gluDeleteQuadric(quadric);
	

	vive.ovr.disable();
	return 0;
}


#endif
