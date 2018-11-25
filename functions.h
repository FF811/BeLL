#pragma once

/******************************************************************************
Funktionskram
*****************************************************************************/


#include <string>

using namespace std;
#define pi   3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095 

//checks if there is an second variable in the entered function-string
bool d3d(string s);

//checks if c is an operator
bool ifoperator(char C);
//checks if there is a function at point i of the string, returns that function. Otherwise returns "no"
char iffunction(int i, string post);

//output: a value for comparing operators
int getvalue(char c);


//changes infix to postfix
std::string to_postfix(std::string infix);

//changes postfix with variables x,y into a value | set d false to ignore y
float to_value(string post, float x, float y, bool d);
