#ifndef GAMEENTITY_H_INCLUDED
#define GAMEENTITY_H_INCLUDED

#include "Game.h"

class GameEntity
{
public:
	GameEntity();
	GameEntity(sf::Vector2f size);

	sf::Vector2f m_size;
	sf::Vector2f m_position;

	bool m_selected;
	bool m_hovered;

	RectangleShape m_shape_container;
	RectangleShape m_shape;
	SFText m_text;

	void Update(Time deltaTime);
	void Draw(sf::RenderTexture& screen);
};

#endif //GAMEENTITY_H_INCLUDED