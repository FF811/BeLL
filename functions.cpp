#include "functions.h"
#include "math.h"

/******************************************************************************
Funktionskram
*****************************************************************************/


#include <string>
#include <stack>
using namespace std;

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
char iffunction(int i, string post)
{
	string f = "";
	for (int j = i; post[j] <= 'z' && post[j] >= 'a'; ++j)
	{
		f += post[j];
	}
	if (f == "sin" || f == "cos" || f == "tan" || f == "log" || f == "rot" /*|| f == "ln"*/) return f[0];
	else return 'n';
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
			if (i >= 3) {
				if (iffunction(i - 3, infix) != 'n') {
					s.push(iffunction(i - 3, infix));
				}
				else s.push('(');
			}
			else { s.push('('); }
		}
		else if (infix[i] == ')' && !s.empty())
		{
			while (s.top() != '(' && !(s.top()>='a' && s.top()<= 'z'))
			{
				post += s.top();
				post += ' ';
				s.pop();
			}
			if (s.top() == '(') { s.pop(); }
			else {
				post += s.top();
				post += ' ';
				s.pop();
			}
		}

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
			float e = pow(op2,op1);
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
				char f = post[i];
				op1 = s.top();
				s.pop();
				if (f == 's') op1 = (sin(op1*pi / 180));
				else if (f == 'c') op1 = (cos(op1*pi / 180));
				//else if (f == 'l') op1 = (log10(op1));
				else if (f == 't') op1 = (tan(op1*pi / 180));
				else if (f == 'r')op1 = (sqrt(op1));
				else if (f == 'l') s.push(log(op1));
				//if (s.top() <= 0.000000001 && s.top() >= -0.000000001) { s.pop(); s.push(0.0); }
				s.push(op1);
			
		}

	}
	return s.top();
}


