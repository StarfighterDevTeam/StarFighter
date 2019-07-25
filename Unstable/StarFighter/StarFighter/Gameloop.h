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
	Node* CreateNode(sf::Vector2f position, bool player);
	Terminal* CreateTerminal(sf::Vector2f position, bool player);

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED