#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(Ammo* Ammunition)
{
	m_speed.x = 0;
	m_speed.y = 0;
	m_multishot = 1;
	m_xspread = 0;
	m_rate_of_fire = 0.4f;
	m_shot_index = 0;
	m_dispersion = 0.f;
	m_firing_ready = true;
	m_rafale_cooldown = 0.8f;
	m_rafale = 0;
	m_rafale_index = 0;
	m_target_homing = NO_HOMING;
	m_shot_mode = NoShotMode;
	m_angle_offset = 0;
	m_delay = 0.f;
	m_weaponOffset = sf::Vector2f(0, 0);
	m_face_target = false;
	m_fire_pattern_return = false;
	m_display_name = "Laser";
	m_level = 1;
	m_credits = 0;
	m_quality = 0;
	m_readyFireTimer = 0;
	m_beam_timer = 0;

	m_ammunition = Ammunition;
}

Weapon::~Weapon()
{
	if (m_ammunition != NULL)
		delete m_ammunition;

	for (Ammo* beam : m_beams)
		beam->Death();
}

void Weapon::CreateBullet(GameObjectType m_collider_type, float offsetX, float dispersion)
{
	Ammo* bullet = m_ammunition->Clone();

	float l_dispersion = dispersion;
	if (m_collider_type == EnemyFire)
	{
		l_dispersion = -l_dispersion;
	}

	//transmitting the value to the bullet
	bullet->m_shot_angle = m_shot_angle;
	bullet->m_offset_x = offsetX;

	//calculation of bullet offset respect to the weapon position
	float bullet_offset_x = bullet->m_offset_x * cos(m_shot_angle) - (bullet->m_size.y / 2 - (bullet->m_size.y * (int)bullet->m_isBeam)) * sin(m_shot_angle);
	float bullet_offset_y = bullet->m_offset_x * sin(m_shot_angle) + (bullet->m_size.y / 2 - (bullet->m_size.y * (int)bullet->m_isBeam)) * cos(m_shot_angle);

	bullet->setPosition(getPosition().x + bullet_offset_x, getPosition().y + bullet_offset_y);

	bullet->m_speed.x = bullet->m_ref_speed * sin(m_shot_angle + (l_dispersion *  M_PI / 180)) * (- m_fire_direction.y);
	bullet->m_speed.y = bullet->m_ref_speed * cos(m_shot_angle + (l_dispersion *  M_PI / 180)) * (m_fire_direction.y);

	bullet->setRotation((m_shot_angle * 180.0f / M_PI) + l_dispersion);

	bullet->m_visible = true;
	bullet->m_collider_type = m_collider_type;
	bullet->m_isOnScene = true;

	bullet->m_collider_type = m_collider_type;
	
	if (bullet->m_isBeam == false)
		bullet->m_layer = m_collider_type == EnemyFire ? EnemyFireLayer : FriendlyFireLayer;
	else
		bullet->m_layer = m_collider_type == EnemyFire ? EnemyBeamLayer : FriendlyBeamLayer;

	(*CurrentGame).addToScene(bullet, true);

	if (m_rafale < 0)//continuous beam created, we need to keep track of it
	{
		m_beams.push_back(bullet);
	}
}

bool Weapon::isFiringReady(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (hyperspeedMultiplier < 1.0f)
	{
		m_readyFireTimer += deltaTime.asSeconds() * hyperspeedMultiplier;
		m_beam_timer += deltaTime.asSeconds() * hyperspeedMultiplier;
	}
	else
	{
		m_readyFireTimer += deltaTime.asSeconds();
		m_beam_timer += deltaTime.asSeconds();
	}

	//enf of beam timer
	if (m_beams.empty() == false && m_rafale_cooldown > 0 && m_beam_timer > m_rafale_cooldown)
	{
		for (Ammo* beam : m_beams)
			beam->Death();

		m_beams.clear();
		m_readyFireTimer = 0;
	}

	//rafale
	bool rafale_ready = (m_rafale == 0);
	if (m_rafale > 0 && m_readyFireTimer > m_rafale_cooldown)
	{
		m_rafale_index = 0;
		m_shot_index = 0;
		rafale_ready = true;
	}

	if (m_readyFireTimer > m_rate_of_fire && rafale_ready == true)
	{
		m_firing_ready = true;
	}
	else if (m_rafale > 0)
	{
		if (m_readyFireTimer > m_rate_of_fire && m_rafale_index <= m_rafale - 1)
		{
			m_firing_ready = true;
		}
	}
	else if (m_rafale < 0)
	{
		m_firing_ready = (m_beams.empty() == true && m_readyFireTimer > m_rate_of_fire);
	}

	return m_firing_ready;
}

void Weapon::Fire(GameObjectType collider_type, sf::Time deltaTime)
{
	if (m_multishot > 1)
	{
		if (m_dispersion == 360)//treating this particular case to avoid a double bullet on the 360th degree
		{
			m_dispersion *= (1 - (1 / m_multishot));//ex: 10 shots at 360°, we make it shots at 324° instead (360 - 360/10 to avoid the double bullet)
		}

		if (m_shot_mode == NoShotMode)
		{
			FireMultiShot(collider_type);
		}
		else if (m_shot_mode == AlternateShotMode)
		{
			FireAlternateShot(collider_type);
		}
		else if (m_shot_mode == AscendingShotMode)
		{
			FireAscendingShot(collider_type);
		}
		else if (m_shot_mode == DescendingShotMode)
		{
			FireDescendingShot(collider_type);
		}
		else if (m_shot_mode == Ascending2ShotMode)
		{
			if (!m_fire_pattern_return)
			{
				FireAscendingShot(collider_type);
			}
			else
			{
				FireDescendingShot(collider_type);
			}

			if (m_shot_index == 0)
			{
				m_fire_pattern_return = !m_fire_pattern_return;
			}
		}
		else if (m_shot_mode == Descending2ShotMode)
		{
			if (!m_fire_pattern_return)
			{
				FireDescendingShot(collider_type);
			}
			else
			{
				FireAscendingShot(collider_type);
			}

			if (m_shot_index == 0)
			{
				m_fire_pattern_return = !m_fire_pattern_return;
			}
		}
	}
	else
	{
		FireSingleShot(collider_type);
	}

	m_readyFireTimer = 0;
	m_firing_ready = false;

	if (m_rafale > 0 && m_shot_index == 0)
	{
		m_rafale_index++;
	}
	else if (m_rafale < 0)//continuous beam started
	{
		m_beam_timer = 0;
	}
}

void Weapon::FireSingleShot(GameObjectType collider_type)
{
	CreateBullet(collider_type);
}

void Weapon::FireMultiShot(GameObjectType collider_type)
{
	if (m_multishot % 2 != 0) //case of an odd number of bullets
	{
		//central bullet
		CreateBullet(collider_type, 0);

		// the rest of the bullets are spread evenly on the left and right of the central bullet
		if (m_multishot > 1)
		{
			for (int i = 1; i < (((m_multishot - 1) / 2) + 1); i++)
			{
				int s = 1;//used for symetry
				for (int j = 0; j < 2; j++)//2 loops: j=1 and then j=-1
				{
					CreateBullet(collider_type, i*s*m_xspread, i*s*m_dispersion / 2 / ((m_multishot - 1) / 2));
					s = -s;
				}
			}
		}
	}

	if (m_multishot % 2 == 0 && m_multishot != 0) //case of an even number of bullets
	{
		for (int i = 1; i < (((m_multishot - 1) / 2) + 2); i++)
		{
			int s = 1;//used for symetry
			for (int j = 0; j < 2; j++)//2 loops: j=1 and then j=-1
			{
				CreateBullet(collider_type, (i*s*m_xspread) - (s*m_xspread / 2), i*s*m_dispersion / (m_multishot - 1) - s*(m_dispersion / (m_multishot - 1) / 2));
				s = -s;
			}
		}
	}
}

void Weapon::FireAlternateShot(GameObjectType m_collider_type)
{
	if (m_multishot % 2 != 0) //case of an odd number of bullets
	{
		if (m_shot_index % 2 != 0)
		{
			//CreateBullet(m_collider_type, - (((shot_index-1)/2)+1)*xspread);
			CreateBullet(m_collider_type, -(((m_shot_index - 1) / 2) + 1)*m_xspread, -(((m_shot_index - 1) / 2) + 1)*m_dispersion / 2 / ((m_multishot - 1) / 2));
		}
		else
		{
			//CreateBullet(m_collider_type, (shot_index/2)*xspread);
			CreateBullet(m_collider_type, (m_shot_index / 2)*m_xspread, (m_shot_index / 2)*m_dispersion / 2 / ((m_multishot - 1) / 2));
		}
	}

	if (m_multishot % 2 == 0) //case of an even number of bullets
	{
		if (m_shot_index % 2 != 0)
		{
			//CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2));
			CreateBullet(m_collider_type, -(((m_shot_index / 2) + 1)*m_xspread) + (m_xspread / 2), -((m_shot_index / 2) + 1)*m_dispersion / (m_multishot - 1) + m_dispersion / (m_multishot - 1) / 2);
		}
		else
		{
			//CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2));
			CreateBullet(m_collider_type, (((m_shot_index / 2) + 1)*m_xspread) - (m_xspread / 2), ((m_shot_index / 2) + 1)*m_dispersion / (m_multishot - 1) - (m_dispersion / (m_multishot - 1) / 2));
		}
	}

	if (m_shot_index < m_multishot - 1)
	{
		m_shot_index++;
	}
	else
	{
		m_shot_index = 0;
	}
}

//left to right order
void Weapon::FireAscendingShot(GameObjectType collider_type)
{
	if (m_multishot % 2 != 0) //case of an even number of bullets
	{
		//left and center bullets (from left to right)
		if (m_shot_index < (((m_multishot - 1) / 2) + 1))
		{
			CreateBullet(collider_type, (-((m_multishot - 1) / 2) + m_shot_index)*m_xspread, (-((m_multishot - 1) / 2) + m_shot_index)*(m_dispersion / (m_multishot - 1)));
		}
		//right
		else
		{
			CreateBullet(collider_type, (m_shot_index - ((m_multishot - 1) / 2))*m_xspread, (m_shot_index - ((m_multishot - 1) / 2))*(m_dispersion / (m_multishot - 1)));
		}
	}

	if (m_multishot % 2 == 0) //case of an odd number of bullets
	{
		//left and center bullets (from left to right)
		if (m_shot_index < (m_multishot / 2))
		{
			CreateBullet(collider_type, (-((m_multishot / 2) + m_shot_index)*m_xspread) + (m_xspread / 2), -((m_multishot / 2) - m_shot_index + (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
		}
		//right
		else
		{
			CreateBullet(collider_type, (m_shot_index - (m_multishot / 2))*m_xspread + (m_xspread / 2), (m_shot_index - (m_multishot / 2) + (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
		}
	}

	if (m_shot_index < m_multishot - 1)
	{
		m_shot_index++;
	}
	else
	{
		m_shot_index = 0;
	}
}

//right to left order
void Weapon::FireDescendingShot(GameObjectType collider_type)
{
	if (m_multishot % 2 != 0) //case of an even number of bullets
	{
		//right and center bullets (from right to left)
		if (m_shot_index < (((m_multishot - 1) / 2) + 1))
		{
			CreateBullet(collider_type, (((m_multishot - 1) / 2) - m_shot_index)*m_xspread, (((m_multishot - 1) / 2) - m_shot_index)*(m_dispersion / (m_multishot - 1)));
		}
		//left
		else
		{
			CreateBullet(collider_type, (-m_shot_index + ((m_multishot - 1) / 2))*m_xspread, (-m_shot_index + ((m_multishot - 1) / 2))*(m_dispersion / (m_multishot - 1)));
		}
	}

	if (m_multishot % 2 == 0) //case of an odd number of bullets
	{
		//right and center bullets (from right to left)
		if (m_shot_index < (m_multishot / 2))
		{
			CreateBullet(collider_type, (((m_multishot / 2) - m_shot_index)*m_xspread) + (m_xspread / 2), ((m_multishot / 2) - m_shot_index - (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
		}
		//left
		else
		{
			CreateBullet(collider_type, (-m_shot_index + (m_multishot / 2))*m_xspread - (m_xspread / 2), (-m_shot_index + (m_multishot / 2) - (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
		}
	}

	if (m_shot_index < m_multishot - 1)
	{
		m_shot_index++;
	}
	else
	{
		m_shot_index = 0;
	}
}

Weapon* Weapon::Clone()
{
	Weapon* weapon = new Weapon(this->m_ammunition->Clone());
	weapon->m_display_name = this->m_display_name;
	weapon->m_fire_direction = this->m_fire_direction;
	weapon->m_rate_of_fire = this->m_rate_of_fire;
	weapon->m_multishot = this->m_multishot;
	weapon->m_xspread = this->m_xspread;
	weapon->m_shot_mode = this->m_shot_mode;
	weapon->m_dispersion = this->m_dispersion;
	weapon->m_rafale = this->m_rafale;
	weapon->m_rafale_cooldown = this->m_rafale_cooldown;
	weapon->m_target_homing = this->m_target_homing;
	weapon->m_angle_offset = this->m_angle_offset;
	weapon->m_delay = this->m_delay;
	weapon->m_weaponOffset = this->m_weaponOffset;

	weapon->m_frameNumber = this->m_frameNumber;
	weapon->m_textureName = this->m_textureName;
	weapon->m_size = this->m_size;

	weapon->m_level = this->m_level;
	weapon->m_credits = this->m_credits;
	weapon->m_quality = this->m_quality;

	return weapon;
}

Weapon* Weapon::CreateRandomWeapon(int level, bool is_bot, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));
	
	int cost_per_multishot = 0;
	//Spending credits on the possible bonuses
	int bonus_multishot = 0;
	int bonus_damage = 0;
	int bonus_rate_of_fire = 0;
	int dispersion = 0;//flag

	int loot_credits_remaining = credits_;
	loot_credits_remaining *= is_bot ? BOT_STATS_MULTIPLIER : 1;
	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = -1;

		//checking bonus limitations
		cost_per_multishot = floor(CREDITS_COST_PER_ONE_MULTISHOT * pow((1 + COST_PER_ONE_MULTISHOT_MULTIPLIER_PER_LEVEL), bonus_multishot));
		bool can_buy_multishot = loot_credits_remaining > cost_per_multishot;
		bool can_buy_rate_of_fire = bonus_rate_of_fire < MAX_RATE_OF_FIRE_BONUS;
		bool can_buy_damage = floor(FIRST_LEVEL_AMMO_DAMAGE * (1 + (1.0f * loot_credits_remaining / 100))) != FIRST_LEVEL_AMMO_DAMAGE;

		//and chosing among the authorized ones
		if (!can_buy_damage && can_buy_rate_of_fire)
		{
			random_type_of_bonus = 2;
		}
		else if (can_buy_multishot && can_buy_rate_of_fire)
		{
			random_type_of_bonus = RandomizeIntBetweenValues(0, 2);
		}
		else if (!can_buy_multishot && can_buy_rate_of_fire)
		{
			random_type_of_bonus = RandomizeIntBetweenValues(1, 2);
		}
		else if (can_buy_multishot && !can_buy_rate_of_fire)
		{
			random_type_of_bonus = RandomizeIntBetweenValues(0, 1);
		}
		else
		{
			random_type_of_bonus = 1;
		}

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
			case 0://multishot
			{
				loot_credits_remaining -= cost_per_multishot;
				bonus_multishot++;
				break;
			}
			case 1://flat bonus damage
			{
				loot_credits_remaining--;
				bonus_damage++;
				break;
			}
			case 2://rate of fire
			{
				loot_credits_remaining--;
				bonus_rate_of_fire++;
				break;
			}
			default:
				break;
		}

		//dispersion weapon?
		int min_multishots_for_dispersion = is_bot ? MIN_MULTISHOTS_FOR_DISPERSION_FOR_BOT - MIN_VALUE_OF_MULTISHOT : MIN_MULTISHOTS_FOR_DISPERSION - MIN_VALUE_OF_MULTISHOT;
		if (bonus_multishot == min_multishots_for_dispersion && dispersion == 0)
		{
			if (RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < WEAPON_CHANCE_OF_DISPERSION)
			{
				dispersion = 1;
			}
			else
			{
				dispersion = -1;
			}
		}
	}

	//Creating the item
	//FX* fx = new FX(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/FX/FX_explosion_S.png", sf::Vector2f(320, 236), 2, sf::seconds(0.4f));
	string fx_name = "explosion_S";
	float duration = atof(((*CurrentGame).m_FXConfig[fx_name][FX_DURATION]).c_str());
	FX* fx = new FX(sf::Vector2f(0, 0), sf::Vector2f(0, 0), (*CurrentGame).m_FXConfig[fx_name][FX_FILENAME], sf::Vector2f(stoi((*CurrentGame).m_FXConfig[fx_name][FX_WIDTH]), stoi((*CurrentGame).m_FXConfig[fx_name][FX_HEIGHT])), 2, sf::seconds(duration));
	fx->m_display_name = fx_name;
	Ammo* ammo = new Ammo(sf::Vector2f(0, 0), sf::Vector2f(0, WEAPON_MIN_VSPEED_VALUE), "2D/Equipment/laser_blue.png", sf::Vector2f(6, 32), -1, fx);
	Weapon* weapon = new Weapon(ammo);
	weapon->m_fire_direction = Vector2i(0, -1);
	weapon->m_textureName = LASER_BLUE_FILENAME;
	weapon->m_size = sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE);
	weapon->m_frameNumber = 1;
	//weapon->m_shot_mode = RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) > WEAPON_CHANCE_OF_ALTERNATE ? NoShotMode : AlternateShotMode;

	//weapon->ammunition->speed.y = RandomizeFloatBetweenValues(sf::Vector2f(500, DEFAULT_AMMO_SPEED));

	weapon->m_target_homing = NO_HOMING;
	weapon->m_ammunition->m_speed.y = DEFAULT_AMMO_SPEED;

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	weapon->m_ammunition->m_damage = FIRST_LEVEL_AMMO_DAMAGE * multiplier_;
	weapon->m_multishot = MIN_VALUE_OF_MULTISHOT;
	weapon->m_rate_of_fire = FIRST_LEVEL_RATE_OF_FIRE;

	//allocating bonuses to the weapon
	weapon->m_multishot += bonus_multishot;
	weapon->m_ammunition->m_damage += ceil((bonus_damage + cost_per_multishot * bonus_multishot) * FIRST_LEVEL_AMMO_DAMAGE / weapon->m_multishot * 0.01);
	weapon->m_rate_of_fire = FIRST_LEVEL_RATE_OF_FIRE / (1 + bonus_rate_of_fire * 0.01);

	//spread of multishot weapons
	if (weapon->m_multishot > 1)
	{
		//int sprite_size = is_bot ? ASSUMED_BOT_SIZE : ASSUMED_SHIP_SIZE;
		//weapon->m_xspread = RandomizeIntBetweenValues(MIN_WEAPON_XSPREAD, sprite_size * 2 / weapon->m_multishot);
		weapon->m_xspread = MIN_WEAPON_XSPREAD;
		
		if (dispersion > 0)
		{
			weapon->m_dispersion = RandomizeFloatBetweenValues(sf::Vector2f(WEAPON_MIN_DISPERSION, is_bot ? WEAPON_MAX_DISPERSION_FOR_BOT : WEAPON_MAX_DISPERSION));
			weapon->m_ammunition->m_damage *= (1 + WEAPON_DISPERSION_DAMAGE_MALUS);
		}
	}

	//saving level and credits used
	weapon->m_level = level;
	weapon->m_credits = credits_;
	weapon->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	//epic stats
	if (Game::GetItemQualityClass(weapon->m_quality) == ItemQuality_Epic)
	{
		int random_epic_ability = RandomizeIntBetweenValues(0, 1);

		switch (random_epic_ability)
		{
			case 0:
			{
				weapon->m_target_homing = HOMING;
				weapon->m_ammunition->m_damage * (1 - WEAPON_HOMING_DAMAGE_MALUS);
				break;
			}
			case 1:
			{
				weapon->m_ammunition->m_Pattern.SetPattern(Oscillator, 200, { 70, 0, 1 });
				weapon->m_ammunition->m_speed.y *= 0.5;
				break;
			}
		}
	}

	#ifndef NDEBUG
		if (!is_bot)
			printf("\nNew weapon created: level %d, quality %f, xp: %d, bonus_multishot: %d, bonus_damage: %d, bonus_rof: %d\n\n", level, weapon->m_quality, credits_, bonus_multishot, bonus_damage, bonus_rate_of_fire);
	#endif

	return weapon;
}