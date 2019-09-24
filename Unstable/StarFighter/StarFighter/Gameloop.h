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
	EscortShip* CreateEscortShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe, AIShip* escorted_ship, sf::Vector2f escort_offset);
	Planet* CreatePlanet(sf::Vector2i sector_index, Hostility hostility, int nb_missions_to_create_min = 1, int nb_missions_to_create_max = 1);
	Mission* CreateMission(Planet* owner);
	Beacon* CreateBeacon(sf::Vector2i sector_index, SpatialObject* trigger, bool isMissionObjective);
	Planet* GetPlanetAtSectorId(int id);
	Asteroid* CreateAsteroid(sf::Vector2i sector_index, AsteroidType asteroid_type);
	sf::Vector2i CreateAsteroidField(int sector_index_bottom, int sector_index_left, int sector_index_size_x, int sector_index_size_y);
	bool IsSectorNearAnExistingMission(sf::Vector2i sector_index);
	pair<Planet*, sf::Vector2i> SnailSearchSectorForMission(sf::Vector2i starting_index, MissionType mission_type);

	vector<StarGenerator*> m_star_generator;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED