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


// GLM provides matries and vectors
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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

/******************************************************************************
Funktionskram
*****************************************************************************/

using namespace std;

#define pi   3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095 

//checks if there is an second variable in the entered function-string
bool d3d(string s)
{
	s += ' ';
	for (int i = 0; i < s.length(); ++i)
		if (s[i] == 'y' && !(s[i + 1] <= 'z' && s[i + 1] >= 'a')) return true;
	return false;
}

//checks if c is an operator
bool ifoperator(char C)
{
	if (C == '+' || C == '-' || C == '*' || C == '/' || C == '^')
	{
		return true;
	}
	return false;
}

//checks if there is a function at point i of the string, returns that function. Otherwise returns "no"
string iffunction(int i, string post)
{
	string f = "";
	for (int j = i; post[j] <= 'z' && post[j] >= 'a'; ++j)
	{
		f += post[j];
	}
	if (f == "sin" || f == "cos" || f == "tan" || f == "log" || f == "sqrt" /*|| f == "ln"*/) return f;
	else return "no";
}

//output: a value for comparing operators
int getvalue(char c)
{
	if (c == '+' || c == '-') { return 1; }
	else if (c == '*' || c == '/') { return 2; }
	else if (c == '^') { return 3; }
	return 0;
}

//changes infix to postfix
std::string to_postfix(std::string infix)
{
	stack<char> s;
	stack<string> function;
	string post = "";
	string t = "";
	for (int i = 0; i < infix.length(); ++i)
	{
		if (ifoperator(infix[i]))
		{
			if (!s.empty())
			{
				while (!s.empty() && getvalue(s.top()) >= getvalue(infix[i]))
				{
					post += s.top();
					post += ' ';
					s.pop();
				}
			}
			s.push(infix[i]);
		}
		else if (infix[i] >= '0' && infix[i] <= '9')
		{
			t += infix[i];
			if (!(infix[i + 1] >= '0' && infix[i + 1] <= '9'))
			{
				post += t;
				post += ' ';
				t = "";
			}
		}
		else if (infix[i] == 'x' || infix[i] == 'y')
		{
			post += infix[i];
			post += ' ';
		}
		else if (infix[i] == '(')
		{
			s.push('(');
		}
		else if (infix[i] == ')' && !s.empty())
		{
			while (s.top() != '(')
			{
				post += s.top();
				post += ' ';
				s.pop();
			}
			s.pop();
			if (!function.empty())
			{
				post += function.top();
				function.pop();
				post += ' ';
			}
		}
		else if (infix[i] >= 'a' && infix[i] <= 'z')
			if (iffunction(i, infix) != "no") { function.push(iffunction(i, infix)); }

	}
	while (!s.empty())
	{
		post += s.top();
		post += ' ';
		s.pop();
	}
	return post;
}

//changes postfix with variables x,y into a value | set d false to ignore y
float to_value(string post, float x, float y, bool d)
{
	stack<float> s;
	int t = 0;
	for (int i = 0; i < post.length(); ++i)
	{
		float op1, op2 = 0;
		if (post[i] == 'x')
		{
			s.push(x);
		}
		else if (post[i] == 'y')
		{
			if (d) s.push(y);
		}
		else if (post[i] == '+')
		{
			op1 = s.top();
			s.pop();
			op2 = s.top();
			s.pop();
			op1 = op1 + op2;
			s.push(op1);
		}
		else if (post[i] == '-')
		{
			op1 = s.top(); s.pop();
			op2 = s.top(); s.pop();
			op1 = op2 - op1;
			s.push(op1);
		}
		else if (post[i] == '*')
		{
			op1 = s.top(); s.pop();
			op2 = s.top(); s.pop();
			op1 = op1 * op2;
			s.push(op1);
		}
		else if (post[i] == '/')
		{
			op1 = s.top(); s.pop();
			op2 = s.top(); s.pop();
			op1 = op2 / op1;
			s.push(op1);
		}
		else if (post[i] == '^')
		{
			op1 = s.top(); s.pop();
			op2 = s.top(); s.pop();
			float e = 1;
			for (int j = 0; j < op1; ++j)
			{
				e = e * op2;
			}
			s.push(e);
		}
		else if (post[i] >= '0' && post[i] <= '9')
		{
			t = 10 * t + post[i] - '0';
			if (post[i + 1] == ' ')
			{
				s.push(t);
				t = 0;
			}
		}
		else if (post[i] >= 'a' && post[i] <= 'z')
		{
			string f = iffunction(i, post);
			if (f != "no")
			{
				op1 = s.top();
				s.pop();
				if (f == "sin") s.push(sin(op1*pi / 180));
				else if (f == "cos") s.push(cos(op1*pi / 180));
				else if (f == "log") s.push(log10(op1));
				else if (f == "tan") s.push(tan(op1*pi / 180));
				else if (f == "sqrt") s.push(sqrt(op1));
				else if (f == "ln") s.push(log(op1));
				if (s.top() <= 0.000000001 && s.top() >= -0.000000001) { s.pop(); s.push(0.0); }
			}
		}

	}
	return s.top();
}



// main display function. this will be called once per frame.
bool display_funktion()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float x = -2;
	float y = -2;
	
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
	if (d3d(fctsh)) 
	{
		while (x < 2)
		{	
			glBegin(GL_TRIANGLE_STRIP);
			y = -2;
			while (y < 2) {
				glVertex3f(x, y, to_value(fctsh, x, y, true));
				glVertex3f(x+0.4, y, to_value(fctsh, x+0.4, y, true));
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
			
			glVertex3f(x, to_value(fctsh, x, y, false),0);
			x = x + 0.05;
			
		}
		glEnd();
	}

	{glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	glVertex3f(-10, 0, 0); glVertex3f(10, 0, 0);
	glColor3f(1, 0, 0);
	glVertex3f(0, -10, 0); glVertex3f(0, 10, 0);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, -10); glVertex3f(0, 0, 10);
	glEnd(); }

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
	Label für 2D/3D
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
		t->setValue("x x *");
		t->setFontSize(20);
		t->setTooltip("Funktion eingeben");
		//t->setCallback( [] {btnaddcallback(); });
		//t->setCallback([&]{t->setValue(""); });

		//Button neben Textfeld
		Button *a = new Button(Eingabe, "~");
		a->setFixedSize(Vector2i(20, 20));
		a->setFontSize(28);
		a->setTooltip("Vorlagen");
	

		//add Button
		Button *b = new Button(Eingabe, "+");
		b->setFixedSize(Vector2i(20, 20));
		b->setFontSize(28);
		b->setTooltip("Funktion hinzufuegen");
		b->setCallback([&t] {fctsh = t->value(); fctsh = to_postfix(fctsh); 
		
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
	/*************************************
	Label für Funktionsbearbeitungsbuttons
	*************************************/
	{
		//Button für alles markieren
		Button *mark = new Button(w, "Alles markieren");
		mark->setFixedSize(Vector2i(180, 20));
		mark->setFontSize(18);
		mark->setTooltip("Alle bisher hinzugefuegten Funktionen markieren");

		//Button für löschen
		Button *destroy = new Button(w, "Markierte loeschen");
		destroy->setFixedSize(Vector2i(180, 20));
		destroy->setTextColor(Color(255, 0, 0, 200));
		destroy->setFontSize(18);
		destroy->setTooltip("Loescht alle markierten Funktionen unwiederruflich");

		//Button für speichern
		Button *save = new Button(w, "Markierte speichern");
		save->setFixedSize(Vector2i(180, 20));
		save->setTextColor(Color(0, 255, 0, 200));
		save->setFontSize(18);
		save->setTooltip("Speichert alle markierten Funktionen");

		//Button für Laden
		Button *load = new Button(w, "Gespeicherte laden");
		load->setFixedSize(Vector2i(180, 20));
		load->setFontSize(18);
		load->setTextColor(Color(14, 14, 130, 250));
		load->setTooltip("Oeffnet schon gespeicherte Funktionen");


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

