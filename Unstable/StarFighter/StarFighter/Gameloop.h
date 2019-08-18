#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "FileLoadUtils.h"
#include "Ship.h"
#include "Node.h"
#include "Terminal.h"
#include "Link.h"

class Gameloop
{
public:
	Gameloop();
	~Gameloop();
	
	void Update(sf::Time deltaTime);
	void Draw();
	void UpdateCamera(sf::Time deltaTime);

	GameObject* m_background;

	//Liaison16
	Node* CreateNode(sf::Vector2f position, AllianceType alliance);
	Terminal* CreateTerminal(sf::Vector2f position, AllianceType alliance);
	Link* CreateLink(Node* node_a, Node* node_b);
	Wing* CreateWing(sf::Vector2f position, AllianceType alliance, float heading);
	L16Entity* GetRectangularSelection();

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED