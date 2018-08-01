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

nanogui::Slider *zoomslide = nullptr;
int width, height;
nanogui::Button *bf[10] = { nullptr };
nanogui::CheckBox *bfsupp[10] = { nullptr };
nanogui::Button *b = nullptr;
int functionnumber = 0;

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
	w3->setPosition(Eigen::Vector2i(10, height - 70));
	w->setPosition(Eigen::Vector2i(width - 210, 10));
	w2->setPosition(Eigen::Vector2i(15, 88));
}

bool wired = true;
bool moveit,moveitreally = false;
float x2, y2, xw, yw, counterz,diffz,counterxy,diffxy,sumz,sumxy = 0;
float poscam = 11;
std::string fctsh="x x * y y * + ";


/* This function is registered as the keyboard callback for GLFW, so GLFW
* will call this during glfwPollEvents() whenever a key is pressed. */
static void callback_Keyboard(GLFWwindow *win, int key, int scancode, int action, int mods)
{
	void* user_pointer = glfwGetWindowUserPointer(win);


	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
	{
		glfwSetWindowShouldClose(win, true);
	}

	if (key == GLFW_KEY_1 && action == GLFW_RELEASE)
	{
		glClearColor(1, 0, 0, 1);
	}
	if (key == GLFW_KEY_2 && action == GLFW_RELEASE)
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
		if (poscam > 1) { poscam--; }
		zoomslide->setValue((poscam - 1) / 20);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
	{
		if (poscam < 21) { poscam++; }
		zoomslide->setValue((poscam-1)/20);
	}
	screen->keyCallbackEvent(key, scancode, action, mods);


}

/* This function is registered as the mouse button callback for GLFW, so GLFW
* will call this during glfwPollEvents() whenever a button is pressed. */
static void callback_MouseButton(GLFWwindow *win, int button, int action, int mods)
{
	void* user_pointer = glfwGetWindowUserPointer(win);

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		moveit = true;
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
		if (diffz > 0 || diffxy > 0) { moveitreally = true; } 
	counterxy = y;
	counterz = x;
	//std::cout << diffz << "     " <<moveit<< std::endl;
	w->setPosition(Eigen::Vector2i(width - 210, 10));
	w3->setPosition(Eigen::Vector2i(10, height - 70));
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
	if (moveit && moveitreally) {
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
	drawfunction(fctsh, 2);
	drawcoordinates(2);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glm::mat4 m = glm::lookAt(vec3(poscam, poscam, poscam), vec3(0, 0, 0), vec3(0, 0, 1));

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
	glClearColor(1, 0, 0, 1);

	/*************
	Fenster rechts
	*************/
#if 1
	w = new nanogui::Window(screen, "");
	w->setFixedSize(Vector2i(200, 700));
	w->setPosition(Vector2i(width-210, 10));
	w->setLayout(new GroupLayout());



	/**************
	Label f�r 2D/3D
	***************/
	{
		new Label(w, "", "sans-bold");
		Widget *Dim = new Widget(w);
		Dim->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0, 10));

		CheckBox *cb1 = new CheckBox(Dim, "2D");
		cb1->setChecked(true); 
		CheckBox *cb2 = new CheckBox(Dim, "3D");
	}

	/****************
	Label f�r EINGABE
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
		b->setCallback([&t] {fctsh = t->value(); fctsh = to_postfix(fctsh); 

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
	Fenster f�r Funktions�bersicht
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
		bfsupp[i] = new CheckBox(Eingabe, "~");
		bfsupp[i]->setFixedSize(Vector2i(20, 20));
		bfsupp[i]->setVisible(true);
		bfsupp[i]->setEnabled(false);
		bfsupp[i]->setFontSize(15);
	}	

		
	/*************************************
	Label f�r Funktionsbearbeitungsbuttons
	*************************************/
	{
		//Button f�r alles markieren
		Button *mark = new Button(w, "Alles markieren");
		mark->setFixedSize(Vector2i(180, 20));
		mark->setFontSize(18);
		mark->setTooltip("Alle bisher hinzugefuegten Funktionen markieren");
		mark->setCallback([] {for (int i = 0; i < 10; i++)
			if (bfsupp[i]->enabled()) {
				bfsupp[i]->setChecked(true);
			}
		});


		//Button f�r l�schen
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
			bf[i]->setCaption("Funktion?");
			bf[i]->setEnabled(false);
			}
		});


		using namespace std;
		//Button f�r speichern
		Button *save = new Button(w, "Markierte speichern");
		save->setFixedSize(Vector2i(180, 20));
		save->setTextColor(Color(0, 255, 0, 200));
		save->setFontSize(18);
		save->setTooltip("Speichert alle markierten Funktionen");
		save->setCallback([] {fstream f;
		f.open("saved.dat", ios::out);
		for (int i = 0; i < 10; i++)
		if (bf[i]->caption() != "Funktion?")
		f << bf[i]->caption() << endl;
		f.close();	
		});
		//Button f�r Laden
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
		while (!f.eof())
		{
			r++;
			getline(f, s);
			if (s != "")
			{
				bf[r]->setCaption(s);
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
	w3->setPosition(Vector2i(10, height-70));
	w3->setLayout(new GroupLayout());
	//Label unten
	new Label(w3, "", "sans-bold");
	Widget *chgr = new Widget(w3);
	chgr->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Fill, 0, 69));

		ToolButton *screenshot = new ToolButton (chgr,ENTYPO_ICON_CAMERA);
		screenshot->setFontSize(20);
		screenshot->setSize(Vector2i(50, 50));
		screenshot->setTextColor(Color(12, 100, 130, 255));

			CheckBox *cam = new CheckBox(chgr, "Kamera"); cam->setChecked(true);
			CheckBox *obj = new CheckBox(chgr, "Objekt");
	
		ToolButton *reset = new ToolButton(chgr, ENTYPO_ICON_EYE);
		reset->setSize(Vector2i(50, 50));
		reset->setTextColor(Color(142, 69, 188, 255));
		
			Button *zoomin = new Button(chgr, "+");
			zoomin->setFixedSize(Vector2i(20, 20));
			zoomin->setFontSize(18);
			zoomin->setTooltip("Reinzoomen");

			zoomin->setCallback([] {if (poscam > 1) { poscam--; }
			zoomslide->setValue((poscam - 1) / 20); });

			zoomslide = new Slider(chgr);
			zoomslide->setFixedWidth(169);
			zoomslide->setValue(0.5);
			zoomslide->setCallback([](float value){ poscam = (20 * value)+1; std::cout << value << std::endl; });
			
			Button *zoomout = new Button(chgr, "-");
			zoomout->setFixedSize(Vector2i(20, 20));
			zoomout->setFontSize(18);
			zoomout->setTooltip("Rauszoomen");
			zoomout->setCallback([] {if (poscam < 21) { poscam++; }
			zoomslide->setValue((poscam - 1) / 20); });

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
	glm::mat4 m = glm::lookAt(vec3(poscam,poscam,poscam), vec3(0, 0, 0), vec3(0, 0, 1));

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

