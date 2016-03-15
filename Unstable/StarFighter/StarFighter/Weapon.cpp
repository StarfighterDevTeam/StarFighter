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
	m_target_seaking = NO_SEAKING;
	m_shot_mode = NoShotMode;
	m_angle_offset = 0;
	m_delay = 0.f;
	m_weaponOffset = sf::Vector2f(0, 0);
	m_face_target = false;
	m_fire_pattern_return = false;
	m_display_name = "Laser";
	m_level = 1;
	m_credits = 0;
	m_readyFireTimer = sf::seconds(0);

	m_ammunition = Ammunition;
}

Weapon::~Weapon()
{
	if (m_ammunition)
	{
		delete m_ammunition;
		m_ammunition = NULL;
	}
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
	bullet->m_shot_angle = this->m_shot_angle;

	//calculation of bullet offset respect to the weapon position
	float bullet_offset_x = offsetX * cos(this->m_shot_angle) - this->m_ammunition->m_size.y / 2 * sin(this->m_shot_angle);
	float bullet_offset_y = offsetX * sin(this->m_shot_angle) + this->m_ammunition->m_size.y / 2 * cos(this->m_shot_angle);

	bullet->setPosition(this->getPosition().x + bullet_offset_x, this->getPosition().y + bullet_offset_y);

	bullet->m_speed.x = bullet->m_ref_speed * sin(this->m_shot_angle + (l_dispersion *  M_PI / 180)) * (-this->m_fire_direction.y);
	bullet->m_speed.y = bullet->m_ref_speed * cos(this->m_shot_angle + (l_dispersion *  M_PI / 180)) * (this->m_fire_direction.y);

	bullet->setRotation((this->m_shot_angle * 180.0f / M_PI) + l_dispersion);

	bullet->m_visible = true;
	bullet->m_collider_type = m_collider_type;
	bullet->m_isOnScene = true;

	if (m_collider_type == FriendlyFire)
	{
		(*CurrentGame).addToScene(bullet, FriendlyFireLayer, m_collider_type);
	}
	else
	{
		(*CurrentGame).addToScene(bullet, EnemyFireLayer, m_collider_type);
	}
}

bool Weapon::isFiringReady(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (hyperspeedMultiplier < 1.0f)
	{
		m_readyFireTimer += deltaTime * hyperspeedMultiplier;
	}
	else
	{
		m_readyFireTimer += deltaTime;
	}

	bool rafale_ended = true;
	if (m_rafale > 0)
	{
		rafale_ended = false;
	}
	if (m_rafale > 0 && m_rafale_index > m_rafale - 1 && m_readyFireTimer > sf::seconds(m_rafale_cooldown))//if you wait long enough, you can reset your rafale
	{
		m_rafale_index = 0;
		m_shot_index = 0;
		rafale_ended = true;
	}

	if (m_readyFireTimer > sf::seconds(m_rate_of_fire) && rafale_ended)
	{
		m_firing_ready = true;
	}
	else if (m_rafale > 0)
	{
		if (m_readyFireTimer > sf::seconds(m_rate_of_fire) && m_rafale_index <= m_rafale - 1)
		{
			m_firing_ready = true;
		}
	}

	return m_firing_ready;
}

void Weapon::Fire(GameObjectType collider_type, sf::Time deltaTime, float hyperspeedMultiplier)
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

	m_readyFireTimer = sf::seconds(0);
	m_firing_ready = false;

	if (m_rafale > 0 && m_shot_index == 0)
	{
		m_rafale_index++;
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
	weapon->m_target_seaking = this->m_target_seaking;
	weapon->m_angle_offset = this->m_angle_offset;
	weapon->m_delay = this->m_delay;
	weapon->m_weaponOffset = this->m_weaponOffset;

	weapon->m_frameNumber = this->m_frameNumber;
	weapon->m_textureName = this->m_textureName;
	weapon->m_size = this->m_size;

	weapon->m_level = this->m_level;
	weapon->m_credits = this->m_credits;

	return weapon;
}

sf::Vector2i Weapon::getFireDirection_for_Direction(Directions direction)
{
	sf::Vector2i fire_direction = sf::Vector2i(0, m_fire_direction.y);

	if (direction == DIRECTION_DOWN)
	{
		fire_direction.y = -m_fire_direction.y;
	}
	else if (direction == DIRECTION_RIGHT)
	{
		fire_direction.x = -m_fire_direction.y;
		fire_direction.y = 0;
	}
	else if (direction == DIRECTION_LEFT)
	{
		fire_direction.x = m_fire_direction.y;
		fire_direction.y = 0;
	}

	return fire_direction;
}

Weapon* Weapon::CreateRandomWeapon(int credits_, int level, bool is_bot)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	int cost_per_multishot = floor(CREDITS_COST_PER_ONE_MULTISHOT * pow((1 + COST_PER_ONE_MULTISHOT_MULTIPLIER_PER_LEVEL), level - 1));
	//Spending credits on the possible bonuses
	int bonus_multishot = 0;
	int bonus_damage = 0;
	int bonus_rate_of_fire = 0;

	int loot_credits_remaining = credits_;
	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = -1;

		//checking bonus limitations
		bool can_buy_multishot = credits_ > cost_per_multishot;
		bool can_buy_rate_of_fire = bonus_rate_of_fire < MAX_RATE_OF_FIRE_BONUS && floor(FIRST_LEVEL_AMMO_DAMAGE * (1 + (1.0f * credits_ / 100))) != FIRST_LEVEL_AMMO_DAMAGE;

		//and chosing among the authorized ones
		if (can_buy_multishot && can_buy_rate_of_fire)
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
	}

	//Creating the item
	FX* fx = new FX(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/FX_explosion_S_blue.png", sf::Vector2f(320, 236), 2, sf::seconds(0.4f));
	Ammo* ammo = new Ammo(sf::Vector2f(0, 0), sf::Vector2f(0, WEAPON_MIN_VSPEED_VALUE), "Assets/2D/Equipment/laser_blue.png", sf::Vector2f(6, 32), -1, fx);
	Weapon* weapon = new Weapon(ammo);
	weapon->m_fire_direction = Vector2i(0, -1);
	weapon->m_textureName = LASER_BLUE_FILENAME;
	weapon->m_size = sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE);
	weapon->m_frameNumber = 1;

	//weapon->ammunition->speed.y = RandomizeFloatBetweenValues(sf::Vector2f(500, DEFAULT_AMMO_SPEED));

	weapon->m_target_seaking = NO_SEAKING;
	weapon->m_ammunition->m_speed.y = DEFAULT_AMMO_SPEED;

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	weapon->m_ammunition->m_damage = FIRST_LEVEL_AMMO_DAMAGE * multiplier_;
	weapon->m_multishot = MIN_VALUE_OF_MULTISHOT;
	weapon->m_rate_of_fire = FIRST_LEVEL_RATE_OF_FIRE;

	//allocating bonuses to the weapon
	weapon->m_ammunition->m_damage += ceil((bonus_damage + CREDITS_COST_PER_ONE_MULTISHOT * bonus_multishot) * FIRST_LEVEL_AMMO_DAMAGE * 0.01);
	weapon->m_multishot += bonus_multishot;
	weapon->m_rate_of_fire -= bonus_rate_of_fire * FIRST_LEVEL_RATE_OF_FIRE * 0.01;

	//spread of multishot weapons
	if (weapon->m_multishot > 1)
	{
		int sprite_size = is_bot ? ASSUMED_BOT_SIZE : ASSUMED_SHIP_SIZE;
		weapon->m_xspread = RandomizeIntBetweenValues(MIN_WEAPON_XSPREAD, sprite_size * 2 / weapon->m_multishot);
	}

	//saving level and credits used
	weapon->m_level = level;
	weapon->m_credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return weapon;
}