#ifndef GameObject_H_INCLUDED
#define GameObject_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "AnimatedSprite.hpp"
#include "TextUtils.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Globals.h"
#include "SFText.h"

using namespace std;
using namespace sf;

class Equipment;
class Weapon;
class Loot;

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
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size);
	GameObject();
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels);

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
	static float GetVectorLength(sf::Vector2f vector);
	float GetAbsoluteSpeedSquared();
	static float GetVectorLengthSquared(sf::Vector2f vector);
	static sf::Vector2f GetVectorFromLengthAndAngle(float length, float angle);
	void SetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle);
	sf::Vector2f SetSpeedForConstantSpeedToDestination(sf::Vector2f coordinates, float speed);
	bool BounceOnBorders(sf::Vector2f area_size);

	static bool NormalizeVector(sf::Vector2f* vector, float max_value);
	static void ScaleVector(sf::Vector2f* vector, float target_value);
	static void AddValueToVector(sf::Vector2f* vector, float added_value);
	static float GetAngleRadForVector(sf::Vector2f vector);
	static void GetAngleRadAndLengthOfVector(sf::Vector2f vector, float* output_length, float* output_angle);
	static float GetAngleRadToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position);
	static float GetAngleDegToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position);
	static float GetDistanceBetweenObjects(GameObject* object1, GameObject* object2);
	static float GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
	static float GetDistanceSquaredBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
	static float GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2);
	static float GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2);
	sf::Vector2f RotateVector(sf::Vector2f vector, float angle_rad);
	static void SetVectorRotation(sf::Vector2f* vector, float angle_rad);
	static bool IntersectSegments(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x = NULL, float *i_y = NULL);
	static float DistancePointToSement(float p0_x, float p0_y, float p_1x, float p_1y, float p_2x, float p_2y, float *i_x = NULL, float *i_y = NULL);

	static sf::Uint8* CreateRectangleWithStroke(sf::Vector2f size, sf::Color color, int stroke_size = 0);

	static int GetPixelDistanceFromEdge(int pixel_index, int width, int height);
	static void GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size = 0);
	static int GaussianBlurDistribution(int x);

	Animation m_defaultAnimation;
	Animation* m_currentAnimation;
	sf::Vector2f m_previous_speed;
};

#endif // GameObject_H_INCLUDED