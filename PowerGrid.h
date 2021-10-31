#pragma once
#include <vector>
#include "Connectible.h"
#include "Color.h"

using std::vector;

class ConnectibleGrid {
public:
	vector<Connectible*> poles;
	vector<int> generators;
	vector<int> receivers;
	vector<vector<int>> renderedLines;
	Color gridColor = Color(0, 0, 0);

	ConnectibleGrid();
};