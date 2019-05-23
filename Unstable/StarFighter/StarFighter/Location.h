#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Upgrade.h"

#define RESOURCES_REFRESH_RATE_IN_DAYS			40
#define NB_CHOICES_MAX							4
#define SEAMONSTER_DEPTH_MIN						30
#define SEAMONSTER_DEPTH_MAX						200
#define WRECK_DEPTH_MIN							30
#define WRECK_DEPTH_MAX							200
#define FISH_DEPTH_MIN							5
#define FISH_DEPTH_MAX							100
#define SEAMONSTER_LOSE_PROBABILITY				0.2
#define SEAMONTER_CREW_KILLS_MAX					4

//forward declaration
class WaterTile;

enum LocationType
{
	Location_Wreck,
	Location_SeaMonster,
	Location_Fish,
	NB_SECRET_LOCATION_TYPES,
	Location_None,
	Location_Seaport,
};

struct DMS_Coord
{
	DMS_Coord(){};
	DMS_Coord(int degree_x, int minute_x, float second_x, int degree_y, int minute_y, float second_y)
	{
		SetCoord(degree_x, minute_x, second_x, degree_y, minute_y, second_y);
	}

	int m_degree_x;
	int m_minute_x;
	float m_second_x;

	int m_degree_y;
	int m_minute_y;
	float m_second_y;

	void SetCoord(int degree_x, int minute_x, float second_x, int degree_y, int minute_y, float second_y){
		m_degree_x = degree_x;
		m_minute_x = minute_x;
		m_second_x = second_x;
		m_degree_y = degree_y;
		m_minute_y = minute_y;
		m_second_y = second_y;
	}
};

class Location : public GameEntity
{
public:
	Location(LocationType type, WaterTile* tile);
	~Location();

	WaterTile* m_tile;
	string m_display_name;
	int m_visited_countdown;
	LocationType m_type;
	string m_choicesID[NB_CHOICES_MAX];
	int m_depth;
	bool m_known;
	vector<UpgradeType> m_upgrades;
};

#endif //LOCATION_H_INCLUDED