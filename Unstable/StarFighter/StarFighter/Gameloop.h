#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "FileLoadUtils.h"

#include "Warship.h"

enum Scale
{
	Scale_Tactical,
	Scale_Strategic,
};

class Gameloop
{
public:
	Gameloop();
	~Gameloop();
	
	void Update(sf::Time deltaTime);
	void UpdateUI(sf::Time deltaTime);
	void Draw();

	//PIRATES
	GameEntity* m_background;
	Warship* m_warship;
	Island* m_island;
	vector<Ship*> m_ships;
	Ship* m_tactical_ship;
	Scale m_scale;
	sf::Text m_pause_text;

	void InitWaterZones();
	bool UpdateTacticalScale();
	void UpdateRoomTileFeedback(RoomTile* tile, sf::Time deltaTime);

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED