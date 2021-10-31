#pragma once
#include <vector>
#include "Color.h"

using std::vector;

class Connectible;

class ConnectibleGrid {
	public:
		//vector<Connectible*> poles;
		//vector<Connectible*> generators;
		//vector<Connectible*> receivers;
		//vector<vector<int>> renderedLines;
		int generatedEnergy = 0;
		int consumedEnergy = 0;
		Color gridColor = Color(0, 0, 0);

		ConnectibleGrid();
		void ResetEnergy();
};


class Connectible
{
	public:
		float x = 0, y = 0,
			width = 0,
			height = 0;
			
		Color baseColor = Color(0.0f, 0.0f, 0.0f);
		Color borderColor = Color(0.1f, 0.1f, 1.0f);

		float borderWidth = 0.02f,
			borderHeight = 0.02f;

		ConnectibleGrid* connectedGrid = nullptr;
		vector<Connectible*> connectedConnectibles;

		Connectible(float x, float y);
		void Draw(bool selected);
		bool IsConnected(Connectible* other);
		vector<Connectible*> RecursiveSearch(Connectible* previous);
		virtual void AddEnergyToGrid();

		void Connect(ConnectibleGrid* grid, Connectible* other);
		void Disconnect(Connectible* other);
};

class PowerPole : public Connectible {
	public:
		PowerPole(float x, float y);
};

class Generator : public Connectible {
	public:
		int energyGenerated = 1;

		Generator(float x, float y);
		void AddEnergyToGrid();
};

class Receiver : public Connectible {
	public:
		int energyConsumed = 1;

		Receiver(float x, float y);
		void AddEnergyToGrid();
};

// Helping class
class ClosestConnectible {
	public:
		double distance = 5;
		int index = -1;

		void Check(double distance, int connectibleIndex);
};