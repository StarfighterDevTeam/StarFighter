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

class SpatialObject;
class AIShip;

enum LayerType
{
	BackgroundLayer,
	SectorLayer,
	StarLayer,	
	Planet_Layer,
	PlayerFireLayer,
	AIShipFireLayer,
	PlayerStroboscopicLayer,
	AIShipLayer,
	PlayerShipLayer,
	MarkerLayer,

	FeedbacksLayer,
	NBVAL_Layer
};

enum ColliderType
{
	BackgroundObject,
	PlanetObject,
	PlayerShipObject,
	PlayerFire,
	EnemyFire,
	EnemyShipObject,
	NBVAL_ColliderType
};

class GameObject : public AnimatedSprite
{
public:
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, int stroke_size = 0);
	GameObject();
	
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, int stroke_size = 0);

	static sf::Uint8* CreateRectangleWithStroke(sf::Vector2f size, sf::Color color, int stroke_size = 0);
	static void GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size = 0);

	~GameObject();

	virtual void Update(sf::Time deltaTime);
	virtual void Draw(RenderTarget& screen);
	virtual void UpdateAnimation(sf::Time deltaTime);
	void SetAnimationLine(int animation, bool keep_frame_index = false);
	static int GetPixelDistanceFromEdge(int pixel_index, int width, int height);
	static int GaussianBlurDistribution(int x);
	void SetSpeedVectorFromAbsoluteSpeedAndAngle(const float absolute_speed, const float curAngle);
	static sf::Vector2f GetSpeedVectorFromAbsoluteSpeedAndAngle(const float absolute_speed, const float curAngle);

	string GetTextureName();
	GameObject* Clone();

	bool m_visible;
	bool m_garbageMe;//object to be deleted
	bool m_removeMe;//object to be removed from m_sceneGameObjects
	ColliderType m_collider;
	LayerType m_layer;
	sf::Vector2f m_size;
	sf::Vector2f m_position;
	float m_radius;
	float m_rotation_speed;
	int m_animationNumber;
	int m_frameNumber;
	int m_currentAnimationIndex;
	std::string m_textureName;
	sf::Vector2f m_speed;
	float m_heading;

	Animation m_defaultAnimation;
	Animation* m_currentAnimation;
	sf::Vector2f m_previous_speed;

	//Star Hunter
	void UpdateStarSectorIndex();
	virtual void MarkThis(SpatialObject* target);
	virtual SpatialObject* GetTargetableEnemyShip(const GameObject* ref_object, const float dist_max, const float angle_delta_max);
	virtual bool GetHitByAmmo(GameObject* ammo);
	virtual void SetPosition(sf::Vector2f position);

	sf::Vector2i m_star_sector_index;
};

#endif // GameObject_H_INCLUDED