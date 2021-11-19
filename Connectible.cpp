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
	if (selected) {
		borderColor.SetGlColor();
		glBegin(GL_QUADS);
			glVertex2f(this->x - (this->width + this->borderWidth) / 2, this->y - (this->height + this->borderHeight) / 2); // BOTTOM LEFT
			glVertex2f(this->x + (this->width + this->borderWidth) / 2, this->y - (this->height + this->borderHeight) / 2); // BOTTOM RIGHT
			glVertex2f(this->x + (this->width + this->borderWidth) / 2, this->y + (this->height + this->borderHeight) / 2); // TOP RIGHT
			glVertex2f(this->x - (this->width + this->borderWidth) / 2, this->y + (this->height + this->borderHeight) / 2); // TOP LEFT
		glEnd();
	}

	baseColor.SetGlColor();
	glBegin(GL_QUADS);
		glVertex2f(this->x - this->width / 2, this->y - this->height / 2); // BOTTOM LEFT
		glVertex2f(this->x + this->width / 2, this->y - this->height / 2); // BOTTOM RIGHT
		glVertex2f(this->x + this->width / 2, this->y + this->height / 2); // TOP RIGHT
		glVertex2f(this->x - this->width / 2, this->y + this->height / 2); // TOP LEFT
	glEnd();
}

bool Connectible::IsConnected(Connectible* other) {
	if (!this->connectedConnectibles.empty()) {
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

void Receiver::Draw(bool selected) {
	Connectible::Draw(selected);

	// TODO: Display energy consumption (by dividing the Connectible square)
	/*if (this->energyConsumed > 1) {
		glColor3f(0, 0, 0);
		glLineWidth(5);
		for (int i = 1; i <= this->energyConsumed; i++) {
			
			glBegin(GL_LINES);
			if (i % 2 == 0) { // Vertical
				glVertex2f((this->x - this->width / 2) + (this->width / (i * (i / (float) 2))), this->y + this->height / 2); // TOP
				glVertex2f((this->x - this->width / 2) + (this->width / (i * (i / (float) 2))), this->y - this->height / 2); // BOTTOM
			}
			else { // Horizontal

			}
			glEnd();
		}
	}*/

	// Display operation state
	if (this->connectedGrid && this->connectedGrid->consumedEnergy > this->connectedGrid->generatedEnergy) {
		glColor3f(1, 0, 0);
		glLineWidth(5);
		glBegin(GL_LINES);
			glVertex2f(this->x - (this->width + this->borderWidth) / 2, this->y + (this->height + this->borderHeight) / 2); // TOP LEFT
			glVertex2f(this->x + (this->width + this->borderWidth) / 2, this->y - (this->height + this->borderHeight) / 2); // BOTTOM RIGHT
		glEnd();
	}
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
	if (distance < this->maximumDistance) {
		this->maximumDistance = distance;
		this->index = connectibleIndex;
	}
}