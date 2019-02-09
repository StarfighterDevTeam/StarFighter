#ifndef GAMEENTITY_H_INCLUDED
#define GAMEENTITY_H_INCLUDED

#include "Game.h"

enum UI_Type
{
	UI_None,
	UI_CrewMember,
	UI_Room,
	UI_RoomTile,
	UI_Connexion,
	UI_EnemyShip,
	UI_Warship,
	UI_WaterTile,
	UI_Island,
	UI_Seaport,
	UI_Compass,
	UI_Weapon,
	UI_System,
};

enum ShipAlliance
{
	Alliance_Player,
	Alliance_Enemy,
};

class GameEntity : public AnimatedSprite
{
public:
	GameEntity(UI_Type ui_type);
	GameEntity(sf::Vector2f size, UI_Type ui_type);

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
	void UpdatePosition();
	void Draw(sf::RenderTexture& screen);

	bool IsHoveredByMouse();

	Animation m_defaultAnimation;
	Animation* m_currentAnimation;
	int m_animationNumber;
	int m_frameNumber;
	int m_currentAnimationIndex;

	void setAnimation(sf::Texture *texture, int frameNumber, int animationNumber);
	void setAnimationLine(int animation, bool keep_frame_index = false);
};

#endif //GAMEENTITY_H_INCLUDED