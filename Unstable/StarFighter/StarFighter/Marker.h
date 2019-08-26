#ifndef MARKER_H_INCLUDED
#define MARKER_H_INCLUDED

#include "Game.h"

class Ship;

enum MarkerType
{
	Marker_Enemy,
	NB_MARKER_TYPES,
};

class Marker : public GameObject
{
public:
	Marker(MarkerType marker_type, GameObject* target);
	~Marker();
	
	void Update(sf::Time deltaTime);
	void Draw(RenderTarget& screen) override;

	MarkerType m_marker_type;
	GameObject* m_target;
	sf::Text m_distance_text;
};

#endif // MARKER_H_INCLUDED
