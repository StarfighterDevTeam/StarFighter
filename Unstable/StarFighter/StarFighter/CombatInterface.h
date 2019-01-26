#ifndef COMBATINTERFACE_H_INCLUDED
#define COMBATINTERFACE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "Ship.h"

#define COMBAT_LIFEBAR_SIZE_X			400.f
#define COMBAT_LIFEBAR_SIZE_Y			20.f
#define COMBAT_LIFEBAR_OFFSET_X			200.f
#define COMBAT_LIFEBAR_MINI_OFFSET_X		1350.f
#define COMBAT_LIFEBAR_OFFSET_Y			100.f
#define COMBAT_FLOODBAR_SIZE_X			(COMBAT_LIFEBAR_SIZE_X)
#define COMBAT_FLOODBAR_SIZE_Y			10.f
#define COMBAT_DISTANCE_SIZE_X			400.f
#define COMBAT_DISTANCE_OFFSET_X			1010.f
#define COMBAT_DISTANCE_OFFSET_Y			1040.f
#define COMBAT_WARPZONE_SIZE_Y			960.f

struct CombatInterface
{
	CombatInterface();
	~CombatInterface();

	void Init(Ship* ship, Ship* enemy_ship);
	Ship* m_ship;
	Ship* m_enemy_ship;
	GameEntity* m_warp_zone;//warp border + enemy ship info (name, class, alliance, volume, crew size)
	GameEntity* m_lifebars[2];
	GameEntity* m_floodbars[2];
	GameEntity* m_crewbars[2];
	GameEntity* m_distance_line;
	GameEntity* m_distance_markers[5];
	GameEntity* m_distance_ships[2];

	void Update(sf::Time deltaTime);
	void Draw(sf::RenderTexture& screen);
};

#endif //COMBATINTERFACE_H_INCLUDED