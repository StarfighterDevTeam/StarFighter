#ifndef COMPASS_H_INCLUDED
#define COMPASS_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"

#define COMPASS_OFFSET_X				240.f
#define COMPASS_OFFSET_Y				1030.f
#define COMPASS_CADRAN_SIZE_X			400.f
#define COMPASS_CADRAN_SIZE_Y			50.f
#define COMPASS_CURSOR_SIZE_X			45.f
#define COMPASS_CURSOR_SIZE_Y			90.f
#define COMPASS_CURSOR_TIP_SIZE_X		8.f
#define COMPASS_CURSOR_TIP_SIZE_Y		20.f
#define COMPASS_MARKER_SIZE_X			4.f
#define COMPASS_MARKER_SIZE_Y			8.f
#define COMPASS_DEGREES_IN_CADRAN		180
#define COMPASS_DEGREES_PER_MARKER		5

struct Compass
{
	Compass();
	~Compass();
	GameEntity* m_cadran;
	GameEntity* m_cursor;
	GameEntity* m_cardinal_pts[8];
	GameEntity* m_degree_markers[72];
	float m_marker_offset;
	sf::Vector2f m_marker_pos0;

	void Update(sf::Time deltaTime, float angle, float desired_angle);
	void Draw(sf::RenderTexture& screen, float angle);
	bool GetInput(float angle, float& desired_angle);
};

#endif //COMPASS_H_INCLUDED