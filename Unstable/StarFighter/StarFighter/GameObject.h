#ifndef GAMEOBJECT_H_INCLUDED
#define GAMEOBJECT_H_INCLUDED

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

class Discoball;

enum LayerType {
	BackgroundLayer,
	GoalLayer,
	ExplosionLayer,
	EnemyObjectLayer,
	AuraLayer,
	FeedbacksLayer,
	PlayerShipLayer,
	DiscoballLayer,
	NBVAL_Layer
};

enum GameObjectType {
	BackgroundObject,
	BumperObject,
	DiscoballObject,
	GoalBlueObject,
	GoalRedObject,
	PlayerShip,
	EnemyObject,
	NBVAL_GameObject
};

enum FloatCompare
{
	LESSER_THAN,
	EQUAL_TO,
	GREATHER_THAN,
	ERROR_COMPARE,
};

class GameObject : public AnimatedSprite
{
public:
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber = 1, int m_animationNumber = 1);
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	GameObject();

	~GameObject();

	virtual void update(sf::Time deltaTime);
	virtual void updateAnimation(sf::Time deltaTime);
	void setAnimationLine(int m_animation, bool keep_frame_index = false);
	bool visible;
	bool isOnScene;
	bool GarbageMe;
	bool DontGarbageMe;
	GameObjectType collider_type;
	LayerType layer;
	sf::Vector2f m_size;

	string getName();
	virtual void Respawn();
	GameObject* Clone();

	float diag;
	bool ghost;
	void setGhost(bool m_ghost);
	float rotation_speed;
	int animationNumber;
	int frameNumber;
	int currentAnimationIndex;

	std::string textureName;

	float GetAbsoluteSpeed();
	void SetSpeedVectorFromAbsoluteSpeed(float absolute_speed, float curAngle);
	float SpeedToPolarAngle(sf::Vector2f curSpeed);

	//TRON SPECIFIC
	virtual void GetDiscoball(GameObject* discoball, float angle_collision = -1.f);
	virtual void PlayerContact(GameObject* player, float angle_collision = -1.f);
	void PlayHitFeedback();
	sf::Clock feedback_reset_clock;

	sf::Vector2f speed;


protected:
	sf::Vector2f initial_position;
	Animation defaultAnimation;
	Animation* currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int m_frameNumber, int m_animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber = 1, int m_animationNumber = 1);
};

#endif // GAMEOBJECT_H_INCLUDED