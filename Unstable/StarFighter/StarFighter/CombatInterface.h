#ifndef COMBATINTERFACE_H_INCLUDED
#define COMBATINTERFACE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "Warship.h"

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
public:
	CombatInterface();
	~CombatInterface();

	void Init(Ship* ship);
	Ship* GetShip() const { return m_ship; }
	void Destroy();

	GameEntity* m_lifebar;
	GameEntity* m_floodbar;
	GameEntity* m_crewbar;
	GameEntity* m_sonar;
	GameEntity* m_combatStrength;
	//GameEntity* m_distance_line;
	//GameEntity* m_distance_markers[5];
	//GameEntity* m_distance_ships[2];
	GameEntity* m_ship_name;
	GameEntity* m_ship_info;

	void Update();
	void Draw(sf::RenderTexture& screen);

private:
	Ship* m_ship;
};

#endif //COMBATINTERFACE_H_INCLUDED