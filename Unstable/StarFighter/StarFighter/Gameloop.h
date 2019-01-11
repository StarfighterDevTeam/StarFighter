#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "FileLoadUtils.h"

#include "Warship.h"

class Gameloop
{
public:
	Gameloop();
	~Gameloop();
	
	void Update(sf::Time deltaTime);
	void UpdateUI(sf::Time deltaTime);
	void Draw();
	void Release();

	//PIRATES
	GameEntity* m_background;
	Warship* m_warship;
	Island* m_island;
	Ship* m_enemy;

	void InitWaterZones();

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED