#include "Color.h"

Color::Color(float red, float green, float blue) 
	: red(red), green(green), blue(blue)
{
	/*this->red = red;
	this->green = green;
	this->blue = blue;*/
}

Color::Color(int red, int green, int blue)
	: red((float) red / 255), green((float) green / 255), blue((float) blue / 255)
{
}

void Color::RandomizeColors()
{
	this->red = rand() / (float)RAND_MAX;
	this->green = rand() / (float)RAND_MAX;
	this->blue = rand() / (float)RAND_MAX;
}

void Color::SetGlColor()
{
	glColor3f(this->red, this->green, this->blue);
}
