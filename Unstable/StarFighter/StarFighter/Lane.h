#ifndef LANE_H_INCLUDED
#define LANE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "FileLoadUtils.h"
#include "Sardine.h"

enum LaneDataFields
{
	LaneData_Id,
	LaneData_Width,
	LaneData_OffsetX,
	LaneData_Sardine_1_4,
	LaneData_Sardine_2_4,
	LaneData_Sardine_3_4,
	LaneData_Sardine_4_4,
};

class LaneData
{
public:
	float m_width;
	float m_offset;
	size_t m_sardine[4];
	//size_t m_counter;
};

class Lane : public GameObject
{
public :
	Lane(string csv_file, float bpm);

	string m_csv_file;
	sf::Vector2f m_initial_position;
	float m_bpm;

	void update(sf::Time deltaTime) override;

	vector<LaneData> m_lane_data;
	float m_lane_width;
	float m_lane_offset;
	size_t m_sardine[4];
	bool m_first_period;
	//size_t m_period_counter;

	float m_width_delta;
	float m_offset_delta;
	float m_offset_moved;

	void ScaleLane(float width);
	bool CreateNextLanePeriod();
	void CreateSardine(SardineType type);
	
	sf::Clock m_period_clock;
	sf::Clock m_change_clock;
	
};

#endif // LANE_H_INCLUDED
