#ifndef INDEPENDANT_H_INCLUDED
#define INDEPENDANT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "AnimatedSprite.hpp"
#include "TextUtils.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Globals.h"

using namespace std;
using namespace sf;

class Equipment;
class Weapon;
class Loot;

enum LayerType {
	BackgroundLayer,
	PortalLayer,
	ExplosionLayer,
	EnemyObjectLayer,
	AuraLayer,
	FeedbacksLayer,
	BotLayer,
	FriendlyFireLayer,
	FakeShipLayer,
	LootLayer,
	PlayerShipLayer,
	EnemyFireLayer,

	PanelLayer,
	HudObject,
	HudCursor,
	NBVAL_Layer
};


enum TargetSeaking
{
	NO_SEAKING,
	SEMI_SEAKING,//aim once every rafale
	SEAKING,//aim once every bullt
	SUPER_SEAKING,//bullet follows target
};

enum ShotMode
{
	NoShotMode,//0
	AlternateShotMode,//1
	AscendingShotMode,//2
	DescendingShotMode,//3
	Ascending2ShotMode,//4 - back and forth
	Descending2ShotMode,//5 - back and forth
};


enum IndependantType {
	BackgroundObject,
	PortalObject,
	ShopObject,
	LootObject,
	PlayerShip,
	FakePlayerShip,
	FriendlyFire,
	Neutral,
	EnemyFire,
	EnemyObject,
	NBVAL_Independant
};

enum InteractionType
{
	No_Interaction,
	PortalInteraction,
	ShopInteraction,
};

enum Directions
{
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_RIGHT,
	DIRECTION_LEFT,
	NO_DIRECTION,
};

enum FloatCompare
{
	LESSER_THAN,
	EQUAL_TO,
	GREATHER_THAN,
	ERROR_COMPARE,
};

class Independant : public AnimatedSprite
{
public:
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber = 1, int m_animationNumber = 1);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	Independant();

	~Independant();

	virtual void update(sf::Time deltaTime);
	virtual void updateAnimation(sf::Time deltaTime);
	void setAnimationLine(int m_animation, bool keep_frame_index = false);
	bool visible;
	bool isOnScene;
	bool GarbageMe;
	bool DontGarbageMe;
	IndependantType collider_type;
	LayerType layer;
	sf::Vector2f m_size;
	sf::Vector2f getIndependantSpeed();
	string getName();
	virtual void Respawn();
	Independant* Clone();

	
	float diag;
	bool ghost;
	void setGhost(bool m_ghost);
	float rotation_speed;
	int animationNumber;
	int frameNumber;
	int currentAnimationIndex;

	std::string textureName;
protected:
	sf::Vector2f initial_position;
	sf::Vector2f speed;
	Animation defaultAnimation;
	Animation* currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int m_frameNumber, int m_animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber = 1, int m_animationNumber = 1);
	
};

#endif // INDEPENDANT_H_INCLUDED