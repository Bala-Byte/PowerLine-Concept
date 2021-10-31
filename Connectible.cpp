#include "Connectible.h"

// GRID

ConnectibleGrid::ConnectibleGrid()
{
	this->gridColor.RandomizeColors();
}

void ConnectibleGrid::ResetEnergy() {
	this->consumedEnergy = 0;
	this->generatedEnergy = 0;
}

// BASE CLASS

Connectible::Connectible(float x, float y)
	:x(x), y(y)
{
};

void Connectible::Draw(bool selected) {
	// TODO: Display energy generation/consumption (by dividing the square)
	// TODO: Display if receiver is not operation
	if (selected) {
		borderColor.GlColor();
		glBegin(GL_QUADS);
			glVertex2f(this->x - (this->width + this->borderWidth) / 2, this->y - (this->height + this->borderHeight) / 2); // BOTTOM LEFT
			glVertex2f(this->x + (this->width + this->borderWidth) / 2, this->y - (this->height + this->borderHeight) / 2); // BOTTOM RIGHT
			glVertex2f(this->x + (this->width + this->borderWidth) / 2, this->y + (this->height + this->borderHeight) / 2); // TOP RIGHT
			glVertex2f(this->x - (this->width + this->borderWidth) / 2, this->y + (this->height + this->borderHeight) / 2); // TOP LEFT
		glEnd();
	}

	baseColor.GlColor();
	glBegin(GL_QUADS);
		glVertex2f(this->x - this->width / 2, this->y - this->height / 2); // BOTTOM LEFT
		glVertex2f(this->x + this->width / 2, this->y - this->height / 2); // BOTTOM RIGHT
		glVertex2f(this->x + this->width / 2, this->y + this->height / 2); // TOP RIGHT
		glVertex2f(this->x - this->width / 2, this->y + this->height / 2); // TOP LEFT
	glEnd();
}

bool Connectible::IsConnected(Connectible* other) {
	if (this->connectedConnectibles.size() > 0) {
		for (Connectible* nthConected : this->connectedConnectibles) {
			if (nthConected == other) {
				return true;
			}
		}
	}
	return false;
}

vector<Connectible*> Connectible::RecursiveSearch(Connectible* previous) {
	vector<Connectible*> returnVector;
	//std::cout << this << std::endl;
	for (Connectible* nthConected : this->connectedConnectibles) {
		if (nthConected != previous) {
			vector<Connectible*> recursiveValue = nthConected->RecursiveSearch(this);
			returnVector.insert(returnVector.end(), recursiveValue.begin(), recursiveValue.end());
		}
	}
	returnVector.push_back(this);
	return returnVector;
}

void Connectible::AddEnergyToGrid() {
	
};

void Connectible::Connect(ConnectibleGrid* grid, Connectible* other) {
	this->connectedGrid = grid;

	if (other) {
		this->connectedConnectibles.push_back(other);
		other->connectedConnectibles.push_back(this);
	}
}

void Connectible::Disconnect(Connectible* other) {
	for (int i = 0; i < this->connectedConnectibles.size(); i++) {
		if (this->connectedConnectibles[i] == other) {
			this->connectedConnectibles.erase(this->connectedConnectibles.begin() + i);
		}
	}
}

// POWER POLE

PowerPole::PowerPole(float x, float y) : Connectible(x, y) {
	baseColor = Color(1.0f, 1.0f, 1.0f);
	this->width = .1f;
	this->height = .1f;
}

/*void PowerPole::Connect(ConnectibleGrid* grid, Connectible* other)
{
	grid->poles.push_back(this);
	this->connectedGrid = grid;

	if(other) {
		this->connectedConnectibles.push_back(other);
		other->connectedConnectibles.push_back(this);
	}
}*/

// GENERATOR

Generator::Generator(float x, float y) : Connectible(x, y) {
	baseColor = Color(1.0f, 0.0f, 0.0f);
	this->width = .3f;
	this->height = .3f;
}

/*void Generator::Connect(ConnectibleGrid* grid Connectible* other) {
	grid->generators.push_back(this);
	this->connectedGrid = grid;

	if (other) {
		this->connectedConnectibles.push_back(other);
		other->connectedConnectibles.push_back(this);
	}
}*/

void Generator::AddEnergyToGrid() {
	this->connectedGrid->generatedEnergy += this->energyGenerated;
};

// RECEIVER

Receiver::Receiver(float x, float y) : Connectible(x, y) {
	baseColor = Color(0.0f, 1.0f, 0.5f);
	this->width = .2f;
	this->height = .2f;
}

/*void Receiver::Connect(ConnectibleGrid* grid Connectible* other)
{
	grid->receivers.push_back(this);
	this->connectedGrid = grid;

	if(other) {
		this->connectedConnectibles.push_back(other);
		other->connectedConnectibles.push_back(this);
	}
}*/

void Receiver::AddEnergyToGrid() {
	this->connectedGrid->consumedEnergy += this->energyConsumed;
};

// CLOSEST CONNECTIBLE

void ClosestConnectible::Check(double distance, int connectibleIndex)
{
	if (distance < this->distance) {
		this->distance = distance;
		this->index = connectibleIndex;
	}
}