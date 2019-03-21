#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "FileLoadUtils.h"

#include "Warship.h"
#include "ContextualOrder.h"
#include "ResourcesInterface.h"

enum Scale
{
	Scale_Tactical,
	Scale_Strategic,
};

enum ContextualMenu
{
	Menu_None,
	Menu_CrewOverboard,
	Menu_PrisonersChoice,
	Menu_CrewUnboard,
	Menu_Reward,
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
	vector<Island*> m_islands;
	vector<Ship*> m_ships;
	Ship* m_tactical_ship;
	Scale m_scale;
	sf::Text m_pause_text;
	ContextualMenu m_menu;

	void InitWaterZones();
	bool UpdateTacticalScale();
	bool UpdateUnboarding();
	void UpdateRoomTileFeedback(RoomTile* tile, sf::Time deltaTime, Ship* ship);

	ContextualOrder* m_contextual_order;

	struct ResourcesInterface m_resources_interface;

	Ship* IsDMSInCombatRange(DMS_Coord DMS_a, DMS_Coord DMS_b);

	int SavePlayerData(Warship* warship);
	int LoadPlayerData(Warship* warship);
	bool GetSaveOrLoadInputs();

	void UpdateContextualOrderFeedback(CrewMember* crew, RoomTile* tile);
	void UpdateTargetRoom(Weapon* weapon, Room* room_hovered);

	void UpdateAICrew(CrewMember* crew);
	void UpdateAITargetRoom(Weapon* weapon);
	bool AddResource(Resource_Meta resource, int value);

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED