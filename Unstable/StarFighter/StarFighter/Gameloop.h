#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "FileLoadUtils.h"
#include "PlayerShip.h"

class Gameloop
{
public:
	Gameloop();
	~Gameloop();
	
	void Update(const float DTIME);
	void Draw();
	void UpdateCamera(const float DTIME);

	GameObject* m_background = nullptr;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED