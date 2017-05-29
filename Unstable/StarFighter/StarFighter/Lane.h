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
	LaneData_Counter,
};

class LaneData
{
public:
	float m_width;
	float m_angle;
	size_t m_counter;
};


class Lane : public GameObject
{
public :
	Lane(GameObject* spawner, string csv_file);
	void update(sf::Time deltaTime) override;

	GameObject* m_spawner;
	float m_lane_angle;//in degrees
	float m_lane_width;
	float m_position_delta;
	float m_angle_delta;
	float m_width_delta;

	void RotateLane(float deg_angle);
	void ScaleLane(float width);
	bool CreateNextLanePeriod();
	vector<LaneData> m_lane_data;
	string m_csv_file;
	sf::Clock m_period_clock;
	sf::Clock m_change_clock;
	size_t m_period_counter;
};

#endif // LANE_H_INCLUDED
