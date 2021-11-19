#include <iostream>
#include "Connectible.h"

constexpr int WINDOW_WIDTH = 640,
			  WINDOW_HEIGHT = 640;

constexpr float barHeight = 0.025f,
				cellPadding = 0.010f;

int viewWidth = WINDOW_WIDTH,
	viewHeight = WINDOW_HEIGHT;

using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::make_shared;

vector<shared_ptr<Connectible>> renderedConnectibles;
vector<ConnectibleGrid*> connectibleGrids;
Connectible* selectedConnectible = nullptr;

void DestroyGrid(ConnectibleGrid* grid) {
	for (int k = 0; k < connectibleGrids.size(); k++) {
		if (connectibleGrids[k] == grid) {
			free(connectibleGrids[k]);
			connectibleGrids.erase(connectibleGrids.begin() + k);
			break;
		}
	}

	cout << ">DESTROYED GRID< ";
}

ConnectibleGrid* CreateGrid() {
	ConnectibleGrid* allocatedGrid = (ConnectibleGrid*)malloc(sizeof(ConnectibleGrid));

	if (allocatedGrid != NULL) {
		connectibleGrids.push_back(allocatedGrid);
		connectibleGrids.back() = new ConnectibleGrid();
		cout << "<NEW GRID> ";

		return connectibleGrids.back();
	}

	cout << "!!GRID ALLOCATION FAILED!!";
	// Recursion might not be the best solution
	return CreateGrid();
}

void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Only on mouse down
	if (action == 1) {
		double cursorPositionX, cursorPositionY;
		glfwGetCursorPos(window, &cursorPositionX, &cursorPositionY);

		double cursorWindowPositionX = (cursorPositionX * 2 - viewWidth) / viewWidth,
				cursorWindowPositionY = -(cursorPositionY * 2 - viewHeight) / viewHeight;

		cout << cursorPositionX << " : " << cursorPositionY << " = " << cursorWindowPositionX << " : " << cursorWindowPositionY << " - ";

		ClosestConnectible closestConnectible = ClosestConnectible();

		for (int i = 0; i < renderedConnectibles.size(); i++) {

			// Check cursor hit
			if ((cursorWindowPositionX < renderedConnectibles[i]->x + renderedConnectibles[i]->width / 2 && cursorWindowPositionX > renderedConnectibles[i]->x - renderedConnectibles[i]->width / 2)
				&&
				(cursorWindowPositionY < renderedConnectibles[i]->y + renderedConnectibles[i]->height / 2 && cursorWindowPositionY > renderedConnectibles[i]->y - renderedConnectibles[i]->height / 2))
			{
				#ifdef _DEBUG
					if (button == 2) {
						cout << renderedConnectibles[i] << endl;
						return;
					}
				#endif

				// Check for selected
				if (selectedConnectible != nullptr && selectedConnectible != renderedConnectibles[i].get()) {
					// 0 - Left
					// 1 - Right
					// 2 - Middle
					if (button == 0) { // Power Line Construction
						// Connect according to the situation
						if (renderedConnectibles[i]->connectedGrid == nullptr && selectedConnectible->connectedGrid != nullptr) { // Connect to previous

							renderedConnectibles[i]->Connect(selectedConnectible->connectedGrid, selectedConnectible);
							renderedConnectibles[i]->AddEnergyToGrid();
						}
						else if (renderedConnectibles[i]->connectedGrid != nullptr && selectedConnectible->connectedGrid == nullptr) { // Connect to this

							selectedConnectible->Connect(renderedConnectibles[i]->connectedGrid, renderedConnectibles[i].get());
							selectedConnectible->AddEnergyToGrid();
						}
						else if (renderedConnectibles[i]->connectedGrid == nullptr && selectedConnectible->connectedGrid == nullptr) { // New Grid

							ConnectibleGrid* createdGrid = CreateGrid();

							// Connect
							renderedConnectibles[i]->Connect(createdGrid, nullptr);
							renderedConnectibles[i]->AddEnergyToGrid();

							selectedConnectible->Connect(createdGrid, renderedConnectibles[i].get());
							selectedConnectible->AddEnergyToGrid();
						}
						else if (renderedConnectibles[i]->connectedGrid == selectedConnectible->connectedGrid) { // Share the same network
							cout << "!Already ";
						}
						else if (renderedConnectibles[i]->connectedGrid != nullptr && selectedConnectible->connectedGrid != nullptr) { // Merge Grigs
							
							vector<Connectible*> mergedGrid = renderedConnectibles[i]->RecursiveSearch(nullptr);
							DestroyGrid(renderedConnectibles[i]->connectedGrid);

							for (Connectible* nthConnectible : mergedGrid) {
								nthConnectible->connectedGrid = selectedConnectible->connectedGrid;
								nthConnectible->AddEnergyToGrid();
							}

							selectedConnectible->Connect(selectedConnectible->connectedGrid, renderedConnectibles[i].get());
						}

						cout << "Connected Poles" << endl;
					}
					else if (button == 1) { // Power Line Destruction
						
						if (selectedConnectible->IsConnected(renderedConnectibles[i].get())) {

							vector<Connectible*> otherGrid;

							// Disconnect connectibles
							selectedConnectible->Disconnect(renderedConnectibles[i].get());
							renderedConnectibles[i]->Disconnect(selectedConnectible);

							// Handle disconnection
							if (renderedConnectibles[i]->connectedConnectibles.empty() && !selectedConnectible->connectedConnectibles.empty()) { // Disconnect from this
								renderedConnectibles[i]->connectedGrid = nullptr;

								otherGrid = selectedConnectible->RecursiveSearch(nullptr);
							}
							else if (!renderedConnectibles[i]->connectedConnectibles.empty() && selectedConnectible->connectedConnectibles.empty()) { // Disconnect from previous
								selectedConnectible->connectedGrid = nullptr;

								otherGrid = renderedConnectibles[i]->RecursiveSearch(nullptr);
							}
							else if (!renderedConnectibles[i]->connectedConnectibles.empty() && !selectedConnectible->connectedConnectibles.empty()) { // Disconnect to separate networks
								vector<Connectible*> newGrid = renderedConnectibles[i]->RecursiveSearch(nullptr);
								otherGrid = selectedConnectible->RecursiveSearch(nullptr);

								ConnectibleGrid* createdGrid = CreateGrid();
								
								for (Connectible* c : newGrid) {
									c->connectedGrid = createdGrid;
									c->AddEnergyToGrid();
								}
							}
							else if (renderedConnectibles[i]->connectedConnectibles.empty() && selectedConnectible->connectedConnectibles.empty()) { // Disconnect with no other connections
								DestroyGrid(renderedConnectibles[i]->connectedGrid);

								renderedConnectibles[i]->connectedGrid = nullptr;
								selectedConnectible->connectedGrid = nullptr;
							}

							if (!otherGrid.empty()) {
								otherGrid[0]->connectedGrid->ResetEnergy();
								for (Connectible* c : otherGrid) {
									c->AddEnergyToGrid();
								}
							}
						}
						else {
							cout << "!Already ";
						}

						cout << "Disconnected Poles" << endl;
					}
					else {
						cout << "!Not yet hadeled mouse event!" << endl;
					}

					selectedConnectible = nullptr;
				}
				else
				{
					cout << "Selected Pole" << endl;
					selectedConnectible = renderedConnectibles[i].get();
				}
				return;
			}

			// TODO: Pass as pointer
			// TODO: Calculate distance on different thread
			if(mods != 0 && button == 0)
				closestConnectible.Check(sqrt(pow(cursorWindowPositionX - renderedConnectibles[i]->x, 2) + pow(cursorWindowPositionY - renderedConnectibles[i]->y, 2)), i);
		}

		if (closestConnectible.index != -1 && closestConnectible.maximumDistance < 0.5) {
			if (closestConnectible.maximumDistance < (renderedConnectibles[closestConnectible.index]->width + renderedConnectibles[closestConnectible.index]->height) / 2) {
				cout << "!TOO CLOSE" << endl;
				return;
			}

			if (cursorWindowPositionX < renderedConnectibles[closestConnectible.index]->x + renderedConnectibles[closestConnectible.index]->width / 2 && cursorWindowPositionX > renderedConnectibles[closestConnectible.index]->x - renderedConnectibles[closestConnectible.index]->width / 2) {
				cout << "X Snapped: ";
				cursorWindowPositionX = renderedConnectibles[closestConnectible.index]->x;
			}
			else if (cursorWindowPositionY < renderedConnectibles[closestConnectible.index]->y + renderedConnectibles[closestConnectible.index]->height / 2 && cursorWindowPositionY > renderedConnectibles[closestConnectible.index]->y - renderedConnectibles[closestConnectible.index]->height / 2) {
				cout << "Y Snapped: ";
				cursorWindowPositionY = renderedConnectibles[closestConnectible.index]->y;
			}
		}

		switch (mods)
		{
		case 1: // SHIFT: Pole
			cout << "New Pole" << endl;
			renderedConnectibles.push_back(make_shared<PowerPole>((float)cursorWindowPositionX, (float)cursorWindowPositionY));
			break;

		case 2: // CTRL: Generator
			cout << "New Generator" << endl;
			renderedConnectibles.push_back(make_shared<Generator>((float)cursorWindowPositionX, (float)cursorWindowPositionY));
			break;

		case 4: // ALT: Reciever
			cout << "New Receiver" << endl;
			renderedConnectibles.push_back(make_shared<Receiver>((float)cursorWindowPositionX, (float)cursorWindowPositionY));
			break;

		default: // 0: Reset Power Line Connection
			cout << "Reset" << endl;
			selectedConnectible = nullptr;
			break;
		}
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	viewWidth = width;
	viewHeight = height;

	//widthRatio = (float)WINDOW_WIDTH / viewWidth;
	//heightRatio = (float)WINDOW_WIDTH / viewWidth;

	glViewport(0, 0, width, height);
}

/*void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action) {
		cout << "key: " << key << "; scancode " << scancode << endl;
	}
}*/

int getConnectibleIndexByPointer(Connectible* pointer) {
	for (int i = 0; i < renderedConnectibles.size(); i++) {
		if (renderedConnectibles[i].get() == pointer)
			return i;
	}
	return -1;
}

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Power Lines", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	cout << "--Running--" << endl;

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Events */
	glfwSetMouseButtonCallback(window, mouseClickCallback);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	//glfwSetKeyCallback(window, keyCallback);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		/* Render here */
		// Draw Power Lines
		for (int i = 0; i < renderedConnectibles.size(); i++) {
			if (!renderedConnectibles[i]->connectedConnectibles.empty()) {
				for (int j = 0; j < renderedConnectibles[i]->connectedConnectibles.size(); j++) {
					if (getConnectibleIndexByPointer(renderedConnectibles[i]->connectedConnectibles[j]) > i) {
						glColor3f(renderedConnectibles[i]->connectedGrid->gridColor.red, renderedConnectibles[i]->connectedGrid->gridColor.green, renderedConnectibles[i]->connectedGrid->gridColor.blue);
						glLineWidth(5.0f);
						glBegin(GL_LINES);
							glVertex2f(renderedConnectibles[i]->x, renderedConnectibles[i]->y);
							glVertex2f(renderedConnectibles[i]->connectedConnectibles[j]->x, renderedConnectibles[i]->connectedConnectibles[j]->y);
						glEnd();
					}
				}
			}
		}

		/*
		for (int i = 0; i < connectibleGrids.size(); i++) {
			for (int j = 0; j < connectibleGrids[i].renderedLines.size(); j++) {
				glColor3f(connectibleGrids[i].gridColor.red, connectibleGrids[i].gridColor.green, connectibleGrids[i].gridColor.blue);
				glLineWidth(5.0f);
				glBegin(GL_LINES);
					glVertex2f(renderedConnectibles[connectibleGrids[i].renderedLines[j][0]]->x, renderedConnectibles[connectibleGrids[i].renderedLines[j][0]]->y);
					glVertex2f(renderedConnectibles[connectibleGrids[i].renderedLines[j][1]]->x, renderedConnectibles[connectibleGrids[i].renderedLines[j][1]]->y);
				glEnd();
			}
		}
		*/

		// Draw Connectibles
		for (int i = 0; i < renderedConnectibles.size(); i++) {
			renderedConnectibles[i]->Draw(renderedConnectibles[i].get() == selectedConnectible);
		}

		// Draw Grid Energy Bar
		int operationalGridCounter = 0;
		for (int i = 0; i < connectibleGrids.size(); i++) {
			if (connectibleGrids[i]->generatedEnergy > 0) {

				/*if (connectibleGrids[i].consumedEnergy > connectibleGrids[i].generatedEnergy) {
					glColor3f(1, 0, 0);
					glBegin(GL_QUADS);
						glVertex2f(-1, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight)); // BOTTOM LEFT
						glVertex2f(1, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight)); // BOTTOM RIGHT
						glVertex2f(1, 1 - ((barHeight + cellPadding) * operationalGridCounter)); // TOP RIGHT
						glVertex2f(-1, 1 - ((barHeight + cellPadding) * operationalGridCounter)); // TOP LEFT
					glEnd();
				}*/

				for (int j = 0; j < connectibleGrids[i]->generatedEnergy; j++) {
					connectibleGrids[i]->gridColor.SetGlColor();
					glBegin(GL_QUADS);
						glVertex2f(-1 + j / (float)connectibleGrids[i]->generatedEnergy * 2 + cellPadding, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight)); // BOTTOM LEFT
						glVertex2f(-1 + (j + 1) / (float)connectibleGrids[i]->generatedEnergy * 2 - cellPadding, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight)); // BOTTOM RIGHT
						glVertex2f(-1 + (j + 1) / (float)connectibleGrids[i]->generatedEnergy * 2 - cellPadding, 1 - ((barHeight + cellPadding) * operationalGridCounter)); // TOP RIGHT
						glVertex2f(-1 + j / (float)connectibleGrids[i]->generatedEnergy * 2 + cellPadding, 1 - ((barHeight + cellPadding) * operationalGridCounter)); // TOP LEFT
					glEnd();

					if (connectibleGrids[i]->consumedEnergy > connectibleGrids[i]->generatedEnergy) { // Consumption exceeds generation
						glColor3f(1, 0, 0);
						glLineWidth(barHeight * 150);
						glBegin(GL_LINES);
							glVertex2f(-1, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight / 2)); // LEFT
							glVertex2f(1, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight / 2)); // RIGHT
						glEnd();

					} else if (connectibleGrids[i]->consumedEnergy > j) {
						glColor3f(0, 0, 0);
						glLineWidth(barHeight * 150);
						glBegin(GL_LINES);
							glVertex2f(-1 + j / (float)connectibleGrids[i]->generatedEnergy * 2 + cellPadding * 2, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight / 2)); // LEFT
							glVertex2f(-1 + (j + 1) / (float)connectibleGrids[i]->generatedEnergy * 2 - cellPadding * 2, 1 - ((barHeight + cellPadding) * operationalGridCounter + barHeight / 2)); // RIGHT
						glEnd();
					}
				}

				operationalGridCounter++;
			}
		}
		
		/*
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
			glVertex2f(-1, 0); // BOTTOM LEFT
			glVertex2f(0, 0); // BOTTOM RIGHT
			glVertex2f(0, 1); // TOP RIGHT
			glVertex2f(-1, 1); // TOP LEFT
		glEnd();

		// Draw squares with triangles
		glColor3f(0, 1, 0);
		glBegin(GL_TRIANGLES);
			glVertex2f(0, 0);
			glVertex2f(1, 0);
			glVertex2f(0, 1);
		glEnd();

		glColor3f(0, 0, 1);
		glBegin(GL_TRIANGLES);
			glVertex2f(1, 1);
			glVertex2f(1, 0);
			glVertex2f(0, 1);
		glEnd();
		*/

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}