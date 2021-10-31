#pragma once
#include <GLFW/glfw3.h>
#include <cstdlib>

class Color
{
	public:
		float red,
			green,
			blue;

		Color(int red, int green, int blue);
		Color(float red, float green, float blue);
		void RandomizeColors();
		void GlColor();
};

