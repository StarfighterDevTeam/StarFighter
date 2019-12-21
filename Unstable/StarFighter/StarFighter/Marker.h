#ifndef MARKER_H_INCLUDED
#define MARKER_H_INCLUDED

#include "SpatialObject.h"

enum MarkerType
{
	Marker_Ally,
	Marker_Enemy,
	Marker_Mission,
	NB_MARKER_TYPES,//by order of draw priority, from lowest to highest
};

class Marker : public GameObject
{
public:
	Marker(MarkerType marker_type, SpatialObject* marker_target);
	~Marker();
	
	void Update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
	void SetMarkerType(MarkerType marker_type);

	MarkerType m_marker_type;
	SpatialObject* m_marker_target;
	sf::Text m_distance_text;
	sf::RectangleShape m_targeting_rect[8];
	sf::RectangleShape m_mission_rect;
	bool m_onScreen;
};

#endif // MARKER_H_INCLUDED
