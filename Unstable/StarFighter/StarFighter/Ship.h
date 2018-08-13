#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Enemy.h"
#include "Loot.h"
#include "FX.h"

#define SHIP_START_X                200
#define SHIP_START_Y                540
#define SHIP_ACCELERATION	        2000.0f
#define SHIP_DECCELERATION_COEF		0.f
#define SHIP_MAX_SPEED				400.0f
#define SHIP_MIN_SPEED				50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f


enum PlayerActions
{
	Action_Idle,
	Action_Melee,
	Action_Dash,
	Action_Muting,
	Action_Pausing,
	NBVAL_PlayerActions,
};

enum PlayerInputStates
{
	Input_Release,//0
	Input_Tap,//1
	Input_Hold,//2
};

enum MoveStates
{
	Character_Idle,
	Character_Dash,
};

class Ship : public GameObject
{
public :
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Ship();
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen) override;
	
	void ManageHudControls(sf::Vector2f inputs_directions);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void IdleDecelleration(sf::Time deltaTime);
	bool ScreenBorderContraints();
	void MaxSpeedConstraints();
	//void UpdateMeleeWeaponPosition();

	bool m_disable_inputs;
	ControlerType m_controllerType;
	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];

	void SetControllerType(ControlerType contoller);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	void UpdateInputStates();
	bool UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition);
	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);

	static int Ship::SaveShip(Ship* ship);
	static bool Ship::LoadShip(Ship* ship);

	sf::Clock m_stroboscopic_effect_clock;

	SFPanel* m_SFTargetPanel;
	SFPanelTypes m_is_asking_SFPanel;

	MoveStates m_move_state;

	//dash
	sf::Vector2f m_dash_target;
	float m_overdash_distance;
	GameObject* m_dash_enemy;
	float m_dash_speed;
	float m_dash_radius;
	float m_dash_cooldown;
	float m_dash_cooldown_timer;
	int m_dash_streak;
	vector<GameObject*> m_dash_enemies_tagged;

	//attack
	Weapon* m_weapon;
	bool GetWeapon(Weapon* weapon);
	bool m_is_attacking;
	float m_attack_cooldown_timer;
	float m_attack_cooldown;

	int m_hp;
	int m_hp_max;

	float m_immune_timer;

	void CollisionWithEnemy(GameObject* enemy) override;
	void CollisionWithWeapon(GameObject* enemy_weapon) override;
	void CollisionWithBullet(GameObject* enemy_bullet) override;
	GameObject* GetDashEnemy() override;
	void SetDashEnemy(GameObject* enemy) override;
	bool DealDamage(int dmg) override;
	void Death() override;
	void GetLoot(GameObject* object) override;
	bool IsImmune();

	CircleShape m_dash_target_feedback;

	//debug
	CircleShape m_dash_radius_feedback;

private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;
};

#endif // SHIP_H_INCLUDED
