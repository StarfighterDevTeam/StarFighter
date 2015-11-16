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
	PortalLayer,
	GoalLayer,
	BumperLayer,
	EnemyObjectLayer,
	AuraLayer,
	ScoresLayer,
	PlayerStroboscopicLayer,
	PlayerShipLayer,
	DiscoballLayer,
	NBVAL_Layer
};

enum GameObjectType {
	BackgroundObject,
	BumperBlueObject,
	BumperRedObject,
	BumperGreenObject,
	DiscoballObject,
	GoalBlueObject,
	GoalRedObject,
	GoalGreenObject,
	PlayerBlueShip,
	PlayerRedShip,
	EnemyObject,
	LevelPortalObject,
	NBVAL_GameObject
};

enum FloatCompare
{
	LESSER_THAN,
	EQUAL_TO,
	GREATHER_THAN,
	ERROR_COMPARE,
};

enum Teams
{
	BlueTeam,
	RedTeam,
	TeamNeutral,
};

enum IngameScript
{
	MainMenuScript,
	OfflineMulti,
	OfflineMultiBig,
	Tuto01,
	Tuto02,
	Tuto03,
	Tuto04,
	Tuto05,
	ScriptTest,
};

class GameObject : public AnimatedSprite
{
public:
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	GameObject();

	~GameObject();

	virtual void update(sf::Time deltaTime);
	virtual void updateAnimation(sf::Time deltaTime);
	void setAnimationLine(int animation, bool keep_frame_index = false);
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
	int m_animationNumber;
	int m_frameNumber;
	int currentAnimationIndex;

	std::string textureName;

	//Utilitary methodes
	float GetAbsoluteSpeed();
	void SetSpeedVectorFromAbsoluteSpeed(float absolute_speed, float curAngle);

	static bool NormalizeSpeed(sf::Vector2f* vector, float max_value);
	static float GetAbsoluteSpeed(sf::Vector2f speed_);
	static float GetAngleRadForSpeed(sf::Vector2f curSpeed);
	static float GetDistanceBetweenObjects(GameObject* object1, GameObject* object2);
	static float GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2);
	static float GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2);
	static bool isCapsuleColliding(GameObject* object, GameObject* bumper, sf::Time deltaTime);
	static bool IntersectSegments(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y);

	//TRON SPECIFIC
	virtual void GetDiscoball(GameObject* discoball, float angle_collision = -1.f);
	virtual void GetPortal(GameObject* portal);
	virtual void DiscoballBumper(GameObject* bumper, sf::Time deltaTime);
	virtual void PlayerBumper(GameObject* bumper, Time deltaTime);
	virtual void PlayerContact(GameObject* player, float angle_collision = -1.f);
	virtual void CheckIfPlayerDiscoballBumped(Time deltaTime);
	virtual void UsingPortal(bool is_using);
	void PlayHitFeedback();
	sf::Clock feedback_reset_clock;

	sf::Vector2f speed;

protected:
	sf::Vector2f initial_position;
	Animation defaultAnimation;
	Animation* currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int m_frameNumber = 1, int m_animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber = 1, int m_animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber, sf::Uint8* pixels);
};

#endif // GAMEOBJECT_H_INCLUDED