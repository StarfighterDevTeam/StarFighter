#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "FileLoadUtils.h"
#include "StarGenerator.h"
#include "Player.h"

class Gameloop
{
public:
	Gameloop();
	~Gameloop();
	
	void Update(sf::Time deltaTime);
	void Draw();
	void UpdateCamera(sf::Time deltaTime);

	void PopulateSector(sf::Vector2i sector_index);
	AIShip* CreateAIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, HostilityLevel hostility);
	Planet* CreatePlanet(sf::Vector2i sector_index, HostilityLevel hostility, int nb_missions_min, int nb_missions_max);
	Mission* CreateMission(Planet* owner);
	Planet* GetPlanetForMission(sf::Vector2i sector_index);

	vector<StarGenerator*> m_star_generator;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED