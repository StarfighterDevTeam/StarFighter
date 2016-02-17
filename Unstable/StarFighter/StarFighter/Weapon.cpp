#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(Ammo* Ammunition)
{
	speed.x = 0;
	speed.y = 0;
	multishot = 1;
	xspread = 0;
	rate_of_fire = 0.4f;
	shot_index = 0;
	dispersion = 0.f;
	firing_ready = true;
	rafale_cooldown = 0.8f;
	rafale = 0;
	rafale_index = 0;
	target_seaking = NO_SEAKING;
	shot_mode = NoShotMode;
	angle_offset = 0;
	delay = 0.f;
	weaponOffset = sf::Vector2f(0, 0);
	face_target = false;
	fire_pattern_return = false;
	display_name = "Laser";
	level = 1;
	credits = 0;

	this->ammunition = Ammunition;
}

Weapon::~Weapon()
{
	delete this->ammunition;
	this->ammunition = NULL;
}

void Weapon::CreateBullet(IndependantType m_collider_type, float offsetX, float dispersion)
{
	Ammo* bullet = this->ammunition->Clone();

	float l_dispersion = dispersion;
	if (m_collider_type == EnemyFire)
	{
		l_dispersion = -l_dispersion;
	}

	//transmitting the value to the bullet
	bullet->shot_angle = this->shot_angle;

	//calculation of bullet offset respect to the weapon position
	float bullet_offset_x = offsetX * cos(this->shot_angle) - this->ammunition->m_size.y / 2 * sin(this->shot_angle);
	float bullet_offset_y = offsetX * sin(this->shot_angle) + this->ammunition->m_size.y / 2 * cos(this->shot_angle);

	bullet->setPosition(this->getPosition().x + bullet_offset_x, this->getPosition().y + bullet_offset_y);

	bullet->speed.x = bullet->ref_speed * sin(this->shot_angle + (l_dispersion *  M_PI / 180)) * (-this->fire_direction.y);
	bullet->speed.y = bullet->ref_speed * cos(this->shot_angle + (l_dispersion *  M_PI / 180)) * (this->fire_direction.y);

	bullet->setRotation((this->shot_angle * 180.0f / M_PI) + l_dispersion);

	bullet->visible = true;
	bullet->collider_type = m_collider_type;
	bullet->isOnScene = true;

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
		this->readyFireTimer += deltaTime * hyperspeedMultiplier;
	}
	else
	{
		this->readyFireTimer += deltaTime;
	}

	bool rafale_ended = true;
	if (rafale > 0)
	{
		rafale_ended = false;
	}
	if (rafale > 0 && rafale_index > rafale - 1 && readyFireTimer > sf::seconds(rafale_cooldown))//if you wait long enough, you can reset your rafale
	{
		rafale_index = 0;
		shot_index = 0;
		rafale_ended = true;
	}

	if (readyFireTimer > sf::seconds(rate_of_fire) && rafale_ended)
	{
		firing_ready = true;
	}
	else if (rafale > 0)
	{
		if (readyFireTimer > sf::seconds(rate_of_fire) && rafale_index <= rafale - 1)
		{
			firing_ready = true;
		}
	}

	return firing_ready;
}

void Weapon::Fire(IndependantType m_collider_type, sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (multishot > 1)
	{
		if (dispersion == 360)//treating this particular case to avoid a double bullet on the 360th degree
		{
			dispersion *= (1 - (1 / multishot));//ex: 10 shots at 360°, we make it shots at 324° instead (360 - 360/10 to avoid the double bullet)
		}

		if (this->shot_mode == NoShotMode)
		{
			FireMultiShot(m_collider_type);
		}
		else if (this->shot_mode == AlternateShotMode)
		{
			FireAlternateShot(m_collider_type);
		}
		else if (this->shot_mode == AscendingShotMode)
		{
			FireAscendingShot(m_collider_type);
		}
		else if (this->shot_mode == DescendingShotMode)
		{
			FireDescendingShot(m_collider_type);
		}
		else if (this->shot_mode == Ascending2ShotMode)
		{
			if (!fire_pattern_return)
			{
				FireAscendingShot(m_collider_type);
			}
			else
			{
				FireDescendingShot(m_collider_type);
			}

			if (shot_index == 0)
			{
				fire_pattern_return = !fire_pattern_return;
			}
		}
		else if (this->shot_mode == Descending2ShotMode)
		{
			if (!fire_pattern_return)
			{
				FireDescendingShot(m_collider_type);
			}
			else
			{
				FireAscendingShot(m_collider_type);
			}

			if (shot_index == 0)
			{
				fire_pattern_return = !fire_pattern_return;
			}
		}
	}
	else
	{
		FireSingleShot(m_collider_type);
	}

	readyFireTimer = sf::seconds(0);
	firing_ready = false;

	if (rafale > 0 && shot_index == 0)
	{
		rafale_index++;
	}
}

void Weapon::FireSingleShot(IndependantType m_collider_type)
{
	CreateBullet(m_collider_type);
}

void Weapon::FireMultiShot(IndependantType m_collider_type)
{
	if (multishot % 2 != 0) //case of an odd number of bullets
	{
		//central bullet
		CreateBullet(m_collider_type, 0);

		// the rest of the bullets are spread evenly on the left and right of the central bullet
		if (multishot > 1)
		{
			for (int i = 1; i < (((multishot - 1) / 2) + 1); i++)
			{
				int s = 1;//used for symetry
				for (int j = 0; j < 2; j++)//2 loops: j=1 and then j=-1
				{
					CreateBullet(m_collider_type, i*s*xspread, i*s*dispersion / 2 / ((multishot - 1) / 2));
					s = -s;
				}
			}
		}
	}

	if (multishot % 2 == 0 && multishot != 0) //case of an even number of bullets
	{
		for (int i = 1; i < (((multishot - 1) / 2) + 2); i++)
		{
			int s = 1;//used for symetry
			for (int j = 0; j < 2; j++)//2 loops: j=1 and then j=-1
			{
				CreateBullet(m_collider_type, (i*s*xspread) - (s*xspread / 2), i*s*dispersion / (multishot - 1) - s*(dispersion / (multishot - 1) / 2));
				s = -s;
			}
		}
	}
}

void Weapon::FireAlternateShot(IndependantType m_collider_type)
{
	if (multishot % 2 != 0) //case of an odd number of bullets
	{
		if (shot_index % 2 != 0)
		{
			//CreateBullet(m_collider_type, - (((shot_index-1)/2)+1)*xspread);
			CreateBullet(m_collider_type, -(((shot_index - 1) / 2) + 1)*xspread, -(((shot_index - 1) / 2) + 1)*dispersion / 2 / ((multishot - 1) / 2));
		}
		else
		{
			//CreateBullet(m_collider_type, (shot_index/2)*xspread);
			CreateBullet(m_collider_type, (shot_index / 2)*xspread, (shot_index / 2)*dispersion / 2 / ((multishot - 1) / 2));
		}
	}

	if (multishot % 2 == 0) //case of an even number of bullets
	{
		if (shot_index % 2 != 0)
		{
			//CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2));
			CreateBullet(m_collider_type, -(((shot_index / 2) + 1)*xspread) + (xspread / 2), -((shot_index / 2) + 1)*dispersion / (multishot - 1) + dispersion / (multishot - 1) / 2);
		}
		else
		{
			//CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2));
			CreateBullet(m_collider_type, (((shot_index / 2) + 1)*xspread) - (xspread / 2), ((shot_index / 2) + 1)*dispersion / (multishot - 1) - (dispersion / (multishot - 1) / 2));
		}
	}

	if (shot_index < multishot - 1)
	{
		shot_index++;
	}
	else
	{
		shot_index = 0;
	}
}

//left to right order
void Weapon::FireAscendingShot(IndependantType m_collider_type)
{
	if (multishot % 2 != 0) //case of an even number of bullets
	{
		//left and center bullets (from left to right)
		if (shot_index < (((multishot - 1) / 2) + 1))
		{
			CreateBullet(m_collider_type, (-((multishot - 1) / 2) + shot_index)*xspread, (-((multishot - 1) / 2) + shot_index)*(dispersion / (multishot - 1)));
		}
		//right
		else
		{
			CreateBullet(m_collider_type, (shot_index - ((multishot - 1) / 2))*xspread, (shot_index - ((multishot - 1) / 2))*(dispersion / (multishot - 1)));
		}
	}

	if (multishot % 2 == 0) //case of an odd number of bullets
	{
		//left and center bullets (from left to right)
		if (shot_index < (multishot / 2))
		{
			CreateBullet(m_collider_type, (-((multishot / 2) + shot_index)*xspread) + (xspread / 2), -((multishot / 2) - shot_index + (xspread / 2))*(dispersion / (multishot - 1)));
		}
		//right
		else
		{
			CreateBullet(m_collider_type, (shot_index - (multishot / 2))*xspread + (xspread / 2), (shot_index - (multishot / 2) + (xspread / 2))*(dispersion / (multishot - 1)));
		}
	}

	if (shot_index < multishot - 1)
	{
		shot_index++;
	}
	else
	{
		shot_index = 0;
	}
}

//right to left order
void Weapon::FireDescendingShot(IndependantType m_collider_type)
{
	if (multishot % 2 != 0) //case of an even number of bullets
	{
		//right and center bullets (from right to left)
		if (shot_index < (((multishot - 1) / 2) + 1))
		{
			CreateBullet(m_collider_type, (((multishot - 1) / 2) - shot_index)*xspread, (((multishot - 1) / 2) - shot_index)*(dispersion / (multishot - 1)));
		}
		//left
		else
		{
			CreateBullet(m_collider_type, (-shot_index + ((multishot - 1) / 2))*xspread, (-shot_index + ((multishot - 1) / 2))*(dispersion / (multishot - 1)));
		}
	}

	if (multishot % 2 == 0) //case of an odd number of bullets
	{
		//right and center bullets (from right to left)
		if (shot_index < (multishot / 2))
		{
			CreateBullet(m_collider_type, (((multishot / 2) - shot_index)*xspread) + (xspread / 2), ((multishot / 2) - shot_index - (xspread / 2))*(dispersion / (multishot - 1)));
		}
		//left
		else
		{
			CreateBullet(m_collider_type, (-shot_index + (multishot / 2))*xspread - (xspread / 2), (-shot_index + (multishot / 2) - (xspread / 2))*(dispersion / (multishot - 1)));
		}
	}

	if (shot_index < multishot - 1)
	{
		shot_index++;
	}
	else
	{
		shot_index = 0;
	}
}

Weapon* Weapon::Clone()
{
	Weapon* weapon = new Weapon(this->ammunition->Clone());
	weapon->display_name = this->display_name;
	weapon->fire_direction = this->fire_direction;
	weapon->rate_of_fire = this->rate_of_fire;
	weapon->multishot = this->multishot;
	weapon->xspread = this->xspread;
	weapon->shot_mode = this->shot_mode;
	weapon->dispersion = this->dispersion;
	weapon->rafale = this->rafale;
	weapon->rafale_cooldown = this->rafale_cooldown;
	weapon->target_seaking = this->target_seaking;
	weapon->angle_offset = this->angle_offset;
	weapon->delay = this->delay;
	weapon->weaponOffset = this->weaponOffset;

	weapon->frameNumber = this->frameNumber;
	weapon->textureName = this->textureName;
	weapon->size = this->size;

	weapon->level = this->level;
	weapon->credits = this->credits;

	return weapon;
}

sf::Vector2i Weapon::getFireDirection_for_Direction(Directions direction)
{
	sf::Vector2i fire_direction_ = sf::Vector2i(0, this->fire_direction.y);

	if (direction == DIRECTION_DOWN)
	{
		fire_direction_.y = -this->fire_direction.y;
	}
	else if (direction == DIRECTION_RIGHT)
	{
		fire_direction_.x = -this->fire_direction.y;
		fire_direction_.y = 0;
	}
	else if (direction == DIRECTION_LEFT)
	{
		fire_direction_.x = this->fire_direction.y;
		fire_direction_.y = 0;
	}

	return fire_direction_;
}

Weapon* Weapon::CreateRandomWeapon(int credits_, int level)
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
	weapon->fire_direction = Vector2i(0, -1);
	weapon->textureName = LASER_BLUE_FILENAME;
	weapon->size = sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE);
	weapon->frameNumber = 1;

	//weapon->ammunition->speed.y = RandomizeFloatBetweenValues(sf::Vector2f(500, DEFAULT_AMMO_SPEED));

	weapon->target_seaking = NO_SEAKING;
	weapon->ammunition->speed.y = DEFAULT_AMMO_SPEED;

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	weapon->ammunition->damage = FIRST_LEVEL_AMMO_DAMAGE * multiplier_;
	weapon->multishot = MIN_VALUE_OF_MULTISHOT;
	weapon->rate_of_fire = FIRST_LEVEL_RATE_OF_FIRE;

	//allocating bonuses to the weapon
	weapon->ammunition->damage += (bonus_damage + CREDITS_COST_PER_ONE_MULTISHOT * bonus_multishot) * FIRST_LEVEL_AMMO_DAMAGE * 0.01;
	weapon->multishot += bonus_multishot;
	weapon->rate_of_fire -= bonus_rate_of_fire * FIRST_LEVEL_RATE_OF_FIRE * 0.01;

	//spread of multishot weapons
	if (weapon->multishot > 1)
	{
		weapon->xspread = RandomizeIntBetweenValues(0, ASSUMED_SHIP_SIZE * 2 / weapon->multishot);
	}

	//saving level and credits used
	weapon->level = level;
	weapon->credits = credits_;

	return weapon;
}