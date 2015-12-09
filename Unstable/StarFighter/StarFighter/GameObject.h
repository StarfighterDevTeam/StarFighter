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
	OfflineMulti2vs2,
	Tuto01,
	Tuto02,
	Tuto03,
	Tuto04,
	Tuto05,
	ScriptTest,
};

enum CollisionSide
{
	Collision_Top,
	Collision_Right,
	Collision_Bottom,
	Collision_Left,
	Collision_TopLeft,
	Collision_TopRight,
	Collision_BottomRight,
	Collision_BottomLeft,
	NoCollision,
};

enum GlowAnimations
{
	GlowDefaultAnimation,
	GlowHitAnimation
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
	GameObjectType m_collider_type;
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

	std::string m_textureName;

	//Utilitary methodes
	float GetAbsoluteSpeed();
	static float GetAbsoluteSpeed(sf::Vector2f speed_);
	float GetAbsoluteSpeedSquared();
	static float GetAbsoluteSpeedSquared(sf::Vector2f speed_);

	void SetSpeedVectorFromAbsoluteSpeed(float absolute_speed, float curAngle);

	static bool NormalizeSpeed(sf::Vector2f* vector, float max_value);
	static void ScaleSpeed(sf::Vector2f* vector, float target_value);
	static void AddSpeed(sf::Vector2f* vector, float added_value);
	static float GetAngleRadForSpeed(sf::Vector2f curSpeed);
	static float GetDistanceBetweenObjects(GameObject* object1, GameObject* object2);
	static float GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2);
	static float GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2);
	static bool isCapsuleColliding(GameObject* object, GameObject* bumper, sf::Time deltaTime);
	static bool isCapsuleCollidingDuringMovement(GameObject* object, float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y);
	static bool IntersectSegments(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x = NULL, float *i_y = NULL);
	static float DistancePointToSement(float p0_x, float p0_y, float p_1x, float p_1y, float p_2x, float p_2y, float *i_x = NULL, float *i_y = NULL);

	//TRON SPECIFIC
	virtual void GetDiscoball(GameObject* discoball, float angle_collision = -1.f);
	virtual void GetPortal(GameObject* portal);
	virtual void CollisionResponse(Time deltaTime);
	virtual void CollisionResponse(GameObject* bumper, CollisionSide collision);
	virtual void CollisionResponse(GameObject* bumper, CollisionSide collision, bool bouncing);
	virtual void PlayerBumper(GameObject* bumper, Time deltaTime);
	virtual void PlayerContact(GameObject* player, float angle_collision = -1.f);
	virtual void CheckIfPlayerDiscoballBumped(Time deltaTime);
	virtual void UsingPortal(bool is_using);
	void PlayHitFeedback();
	sf::Clock feedback_reset_clock;

	static int GetPixelDistanceFromEdge(int pixel_index, int width, int height);
	void GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size = 0);
	static int GaussianBlurDistribution(int x);

	sf::Vector2f speed;

protected:
	sf::Vector2f initial_position;
	Animation defaultAnimation;
	Animation* currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels);
};

#endif // GAMEOBJECT_H_INCLUDED