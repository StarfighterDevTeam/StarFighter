#ifndef LANE_H_INCLUDED
#define LANE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "FileLoadUtils.h"

enum LaneDataFields
{
	LaneData_Id,
	LaneData_Width,
	LaneData_Angle,
	LaneData_OffsetX,
	LaneData_OffsetY,
	LaneData_Counter,
};

class LaneData
{
public:
	float m_width;
	float m_angle;
	sf::Vector2f m_offset;
	size_t m_counter;
};

class Spawner : public GameObject
{
public:
	Spawner(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, string csv_file);

	string m_csv_file;
	vector<LaneData> m_lane_data;
};

class Lane : public GameObject
{
public :
	Lane(Spawner* spawner);
	void update(sf::Time deltaTime) override;

	Spawner* m_spawner;
	float m_lane_angle;//in degrees
	float m_lane_width;
	sf::Vector2f m_lane_offset;

	float m_center_delta;
	float m_angle_delta;
	float m_width_delta;
	sf::Vector2f m_offset_delta;

	void RotateLane(float deg_angle);
	void ScaleLane(float width);
	bool CreateNextLanePeriod();
	
	sf::Clock m_period_clock;
	sf::Clock m_change_clock;
	size_t m_period_counter;
};

#endif // LANE_H_INCLUDED
