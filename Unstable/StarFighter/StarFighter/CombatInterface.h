#ifndef COMBATINTERFACE_H_INCLUDED
#define COMBATINTERFACE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "Ship.h"

struct CombatInterface
{
	CombatInterface();
	~CombatInterface();

	void Init(Ship* enemy_ship);
	Ship* m_enemy_ship;
	GameEntity* m_enemy_ui;//warp border + enemy ship info (name, class, alliance, volume, crew size)
	GameEntity* m_lifebars[2];
	GameEntity* m_floodbars[2];
	GameEntity* m_crewbars[2];
	GameEntity* m_distance_line;
	GameEntity* m_distance_ships[2];

	void Update(sf::Time deltaTime);
	void Draw(sf::RenderTexture& screen);
};

#endif //COMBATINTERFACE_H_INCLUDED