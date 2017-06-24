#ifndef SARDINE_H_INCLUDED
#define SARDINE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "FileLoadUtils.h"

enum SardineType
{
	SardineType_Tap,
};

class Sardine : public GameObject
{
public :
	Sardine(SardineType type, GameObject* lane);

	SardineType m_type;
	void update(sf::Time deltaTime) override;

	GameObject* m_lane;
	sf::Vector2f m_position_offset;
};

#endif // SARDINE_H_INCLUDED
