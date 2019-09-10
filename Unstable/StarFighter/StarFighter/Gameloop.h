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
	AIShip* CreateAIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe);
	Planet* CreatePlanet(sf::Vector2i sector_index, Hostility hostility, int nb_missions_to_create_min, int nb_missions_to_create_max);
	Mission* CreateMission(Planet* owner);
	Planet* GetPlanetAtSectorId(int id);

	vector<StarGenerator*> m_star_generator;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED