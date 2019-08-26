#ifndef MARKER_H_INCLUDED
#define MARKER_H_INCLUDED

#include "Game.h"

class Ship;

enum MarkerType
{
	Marker_Enemy,
	Marker_Ally,
	NB_MARKER_TYPES,
};

class Marker : public GameObject
{
public:
	Marker(MarkerType marker_type, GameObject* target);
	~Marker();
	
	void Update(sf::Time deltaTime);
	void Draw(RenderTarget& screen) override;
	void SetMarkerType(MarkerType marker_type);

	MarkerType m_marker_type;
	GameObject* m_target;
	sf::Text m_distance_text;
	//sf::RectangleShape m_targeting_rect[8];
	sf::RectangleShape m_targeting_rect;
};

#endif // MARKER_H_INCLUDED
