#ifndef MARKER_H_INCLUDED
#define MARKER_H_INCLUDED

#include "SpatialObject.h"

enum MarkerType
{
	Marker_Enemy,
	Marker_Ally,
	Marker_Mission,
	NB_MARKER_TYPES,
};

class Marker : public GameObject
{
public:
	Marker(MarkerType marker_type, SpatialObject* target);
	~Marker();
	
	void Update(sf::Time deltaTime);
	void Draw(RenderTarget& screen) override;
	void SetMarkerType(MarkerType marker_type);

	MarkerType m_marker_type;
	SpatialObject* m_target;
	sf::Text m_distance_text;
	sf::RectangleShape m_targeting_rect[8];
	sf::RectangleShape m_mission_rect;
	bool m_onScreen;
	bool m_isMission;
};

#endif // MARKER_H_INCLUDED
