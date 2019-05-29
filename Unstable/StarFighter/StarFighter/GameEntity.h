#ifndef GAMEENTITY_H_INCLUDED
#define GAMEENTITY_H_INCLUDED

#include "Game.h"

enum UI_Type
{
	UI_None,
	UI_Commodity,
	UI_Compass,
	UI_Connexion,
	UI_CrewMember,
	UI_CrewMemberUnboarding,
	UI_EnemyShip,
	UI_Engine,
	UI_Island,
	UI_Location,
	UI_Resource,
	UI_ResourceUpkeep,
	UI_Room,
	UI_RoomTile,
	UI_Rudder,
	UI_ShopItem,
	UI_Sonar,
	UI_Upgrade,
	UI_Warship,
	UI_WaterTile,
	UI_Weapon,
};

enum ShipAlliance
{
	Alliance_Player,
	Alliance_Enemy,
	Alliance_Ally,
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
	sf::Vector2f m_ship_offset;

	void Update(Time deltaTime);
	void UpdatePosition();
	void GetHoveredState();
	void Draw(sf::RenderTexture& screen);

	bool IsHoveredByMouse();
	bool ButtonUpdate();

	Animation m_defaultAnimation;
	Animation* m_currentAnimation;
	int m_animationNumber;
	int m_frameNumber;
	int m_currentAnimationIndex;

	void setAnimation(sf::Texture *texture, int frameNumber, int animationNumber);
	void setAnimationLine(int animation, bool keep_frame_index = false);

	static bool IsColliding(GameEntity* entityA, GameEntity* entityB);
};

#endif //GAMEENTITY_H_INCLUDED