#ifndef GameObject_H_INCLUDED
#define GameObject_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "AnimatedSprite.hpp"
#include "TextUtils.h"
#include <vector>
#include "Globals.h"
#include "SFText.h"

using namespace std;
using namespace sf;

enum LayerType {
	BackgroundLayer,
	PortalLayer,
	PlayerStroboscopicLayer,
	ExplosionLayer,
	EnemyObjectLayer,
	AuraLayer,
	FeedbacksLayer,
	FriendlyFireLayer,
	PlayerShipLayer,
	EnemyFireLayer,

	PanelLayer,
	HudObject,
	HudCursor,
	NBVAL_Layer
};

enum GameObjectType {
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
	NBVAL_GameObject
};

class GameObject : public AnimatedSprite
{
public:
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, float stroke_size = 0);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, float radius, float stroke_size = 0);
	GameObject();
	
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, float stroke_size = 0);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, float radius, float stroke_size = 0);
	virtual void Init(){};

	~GameObject();

	virtual void update(sf::Time deltaTime);
	virtual void updateAnimation(sf::Time deltaTime);
	void setAnimationLine(int animation, bool keep_frame_index = false);

	bool m_visible;
	bool m_isOnScene;
	bool m_GarbageMe;
	bool m_DontGarbageMe;
	GameObjectType m_collider_type;
	LayerType m_layer;
	sf::Vector2f m_size;

	string getName();
	GameObject* Clone();
	void setGhost(bool ghost);
	void SetConditionalColor(sf::Color color, bool condition);
	
	float m_diag;
	bool m_ghost;
	float m_rotation_speed;
	int m_animationNumber;
	int m_frameNumber;
	int m_currentAnimationIndex;
	std::string m_textureName;
	sf::Vector2f m_speed;

	//Utilitary methods
	float GetAbsoluteSpeed();
	float GetAbsoluteSpeedSquared();

	void SetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle);
	sf::Vector2f SetConstantSpeedToDestination(sf::Vector2f coordinates, float speed);
	float SetRotationToTarget(sf::Vector2f target_position);
	bool BounceOnBorders(sf::Vector2f area_size);
	bool IsGameObjectInsideArea(sf::Vector2f coordinates, sf::Vector2f area_size);
	float GetDistanceBetweenObjects(GameObject* object1, GameObject* object2);
	float GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2);

	Animation m_defaultAnimation;
	Animation* m_currentAnimation;
	sf::Vector2f m_previous_speed;
};

#endif // GameObject_H_INCLUDED