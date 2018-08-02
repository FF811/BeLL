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


// GLM provides matries and vectors
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "functions.h"
#include "draw.h"
#include <Eigen/Core>
using namespace glm;

// GLU gives us the ability to draw simple geometric shapes
#include <gl/GLU.h>


 /****************************************************************************
 * WINDOW-RELATED CALLBACKS                                                 *
 ****************************************************************************/
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
int width, height;
nanogui::Button *bf[10] = { nullptr };
nanogui::CheckBox *bfsupp[10] = { nullptr };
nanogui::Button *b = nullptr;
nanogui::ColorPicker *cp = nullptr;
int functionnumber = 0;
bool wired,makeit3d = true;
bool moveit = false;
float x2, y2, xw, yw, counterz,diffz,counterxy,diffxy,sumz,sumxy = 0;
float distanz = 11;
std::string fctsh[10] = { "0 "};
float red[10], green[10], blue[10] = { 0 };
static bool bval = false;
static std::string strval = "oink!";

/* This function is registered as the framebuffer size callback for GLFW,
* so GLFW will call this during glfwPollEvents() whenever the window is resized. */
static void callback_Resize(GLFWwindow *win, int wi, int h)
{
	void* user_pointer = glfwGetWindowUserPointer(win);
	glViewport(0, 0, wi, h);
	printf("new framebuffer size izz da: %dx%d pixels\n", wi, h);
	screen->resizeCallbackEvent(wi, h);
	height = h;
	width = wi;
	w3->setPosition(Eigen::Vector2i(220, height - 70));
	w->setPosition(Eigen::Vector2i(10, 10));
	w2->setPosition(Eigen::Vector2i(15, 88));
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
	/*if (key == GLFW_KEY_2 && action == GLFW_RELEASE)
	{
		glClearColor(0, 1, 0, 1);
	}
	if (key == GLFW_KEY_3 && action == GLFW_RELEASE)
	{
		glClearColor(0, 0, 1, 1);
	}

	if (key == GLFW_KEY_4 && action == GLFW_RELEASE)
	{
		glClearColor(0.4, 0.4, 0.4, 1);
	}*/

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
	//std::cout << xoffset << "    " << yoffset << std::endl;
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
		}
		if (action == GLFW_RELEASE)
		{
			moveit = false;
		}
	}
	screen->mouseButtonCallbackEvent(button, action, mods);
	//std::cout << button << "    " << action << std::endl;
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
	//std::cout << diffz << "     " <<moveit<< std::endl;
	w->setPosition(Eigen::Vector2i(10, 10));
	w3->setPosition(Eigen::Vector2i(220, height - 70));
	w2->setPosition(Eigen::Vector2i(15, 88));
}

// This function will create an opengl window with a given width and height.
// The user pointer will be stored in said window and can be accessed using glfwGetWindowUserPointer.
GLFWwindow* open_window(int w, int h, void* user_pointer = nullptr)
{

	GLFWwindow* win = nullptr;

	/* initialize GLFW library */
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
float alpha = 0;
float beta = 0;




// main display function. this will be called once per frame.
bool display_funktion()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glColor3f(0, 0, 0);

	if (wired) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
	glPushMatrix();
	glLineWidth(1.0);
	if (makeit3d)
	{if (moveit) {
		//diffz = diffz * 0.36;
		sumz = sumz + diffz;
		if (sumz >= 360) { sumz = sumz - 360; };
		if (sumz <= -360) { sumz = sumz + 360; };
		//diffxy = diffxy * 0.36;
		sumxy = sumxy + diffxy;
		if (sumxy >= 360) { sumxy = sumxy - 360; };
		if (sumxy <= -360) { sumxy = sumxy + 360; };
		
		
	}
	
	glRotatef(sumz, 0, 0, 1);
	glRotatef(sumxy, -sin((45-sumz)*pi/180), cos((45-sumz)*pi/180), 0);
	}//glTranslatef(-distanz*sin((45 - sumz)*pi / 180), -distanz* cos((45 - sumz)*pi / 180),-distanz* cos((45 - sumxy)*pi / 180));
	for (int i = 0; i < functionnumber; i++) 
		{ if (bfsupp[i]->checked())
			{ 
				glColor3f(red[i], green[i], blue[i]); if (d3d(fctsh[i]) == makeit3d)	drawfunction(fctsh[i], distanz); 
			} 
		};
	drawcoordinates(distanz);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glm::mat4 m = glm::lookAt(vec3(0,0, distanz), vec3(0, 0, 0), vec3(0, 1, 0));
	if (makeit3d)
	{
		m = glm::lookAt(vec3(distanz, distanz, distanz), vec3(0, 0, 0), vec3(0, 0, 1));
	}

	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(m));


	return true;
}

/****************************************************************************
stupid functions of the program
*****************************************************************************/


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
	glClearColor(0.7, 0.7, 0.7, 1);

	/*************
	Fenster rechts
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
		Dim->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 3));

		Button *dimension = new Button(Dim, "3D");
		dimension->setFixedSize(Vector2i(20,20));
		dimension->setFontSize(18);
		dimension->setTooltip("Dimension festlegen (2D/3D)");
		dimension->setBackgroundColor(Color(142, 69, 15, 255));
		dimension->setCallback([&dimension] {if (dimension->caption() == "2D") { dimension->setCaption("3D"); makeit3d = true; }
		else { dimension->setCaption("2D"); makeit3d = false; } });
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
		t->setValue("x^3");
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
			
		
			
		fctsh[functionnumber] = t->value(); fctsh[functionnumber] = to_postfix(fctsh[functionnumber]);
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

			/*Button *h3 = new Button(winner, "Farbe");
			h3->setFixedSize(Vector2i(88, 20));
			h3->setCallback([&]
			{
				winwin->dispose();
				Window *color = new Window(screen, "Farbe");
				color->setFixedSize(Vector2i(130, 150));
				color->setPosition(Vector2i(220, 150));
				color->setLayout(new GroupLayout());
				color->setVisible(true);
				color->setFocused(true);
				Widget *colorw = new Widget(color);
				colorw->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Maximum, 2, 6));*/
			cp = new ColorPicker(winner);
			cp->setColor(Vector4f(1,0,0,1));
			cp->setCallback([&](Color col) {
				red[i] = col[1];
				green[i] = col[2];
				blue[i] = col[3];
			});
			

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
					fctsh[i] = "0";
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
			fctsh[i - x] = fctsh[i];
			fctsh[i] = "0";
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
				fctsh[r] = s;
				bf[r]->setEnabled(true);
				bfsupp[r]->setEnabled(true);

			}
			
		}
		f.close();
		});

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
	screen->setVisible(true);
	screen->performLayout();


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float position[] = { -2.0f, 2.0f, -2.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);




	//glClearColor(1, 0, 0, 1);
	quadric = gluNewQuadric();

	// set up example projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glm::mat4 p = glm::perspective(glm::radians(60.0f), 3.0f / 2.0f, 0.1f, 100.0f);
	glLoadMatrixf(glm::value_ptr(p));

	// set up example model view matrix
	glMatrixMode(GL_MODELVIEW);
	glm::mat4 m = glm::lookAt(vec3(1,1,1), vec3(0, 0, 0), vec3(0, 0, 1));

	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(m));

	glPushMatrix();
	// the main loop. as long as the display funtion returns true and the window
	// should not be closed swap the buffers and poll events.*/
	while (display_funktion() && !glfwWindowShouldClose(win))
	{
	
		glfwPollEvents();
		// Draw nanogui 
		screen->drawContents();
		screen->drawWidgets();

		glfwSwapBuffers(win);

	}
	// clean up!
	glfwDestroyWindow(win);
	glfwTerminate();
	gluDeleteQuadric(quadric);



	return 0;
}


#endif
