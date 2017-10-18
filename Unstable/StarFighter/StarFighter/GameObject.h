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
	StellarMapLayer,
	LocationLayer,
	StarshipLayer,
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
	LocationObject,
	ShopObject,
	StarshipObject,
	PlayerShip,
	FakePlayerShip,
	FriendlyFire,
	Neutral,
	EnemyFire,
	EnemyObject,
	NBVAL_GameObject
};

enum StarshipData
{
	StarshipData_Name,
	StarshipData_TextureName,
	StarshipData_OreCostType1,
	StarshipData_OreCostQuantity1,
	StarshipData_OreCostType2,
	StarshipData_OreCostQuantity2,
	StarshipData_Armor,
	StarshipData_FuelTankMax,
	StarshipData_Weight,
	StarshipData_SpeedMax,
	StarshipData_OreStockMax,
	StarshipData_FuelStockMax,
	StarshipData_NbDrills,
	StarshipData_DrillDuration,//Miner specifics
	StarshipData_DrillSuccessRateBonus,
	StarshipData_ExtractionDurationBonus,
	StarshipData_ScoutRange,
};

enum OreData
{
	OreData_Name,
	OreData_TextureName,
	OreData_Weight,
	OreData_Extraction,
	OreData_Price,
	OreData_Propulsion,
	OreData_PropulsionSpeedBonus,
};

enum BuildingData
{
	BuildingData_Name,
	BuildingData_TextureName,
	BuildingData_Slots,
	BuildingData_Stock,
	BuildingData_Fuel,
	BuildingData_CanExtractOre,
	BuildingData_CanExtractFuel,
	BuildingData_ExtractionDurationBonus,
	BuildingData_OreCostType1,
	BuildingData_OreCostQuantity1,
	BuildingData_OreCostType2,
	BuildingData_OreCostQuantity2,
	BuildingData_StarshipProduction1,
	BuildingData_StarshipProduction2,
	BuildingData_StarshipProduction3,
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
	virtual void Draw(sf::RenderTexture& screen);

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

	//StarMiner specific
	static size_t GetLightYearsBetweenObjects(GameObject* object1, GameObject* object2);
	virtual void HoverObject(GameObject* object);
	virtual bool CanSupplyFuel();

	//Utilitary methods
	float GetAbsoluteSpeed();
	static float GetAbsoluteSpeed(sf::Vector2f speed_);
	float GetAbsoluteSpeedSquared();
	static float GetAbsoluteSpeedSquared(sf::Vector2f speed_);
	static sf::Vector2f GetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle);
	void SetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle);
	sf::Vector2f SetSpeedForConstantSpeedToDestination(sf::Vector2f coordinates, float speed);

	static bool NormalizeSpeed(sf::Vector2f* vector, float max_value);
	static void ScaleSpeed(sf::Vector2f* vector, float target_value);
	static void AddSpeed(sf::Vector2f* vector, float added_value);
	static float GetAngleRadForSpeed(sf::Vector2f curSpeed);
	static float GetDistanceBetweenObjects(GameObject* object1, GameObject* object2);
	static float GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
	static float GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2);
	static float GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2);
	static bool IntersectSegments(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x = NULL, float *i_y = NULL);
	static float DistancePointToSement(float p0_x, float p0_y, float p_1x, float p_1y, float p_2x, float p_2y, float *i_x = NULL, float *i_y = NULL);

	static sf::Uint8* CreateRectangleWithStroke(sf::Vector2f size, sf::Color color, int stroke_size = 0);

	static int GetPixelDistanceFromEdge(int pixel_index, int width, int height);
	static void GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size = 0);
	static int GaussianBlurDistribution(int x);

protected:
	sf::Vector2f m_initial_position;
	Animation m_defaultAnimation;
	Animation* m_currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1, int animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels);
	
};

#endif // GameObject_H_INCLUDED