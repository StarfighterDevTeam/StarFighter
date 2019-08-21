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

class Equipment;
class Weapon;
class Loot;

enum LayerType {
	BackgroundLayer,
	StarLayer,	
	PlayerStroboscopicLayer,
	PlayerShipLayer,
	PlayerFireLayer,
	EnemyShipLayer,
	EnemyFireLayer,
	FeedbacksLayer,
	NBVAL_Layer
};

enum ColliderType {
	BackgroundObject,
	PlayerShipObject,
	PlayerFire,
	EnemyFire,
	EnemyShip,
	HUDLayer,
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
	
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size);

	static sf::Uint8* CreateRectangleWithStroke(sf::Vector2f size, sf::Color color, int stroke_size = 0);
	static void GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size = 0);

	~GameObject();

	virtual void Update(sf::Time deltaTime);
	virtual void UpdateAnimation(sf::Time deltaTime);
	void SetAnimationLine(int animation, bool keep_frame_index = false);
	static int GetPixelDistanceFromEdge(int pixel_index, int width, int height);
	static int GaussianBlurDistribution(int x);
	void SetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle);

	string GetTextureName();
	GameObject* Clone();

	bool m_visible;
	bool m_garbageMe;
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

	Animation m_defaultAnimation;
	Animation* m_currentAnimation;
	sf::Vector2f m_previous_speed;

	//Star Hunter
	void SetStarSectorIndex(sf::Vector2i sector_index);
	void UpdateStarSectorIndex();

	sf::Vector2i m_star_sector_index;
};

#endif // GameObject_H_INCLUDED