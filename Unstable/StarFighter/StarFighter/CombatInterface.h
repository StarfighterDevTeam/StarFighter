#ifndef COMBATINTERFACE_H_INCLUDED
#define COMBATINTERFACE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"

class Ship;//forward declaration

#define COMBAT_LIFEBAR_SIZE_X			400.f
#define COMBAT_LIFEBAR_SIZE_Y			20.f
#define COMBAT_LIFEBAR_OFFSET_X			ROOMTILE_OFFSET_X //200.f
#define COMBAT_LIFEBAR_ENEMY_OFFSET_X	ROOMTILE_ENEMY_OFFSET_X //1350.f
#define COMBAT_INTERFACE_OFFSET_Y		80
#define COMBAT_FLOODBAR_SIZE_X			COMBAT_LIFEBAR_SIZE_X
#define COMBAT_FLOODBAR_SIZE_Y			10.f
#define COMBAT_DISTANCE_SIZE_X			400.f
#define COMBAT_DISTANCE_OFFSET_X		1010.f
#define COMBAT_DISTANCE_OFFSET_Y		1040.f
#define COMBAT_WARPZONE_SIZE_Y			960.f

struct CombatInterface
{
	CombatInterface();
	~CombatInterface();

	void Init(Ship* ship, ShipAlliance alliance, string ship_name, ShipType ship_type);
	void Destroy();
	Ship* m_ship;
	GameEntity* m_lifebar;
	GameEntity* m_floodbar;
	GameEntity* m_crewbar;
	//GameEntity* m_distance_line;
	//GameEntity* m_distance_markers[5];
	//GameEntity* m_distance_ships[2];
	GameEntity* m_ship_name;
	GameEntity* m_ship_info;

	void Update(int health, int health_max, int flood, int flood_max, int nb_crew, int nb_crew_max);
	void Draw(sf::RenderTexture& screen);
};

#endif //COMBATINTERFACE_H_INCLUDED