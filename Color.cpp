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
	this->red = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	this->green = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	this->blue = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void Color::GlColor()
{
	glColor3f(this->red, this->green, this->blue);
}
