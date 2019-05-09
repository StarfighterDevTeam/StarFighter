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
	vector<Seaport*> m_seaports;
	vector<Ship*> m_ships;
	vector<Location*> m_secret_locations[NB_SECRET_LOCATION_TYPES];
	Ship* m_tactical_ship;
	Scale m_scale;
	sf::Text m_pause_text;
	ContextualMenu m_menu;

	void InitWaterZones();
	bool UpdateTacticalScale();
	void UpdateRoomTileFeedback(RoomTile* tile, sf::Time deltaTime, Ship* ship);

	ContextualOrder* m_contextual_order;

	struct ResourcesInterface m_resources_interface;

	Ship* IsDMSInCombatRange(DMS_Coord DMS_hovered, bool ally_included);
	void StartCombatWithShip(Ship* enemy_ship);

	int SavePlayerData(Warship* warship);
	int LoadPlayerData(Warship* warship);
	bool GetSaveOrLoadInputs();

	void UpdateContextualOrderFeedback(CrewMember* crew, RoomTile* tile);
	void UpdateTargetRoom(Weapon* weapon, Room* room_hovered);

	void UpdateAICrew(CrewMember* crew);
	void UpdateAITargetRoom(Weapon* weapon);

	void GenerateRandomIslands(int zone_coord_x, int zone_coord_y);
	void GenerateRandomSecretLocations(int zone_coord_x, int zone_coord_y);
	void GenerateRandomShips(int zone_coord_x, int zone_coord_y);
	void SpendDays(int days, bool skip_time);
	bool CanIslandBeCreatedInArea(int upcorner_x, int upcorner_y, int width, int height, int zone_coord_x, int zone_coord_y);

	bool AddCommodityToHolds(CommodityType commodity_type);
	void RemoveCommodity(CommodityType commodity_type);

	Reward* GenerateReward(int rewardID, Location* location, Ship* other_ship, int gauge, int gauge_max);

	//IA
	void SetAIStrategicalDestination(Ship* ship, DMS_Coord warship_DMS);

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED