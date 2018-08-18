#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Ship::Ship()
{
	
}

#define PLAYER_DEFAULT_SPEED			500.f

#define DASH_RADIUS						300.f
#define DASH_SPEED						1500
#define DASH_SLOWDOWN_SPEED				300
#define DASH_COOLDOWN					1.0f
//#define DASH_OVERDASH_FACTOR			1.f
#define DASH_OVERDASH_DISTANCE			250.f
#define DASH_COMBO_SLOWMOTION			0.1f

#define IMMUNE_DMG_DURATION				2.f
#define AFTERDEATH_DURATION				2.f

void Ship::Init()
{
	m_layer = PlayerShipLayer;
	m_collider_type = PlayerShip;
	m_moving = false;
	m_movingX = m_movingY = false;
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;
	for (size_t i = 0; i < NBVAL_PlayerActions; i++)
	{
		m_actions_states[i] = false;
	}

	m_SFTargetPanel = NULL;
	m_is_asking_SFPanel = SFPanel_None;

	m_move_state = Character_Idle;
	m_dash_enemy = NULL;

	m_dash_radius = DASH_RADIUS;
	m_dash_speed = DASH_SPEED;
	m_dash_cooldown = DASH_COOLDOWN;
	m_dash_cooldown_timer = m_dash_cooldown;
	m_dash_streak = 0;

	m_is_attacking = false;
	m_weapon = new Weapon(this, Weapon_Katana);
	(*CurrentGame).addToScene(m_weapon, PlayerWeaponLayer, PlayerWeaponObject);
	m_attack_cooldown_timer = m_weapon->m_attack_cooldown;

	m_immune_timer = IMMUNE_DMG_DURATION;
	m_afterdeath_timer = AFTERDEATH_DURATION;

	m_hp_max = 3;
	m_hp = m_hp_max;

	m_dash_ammo_max = 2;
	m_dash_ammo_current = m_dash_ammo_max;

	m_dash_target_feedback.setRadius(40);
	m_dash_target_feedback.setOrigin(sf::Vector2f(40, 40));
	m_dash_target_feedback.setFillColor(sf::Color(0, 0, 0, 0));
	m_dash_target_feedback.setOutlineThickness(6);
	m_dash_target_feedback.setOutlineColor(sf::Color(0, 255, 0, 100));
	m_dash_target_feedback.setPosition(getPosition());

	m_hit_feedback_timer = HIT_FEEDBACK_DURATION;

	//debug
	m_dash_radius_feedback.setRadius(m_dash_radius);
	m_dash_radius_feedback.setOrigin(sf::Vector2f(m_dash_radius, m_dash_radius));
	m_dash_radius_feedback.setFillColor(sf::Color(0, 0, 0, 0));
	m_dash_radius_feedback.setOutlineThickness(2);
	m_dash_radius_feedback.setOutlineColor(sf::Color(255, 255, 255, 30));
	m_dash_radius_feedback.setPosition(getPosition());
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	Init();
}

Ship::~Ship()
{
	if (m_weapon)
	{
		m_weapon->m_GarbageMe = true;
		m_weapon->m_visible = false;
	}
}

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Ship::update(sf::Time deltaTime)
{
	//update timers (only if it's still meaningful to do so)
	if (m_immune_timer < IMMUNE_DMG_DURATION)
		m_immune_timer += deltaTime.asSeconds();

	if (m_weapon && m_attack_cooldown_timer < m_weapon->m_attack_cooldown)
		m_attack_cooldown_timer += deltaTime.asSeconds();

	if (m_dash_cooldown_timer < m_dash_cooldown)
		m_dash_cooldown_timer += deltaTime.asSeconds();

	if (m_stroboscopic_effect_timer < STROBOSCOPIC_EFFECT_FREQUENCY)
		m_stroboscopic_effect_timer += deltaTime.asSeconds();

	if (m_hit_feedback_timer < HIT_FEEDBACK_DURATION)
	{
		m_hit_feedback_timer += deltaTime.asSeconds();
		if (m_hit_feedback_timer >= HIT_FEEDBACK_DURATION && m_hp <= 0)
		{
			Death();
		}
	}

	//reset color
	setColor(m_color);

	//dead?
	if (m_afterdeath_timer < AFTERDEATH_DURATION)
	{
		m_afterdeath_timer += deltaTime.asSeconds();

		if (m_afterdeath_timer >= AFTERDEATH_DURATION)
		{
			Respawn();
		}
		else
		{
			//allow to play while dead? -only for debug
			//return;
		}
	}
	
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	if ((*CurrentGame).m_window_has_focus)
	{
		inputs_direction= InputGuy::getDirections();
	}
		
	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	//Action input
	UpdateInputStates();

	//ATTACKS
	GameObject* dash_enemy = NULL;
	if (m_dash_ammo_current > 0)
	{
		dash_enemy = (*CurrentGame).getDashTargetWithBlacklist(m_dash_radius, m_dash_enemies_tagged);
	}

	if (m_move_state == Character_Idle)
	{
		ManageAcceleration(inputs_direction);

		//Attack
		if (m_is_attacking)//reset
		{
			if (m_weapon && m_weapon->m_attack_timer >= m_weapon->m_attack_duration)
			{
				m_weapon->m_visible = false;
				m_weapon->Extend(sf::Vector2f(0.f, 1.f));
				m_weapon->m_enemies_tagged.clear();
				m_weapon->setColor(m_weapon->m_color);

				if (m_weapon->m_is_ranged && m_weapon->m_bullet_is_unique && m_weapon->GetFiredBulletsCount() > 0)
				{
					//do nothing
				}
				else
				{
					m_is_attacking = false;
					m_attack_cooldown_timer = 0.f;
				}
			}
		}
		
		if (m_inputs_states[Action_Melee] == Input_Tap)//start attack
		{
			if (m_weapon && m_attack_cooldown_timer >= m_weapon->m_attack_cooldown)
			{
				if (!m_is_attacking)
				{
					if (m_weapon->m_is_ranged == false)
					{
						m_weapon->m_visible = true;
						(*CurrentGame).PlaySFX(SFX_Melee);
					}
					else
					{
						m_weapon->Shoot(dash_enemy);//shoot an enemy. If no enemy found, it will shoot towards current rotation.
					}

					m_is_attacking = true;
					m_weapon->m_attack_timer = 0.f;
				}
			}
		}
		

		if (m_is_attacking)//update
		{
			if (m_weapon->m_attack_duration > 0)
			{
				float ratio = m_weapon->m_attack_timer / m_weapon->m_attack_duration;
				if (ratio > 1.0f)
				{
					ratio = 1.0f;
				}

				m_weapon->Extend(sf::Vector2f(ratio, 1.f));
			}
		}

		//Dash?
		if (m_inputs_states[Action_Dash] == Input_Tap)//start
		{
			if (m_dash_ammo_current > 0)
			{
				if (m_dash_cooldown_timer >= m_dash_cooldown)
				{
					m_dash_streak = 0;

					(*CurrentGame).CreateSFTextPop("Dash", Font_Arial, 24, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y / 2 - 20);
					m_move_state = Character_Dash;

					float cur_angle = (getRotation() - 180) / 180 * M_PI;

					float dash_distance = m_dash_radius;
					sf::Vector2f dash_vector = GetVectorFromLengthAndAngle(dash_distance, cur_angle);

					if (dash_enemy)
					{
						m_dash_streak = 1;
						m_dash_enemy = dash_enemy;
						m_dash_enemies_tagged.push_back(dash_enemy);

						dash_vector = m_dash_enemy->getPosition() - getPosition();

						m_overdash_distance = DASH_OVERDASH_DISTANCE;
						dash_distance = GetDistanceBetweenPositions(m_dash_enemy->getPosition(), getPosition()) + m_overdash_distance;

						ScaleVector(&dash_vector, dash_distance);
					}

					m_dash_target = getPosition() + dash_vector;

					m_speed = dash_vector;
					ScaleVector(&m_speed, m_dash_speed);

					m_dash_ammo_current--;
					(*CurrentGame).PlaySFX(SFX_Dash);
				}
			}
		}
	}

	if (m_move_state == Character_Dash)
	{
		sf::Vector2f dash_vector = m_dash_target - getPosition();

		if (m_dash_enemy)//start dash
		{
			dash_vector = m_dash_enemy->getPosition() - getPosition();

			float dash_distance = GetDistanceBetweenPositions(m_dash_enemy->getPosition(), getPosition()) + m_overdash_distance;

			ScaleVector(&dash_vector, dash_distance);

			m_dash_target = getPosition() + dash_vector;
		}
		else
		{
			if (m_dash_streak >= 1)//dash update: start combo?
			{
				if (m_inputs_states[Action_Dash] == Input_Tap)
				{
					if (m_dash_ammo_current > 0)
					{
						GameObject* new_dash_enemy = (*CurrentGame).getDashTargetWithBlacklist(m_dash_radius, m_dash_enemies_tagged);

						if (new_dash_enemy)
						{

							//dash combo
							m_dash_streak++;
							m_dash_enemy = new_dash_enemy;
							m_dash_enemies_tagged.push_back(new_dash_enemy);

							ostringstream ss;
							ss << "Dash (" << m_dash_streak << ")";
							(*CurrentGame).CreateSFTextPop(ss.str(), Font_Arial, 24, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y / 2 - 20);

							dash_vector = m_dash_enemy->getPosition() - getPosition();

							m_overdash_distance = DASH_OVERDASH_DISTANCE;//GetVectorLength(dash_vector) * DASH_OVERDASH_FACTOR;
							float dash_distance = GetDistanceBetweenPositions(m_dash_enemy->getPosition(), getPosition()) + m_overdash_distance;

							ScaleVector(&dash_vector, dash_distance);

							m_dash_target = getPosition() + dash_vector;
							m_speed = dash_vector;
							ScaleVector(&m_speed, m_dash_speed);

							m_dash_ammo_current--;

							(*CurrentGame).PlaySFX(SFX_Dash);
						}
					}
				}
			}
		}

		float dist_to_target = GetDistanceBetweenPositions(getPosition(), m_dash_target);
		//slow down if: 1) we have dashed 1 time already AND 2) there is another remaining valid target within range for another dash
		float dash_speed = DASH_SPEED;
		if (!m_dash_enemy && m_dash_streak > 0 && dash_enemy)
		{
			(*CurrentGame).m_timescale = DASH_COMBO_SLOWMOTION;
			(*CurrentGame).m_shader.setParameter("ratio", 0.75f);
		}
		else
		{
			(*CurrentGame).m_timescale = 1.f;
			(*CurrentGame).m_shader.setParameter("ratio", 0.f);
		}

		if (dist_to_target > dash_speed * deltaTime.asSeconds())
		{
			m_speed = dash_vector;
			ScaleVector(&m_speed, dash_speed);
		}
		else//end of dash
		{
			m_speed = sf::Vector2f(0, 0);
			setPosition(m_dash_target);
			m_move_state = Character_Idle;
			m_dash_cooldown_timer = 0.f;
			m_dash_enemies_tagged.clear();
			(*CurrentGame).m_timescale = 1.f;
			(*CurrentGame).m_shader.setParameter("ratio", 0.f);
		}
		
		//printf("Speed: %f, %f (angle: %f)\n", m_speed.x, m_speed.y, GetAngleRadBetweenPositions(getPosition(), m_dash_target));
	}

	//MaxSpeedConstraints();
	if (m_move_state == Character_Idle)
	{
		IdleDecelleration(deltaTime);
	}

	SetConditionalColor(sf::Color(200, 200, 200, 180), m_move_state == Character_Dash, true);
	SetConditionalColor(sf::Color(m_color.r, m_color.g, m_color.b, 255 * cos(m_immune_timer * 10)), m_immune_timer < IMMUNE_DMG_DURATION, false);

	if (m_hit_feedback_timer < HIT_FEEDBACK_DURATION)
	{
		setColor(sf::Color::Red);
	}

	if (m_move_state == Character_Dash)
	{
		PlayStroboscopicEffect(sf::seconds(STROBOSCOPIC_EFFECT_DURATION), sf::seconds(STROBOSCOPIC_EFFECT_FREQUENCY));
	}

	GameObject::update(deltaTime);

	UpdateRotation();

	//HUD
	m_is_asking_SFPanel = SFPanel_None;
	if (m_SFTargetPanel)
	{
		m_SFTargetPanel->Update(deltaTime);
	}

	if (ScreenBorderContraints())
	{
		if (m_move_state == Character_Dash)
		{
			m_move_state = Character_Idle;
			m_speed = sf::Vector2f(0, 0);
			m_dash_cooldown_timer = 0.f;
			m_dash_enemies_tagged.clear();
			(*CurrentGame).m_timescale = 1.f;
			(*CurrentGame).m_shader.setParameter("ratio", 0.f);
		}
	}

	UpdateWeaponPosition(m_weapon);

	if (dash_enemy && !m_dash_enemy && m_dash_cooldown_timer >= m_dash_cooldown)
	{
		m_dash_target_feedback.setOutlineColor(sf::Color(0, 255, 0, 100));
		m_dash_target_feedback.setPosition(dash_enemy->getPosition());
	}
	else
	{
		m_dash_target_feedback.setOutlineColor(sf::Color(0, 0, 0, 0));
	}

	//debug
	m_dash_radius_feedback.setPosition(getPosition());

	if (m_move_state == Character_Idle)
	{
		if (m_dash_cooldown_timer >= m_dash_cooldown)
		{
			m_center_feedback.setFillColor(sf::Color(255, 255, 255, 255));
		}
		else
		{
			m_center_feedback.setFillColor(sf::Color(255, 128, 0, 255));
		}
	}
	if (m_move_state == Character_Dash)
	{
		m_center_feedback.setFillColor(sf::Color(255, 0, 0, 255));
	}

	//if (IsImmune())
	//{
	//	m_center_feedback.setFillColor(sf::Color(0, 0, 0, 255));
	//}
}

bool Ship::ScreenBorderContraints()
{
	bool touched_screen_border = false;

	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		m_speed.x = 0;
		touched_screen_border = true;
	}

	if (this->getPosition().x > (*CurrentGame).m_map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).m_map_size.x - (m_size.x / 2), this->getPosition().y);
		m_speed.x = 0;
		touched_screen_border = true;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		m_speed.y = 0;
		touched_screen_border = true;
	}

	if (this->getPosition().y > (*CurrentGame).m_map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).m_map_size.y - (m_size.y / 2));
		m_speed.y = 0;
		touched_screen_border = true;
	}

	return touched_screen_border;
}

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_stroboscopic_effect_timer >= time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_timer = 0.f;
	}
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle decceleration
	if (!m_movingX)
	{
		m_speed.x = 0;
	}

	if (!m_movingY)
	{
		m_speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	m_speed.x = inputs_direction.x * PLAYER_DEFAULT_SPEED;
	m_speed.y = inputs_direction.y * PLAYER_DEFAULT_SPEED;

	//m_speed.x += inputs_direction.x * SHIP_ACCELERATION;
	//m_speed.y += inputs_direction.y * SHIP_ACCELERATION;

	//max speed constraints
	//if (abs(m_speed.x) > SHIP_MAX_SPEED)
	//{
	//	m_speed.x = m_speed.x > 0 ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;
	//}
	//if (abs(m_speed.y) > SHIP_MAX_SPEED)
	//{
	//	m_speed.y = m_speed.y > 0 ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;
	//}
}

void Ship::MaxSpeedConstraints()
{
	float ship_max_speed = SHIP_MAX_SPEED;

	//max speed constraints
	NormalizeVector(&m_speed, ship_max_speed);
}

void Ship::UpdateInputStates()
{
	if ((*CurrentGame).m_window_has_focus)
	{
		GetInputState(InputGuy::isFiring(), Action_Melee);
		GetInputState(InputGuy::isDashing(), Action_Dash);
	}
	else
	{
		GetInputState(false, Action_Melee);
		GetInputState(false, Action_Dash);
	}
}

bool Ship::UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition)
{
	if (state_required == Input_Tap && condition && m_inputs_states[action] == Input_Tap)
	{
		m_actions_states[action] = !m_actions_states[action];
		return true;
	}
	else if (state_required == Input_Hold && condition)
	{
		m_actions_states[action] = m_inputs_states[action];
		return true;
	}
	else if (!condition)
	{
		m_actions_states[action] = false;
	}
	return false;
}

void Ship::GetInputState(bool input_guy_boolean, PlayerActions action)
{
	if (input_guy_boolean)
	{
		m_inputs_states[action] = m_inputs_states[action] == Input_Release ? Input_Tap : Input_Hold;
	}
	else
	{
		m_inputs_states[action] = Input_Release;
	}
}

//SAVE AND LOAD LOCAL FILE
int Ship::SaveShip(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving game in local file.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		data << "Save ";// << ship->m_speed.x << endl;

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No existing save file founded. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Ship::LoadShip(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading ship from local file.\n");
	assert(ship != NULL);

	std::ifstream  data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			string saved_content;
			ss >> saved_content;
			//ship->content = saved_content;
		}

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void Ship::CollisionWithEnemy(GameObject* enemy)
{
	if (enemy == m_dash_enemy)
	{
		if (GetDistanceBetweenObjects(enemy, this) < 16)//we don't aim for a pixel collision here but for a true proximity to the center of the enemy
		{
			int dmg = m_weapon ? m_weapon->m_dmg : 0;
			m_dash_enemy->DealDamage(dmg);

			m_dash_enemy = NULL;
		}
	}
	else if (m_move_state != Character_Dash)
	{
		Enemy* enemy_ = (Enemy*)enemy;
		DealDamage(enemy_->m_dmg);
	}
}

void Ship::CollisionWithWeapon(GameObject* enemy_weapon)
{
	if (m_move_state != Character_Dash)
	{
		enemy_weapon->CollisionWithEnemy(this);
	}
}

void Ship::CollisionWithBullet(GameObject* enemy_bullet)
{
	if (m_move_state != Character_Dash)
	{
		enemy_bullet->CollisionWithBullet(this);
	}
}

void Ship::Draw(sf::RenderTexture& screen)
{
	GameObject::Draw(screen);

	if (m_visible)
	{
		screen.draw(m_dash_radius_feedback);
		screen.draw(m_dash_target_feedback);
	}
}

GameObject* Ship::GetDashEnemy()
{
	return m_dash_enemy;
}

void Ship::SetDashEnemy(GameObject* enemy)
{
	m_dash_enemy = enemy;
}

bool Ship::DealDamage(int dmg)
{
	if (m_immune_timer < IMMUNE_DMG_DURATION == false)
	{
		m_immune_timer = 0.f;
		m_hp -= dmg;

		ostringstream ss;
		ss << "-" << dmg;
		(*CurrentGame).CreateSFTextPop(ss.str(), Font_Arial, 30, sf::Color::Red, getPosition(), PlayerBlue, 100, 50, 2, NULL, -m_size.y*getScale().y / 2);

		if (dmg > 0)
		{
			m_hit_feedback_timer = 0.f;
			(*CurrentGame).PlaySFX(SFX_GruntPlayer);
		}
	}

	return m_hp <= 0;
}

void Ship::Death()
{
	setGhost(true);
	m_weapon->setGhost(true);

	m_move_state = Character_Idle;
	m_speed = sf::Vector2f(0, 0);

	m_dash_enemy = NULL;

	m_afterdeath_timer = 0.f;

	(*CurrentGame).m_shader.setParameter("ratio", 1.f);
}

void Ship::Respawn()
{
	m_dash_cooldown_timer = m_dash_cooldown;
	m_immune_timer = IMMUNE_DMG_DURATION;
	m_dash_ammo_current = m_dash_ammo_max;

	Weapon* weapon = new Weapon(this, Weapon_Katana);
	GetWeapon(weapon);
	delete weapon;
	m_attack_cooldown_timer = m_weapon->m_attack_cooldown;

	//debug respawn
	setGhost(false);
	m_weapon ->setGhost(false);
	//setPosition(sf::Vector2f(SHIP_START_X, SHIP_START_Y));
	m_hp = m_hp_max;

	(*CurrentGame).m_shader.setParameter("ratio", 0.f);
}

bool Ship::GetWeapon(Weapon* weapon)
{
	//Weapon* m_weapon = weapon->m_is_ranged ? m_is_ranged_weapon : m_weapon;
	m_weapon;

	//no current weapon? keep it...
	if (m_weapon == NULL)
	{
		m_weapon = new Weapon(this, weapon->m_type);
	}
	else if (m_weapon->m_type == weapon->m_type)
	{
		return false;
	}
	
	//...otherwise, apply new weapon's properties
	m_weapon->m_type = weapon->m_type;
	m_weapon->m_color = weapon->m_color;

	m_weapon->m_range = weapon->m_range;
	m_weapon->m_attack_duration = weapon->m_attack_duration;
	m_weapon->m_is_piercing = weapon->m_is_piercing;
	m_weapon->m_is_ranged = weapon->m_is_ranged;
	m_weapon->m_can_be_parried = weapon->m_can_be_parried;
			
	m_weapon->m_enemies_tagged.clear();
	m_weapon->m_visible = false;

	if (weapon->m_is_ranged)
	{
		m_weapon->m_bullet->m_textureName = weapon->m_bullet->m_textureName;
		m_weapon->m_bullet->m_size = weapon->m_bullet->m_size;
		m_weapon->m_bullet->m_frameNumber = weapon->m_bullet->m_frameNumber;
		m_weapon->m_bullet_speed = weapon->m_bullet_speed;
		m_weapon->m_bullet_is_following_target = weapon->m_bullet_is_following_target;
		m_weapon->m_bullet_is_unique = weapon->m_bullet_is_unique;
	}
	
	return true;
}

void Ship::GetLoot(GameObject* object)
{
	object->m_GarbageMe = true;
	object->m_visible = false;

	Loot* loot = (Loot*)object;
	//bonus
	switch (loot->m_type)
	{
		case Loot_BonusMeleeRange:
		{
			m_weapon->m_range += loot->m_range_bonus;
			break;
		}

		case Loot_DashAmmo:
		{
			size_t max = m_dash_ammo_max - m_dash_ammo_current;
			m_dash_ammo_current += loot->m_dash_ammo > max ? max : loot->m_dash_ammo;
			break;
		}

		case Loot_WeaponKatana:
		case Loot_WeaponSpear:
		case Loot_WeaponShuriken:
		{
			Weapon* weapon = new Weapon(this, loot->m_weapon_type);
			GetWeapon(weapon);
			delete weapon;
			weapon = NULL;
			break;
		}
	}
}