#ifndef GAMEENTITY_H_INCLUDED
#define GAMEENTITY_H_INCLUDED

#include "Game.h"

enum UI_Type
{
	UI_CrewMember,
	UI_Room,
	UI_Connexion,
};

class GameEntity
{
public:
	GameEntity();
	GameEntity(sf::Vector2f size);

	sf::Vector2f m_size;
	sf::Vector2f m_position;

	bool m_selected;
	bool m_hovered;
	UI_Type m_UI_type;

	RectangleShape m_shape_container;
	RectangleShape m_shape;
	SFText m_text;

	sf::Color m_default_color;

	void Update(Time deltaTime);
	void Draw(sf::RenderTexture& screen);
};

#endif //GAMEENTITY_H_INCLUDED