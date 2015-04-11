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
	angle = 0.f;
	dispersion = 0.f;
	firing_ready = true;
	rafale_cooldown = 0.8f;
	rafale = 0;
	rafale_index = 0;
	target_seaking = TargetSeaking::NO_SEAKING;
	shot_mode = ShotMode::NoShotMode;
	angle_offset = 0;
	target_seaking_angle = 0;
	weaponOffset = sf::Vector2f(0, 0);
	face_target = false;

	this->ammunition = Ammunition;
}

void Weapon::CreateBullet(IndependantType m_collider_type, float offsetX, float dispersion)
{
	Ammo* bullet = this->ammunition->Clone();

	//if target seaking (closest enemy)
	if (target_seaking != TargetSeaking::NO_SEAKING)
	{
		if (rafale > 0 && rafale_index > 0 && target_seaking == TargetSeaking::SEMI_SEAKING)
		{
			//do nothing
		}
		else
		{
			if (m_collider_type == IndependantType::FriendlyFire)
			{
				target_seaking_angle = (*CurrentGame).GetAngleToNearestIndependant(IndependantType::EnemyObject, this->getPosition(), bullet->range);
			}
			else if (m_collider_type == IndependantType::EnemyFire)
			{
				target_seaking_angle = (*CurrentGame).GetAngleToNearestIndependant(IndependantType::PlayerShip, this->getPosition(), bullet->range);
				target_seaking_angle += 180;
			}
		}
	}

	//calculating the angle of the shot, which will determine how to offset the position of the bullet respect to the weapon position
	bullet->shot_angle = this->shot_angle - (this->angle_offset / 180 * M_PI);
	if (!this->face_target)
	{
		bullet->shot_angle = this->shot_angle - ((this->angle_offset + this->target_seaking_angle) / 180 * M_PI);
	}

	//calculation of bullet offset respect to the weapon position
	float bullet_offset_x = offsetX * cos(this->shot_angle) + this->ammunition->m_size.y / 2 * sin(this->shot_angle);
	float bullet_offset_y = offsetX * sin(this->shot_angle) + this->ammunition->m_size.y / 2 * cos(this->shot_angle);
	

	//because we don't use fire direction on the X axis, it needs to be changed manually here
	if (m_collider_type == IndependantType::FriendlyFire)
	{
		bullet_offset_x *= -1;
	}

 	bullet->setPosition(this->getPosition().x + bullet_offset_x, this->getPosition().y + bullet_offset_y);

	bullet->speed = this->AngleShot(this->angle + dispersion + angle_offset + target_seaking_angle, bullet->ref_speed);
	bullet->speed.x = bullet->speed.x * - this->fire_direction.y;
	bullet->speed.y = bullet->speed.y * this->fire_direction.y;
	bullet->speed = Independant::getSpeed_for_Direction((*CurrentGame).direction, bullet->speed);

	bullet->rotate(Independant::getRotation_for_Direction((*CurrentGame).direction));
	bullet->rotate(this->angle - dispersion - angle_offset - target_seaking_angle);

	bullet->visible = true;
	bullet->collider_type = m_collider_type;
	bullet->isOnScene = true;
	if (m_collider_type == IndependantType::FriendlyFire)
	{
		(*CurrentGame).addToScene(bullet, LayerType::FriendlyFireLayer, m_collider_type);
	}
	else
	{
		(*CurrentGame).addToScene(bullet, LayerType::EnemyFireLayer, m_collider_type);
	}

}

bool Weapon::isFiringReady()
{
	if (rafale > 0 && deltaClock.getElapsedTime() > sf::seconds(rafale_cooldown))//if you wait long enough, you can reset your rafale
	{
		firing_ready = true;
		rafale_index = 0;
		shot_index = 0;
	}

	if (rafale > 0 && rafale_index > rafale - 1)
	{
		if (deltaClock.getElapsedTime() > sf::seconds(rafale_cooldown))
		{
			firing_ready = true;
			rafale_index = 0;
		}
	}
	else
	{
		if (this->shot_mode != ShotMode::NoShotMode && multishot > 1)
		{
			if (deltaClock.getElapsedTime() > sf::seconds(rate_of_fire / multishot))
			{
				firing_ready = true;
			}
		}

		else if (deltaClock.getElapsedTime() > sf::seconds(rate_of_fire))
		{
			firing_ready = true;
		}
	}

	return firing_ready;
}

void Weapon::Fire(IndependantType m_collider_type)
{
	if (isFiringReady())
	{
		if (multishot > 1)
		{
			if (dispersion == 360)//treating this particular case to avoid a double bullet on the 360th degree
			{
				dispersion *= (1 - (1 / multishot));//ex: 10 shots at 360°, we make it shots at 324° instead (360 - 360/10 to avoid the double bullet)
			}

			if (this->shot_mode == ShotMode::NoShotMode)
			{
				FireMultiShot(m_collider_type);
			}
			else if (this->shot_mode == ShotMode::AlternateShotMode)
			{
				FireAlternateShot(m_collider_type);
			}
			else if (this->shot_mode == ShotMode::AscendingShotMode)
			{
				FireAscendingShot(m_collider_type);
			}
			else if (this->shot_mode == ShotMode::DescendingShotMode)
			{
				FireDescendingShot(m_collider_type);
			}
		}
		else
		{
			FireSingleShot(m_collider_type);
		}

		deltaClock.restart();
		firing_ready = false;

		if (rafale > 0 && shot_index == 0)
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
		CreateBullet(m_collider_type, 0, angle);

		// the rest of the bullets are spread evenly on the left and right of the central bullet
		if (multishot > 1)
		{
			for (int i = 1; i < (((multishot - 1) / 2) + 1); i++)
			{
				int s = 1;//used for symetry
				for (int j = 0; j < 2; j++)//2 loops: j=1 and then j=-1
				{
					CreateBullet(m_collider_type, i*s*xspread, i*s*dispersion / 2 / ((multishot - 1) / 2) + angle);
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
				CreateBullet(m_collider_type, (i*s*xspread) - (s*xspread / 2), i*s*dispersion / (multishot - 1) - s*(dispersion / (multishot - 1) / 2) + angle);
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
			CreateBullet(m_collider_type, -(((shot_index - 1) / 2) + 1)*xspread, -(((shot_index - 1) / 2) + 1)*dispersion / 2 / ((multishot - 1) / 2) + angle);
		}
		else
		{
			//CreateBullet(m_collider_type, (shot_index/2)*xspread);
			CreateBullet(m_collider_type, (shot_index / 2)*xspread, (shot_index / 2)*dispersion / 2 / ((multishot - 1) / 2) + angle);
		}
	}

	if (multishot % 2 == 0) //case of an even number of bullets
	{
		if (shot_index % 2 != 0)
		{
			//CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2));
			CreateBullet(m_collider_type, -(((shot_index / 2) + 1)*xspread) + (xspread / 2), -((shot_index / 2) + 1)*dispersion / (multishot - 1) + dispersion / (multishot - 1) / 2 + angle);
		}
		else
		{
			//CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2));
			CreateBullet(m_collider_type, (((shot_index / 2) + 1)*xspread) - (xspread / 2), ((shot_index / 2) + 1)*dispersion / (multishot - 1) - (dispersion / (multishot - 1) / 2) + angle);
		}

	}

	if (shot_index < multishot - 1)
		shot_index++;
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
			CreateBullet(m_collider_type, (-((multishot - 1) / 2) + shot_index)*xspread, (-((multishot-1)/2) + shot_index)*(dispersion/(multishot-1)));
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
			CreateBullet(m_collider_type, (-((multishot / 2) + shot_index)*xspread) + (xspread / 2), -((multishot / 2) + shot_index + (xspread / 2))*(dispersion / (multishot - 1)));
		}
		//right
		else
		{
			CreateBullet(m_collider_type, (shot_index - (multishot / 2))*xspread + (xspread / 2), (shot_index - (multishot / 2) + (xspread / 2))*(dispersion / (multishot - 1)));
		}
	}

	if (shot_index < multishot - 1)
		shot_index++;
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
		shot_index++;
	else
	{
		shot_index = 0;
	}
}

sf::Vector2f Weapon::AngleShot(float angle, float m_ref_speed)
{
	sf::Vector2f new_speed;
	angle = -angle * M_PI / 180;
	new_speed.x = m_ref_speed * sin(angle);
	new_speed.y = m_ref_speed * cos(angle);
	return new_speed;
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
	weapon->weaponOffset = this->weaponOffset;

	weapon->frameNumber = this->frameNumber;
	weapon->textureName = this->textureName;
	weapon->size = this->size;

	return weapon;
}

#define WEAPON_RATE_OF_FIRE_MULTIPLIER				5
#define WEAPON_MULTISHOT_MULTIPLIER					2
#define WEAPON_CHANCE_OF_DISPERSION					0.50
#define WEAPON_DISPERSION_MAX_ANGLE					170
#define WEAPON_CHANCE_OF_ALTERNATE					0.10
#define WEAPON_CHANCE_OF_ASCENDING					0.10
#define WEAPON_CHANCE_OF_DESCENDING					0.10
#define WEAPON_VSPEED_LN_MULTIPLIER					50

#define BOT_WEAPON_RATE_OF_FIRE_MALUS_MULTIPLIER	1
#define BOT_WEAPON_MULTISHOT_MALUS_MULTIPLIER		0.5
#define BOT_WEAPON_VSPEED_MALUS_MULTIPLIER			1
#define BOT_WEAPON_DAMAGE_MALUS_MULTIPLIER			1

void Weapon::AddWeaponProperty(int chosen_property, int value, sf::Vector2f BeastScale)
{
	switch (chosen_property)
	{
		case 0:
		{
			this->rate_of_fire /= (RandomizeFloatBetweenValues(BeastScale) * WEAPON_RATE_OF_FIRE_MULTIPLIER);
			break;
		}
		case 1:
		{
			this->multishot = RandomizeIntBetweenFloats(sf::Vector2f(BeastScale.x*WEAPON_MULTISHOT_MULTIPLIER, BeastScale.y*WEAPON_MULTISHOT_MULTIPLIER));
			if (multishot > 1)
			{
				this->xspread = RandomizeIntBetweenFloats(sf::Vector2f(0, 32));
				if (this->xspread * multishot > this->size.x)
				{
					this->xspread = this->size.x / multishot;
				}
			}

			double dispersion_chance = (double)rand() / (RAND_MAX);
			if (dispersion_chance < WEAPON_CHANCE_OF_DISPERSION)
			{
				int dispersion_roll = rand() % (WEAPON_DISPERSION_MAX_ANGLE + 1);
				this->dispersion = dispersion_roll;
			}

			double alternate_chance = (double)rand() / (RAND_MAX);
			if (alternate_chance < WEAPON_CHANCE_OF_ALTERNATE)
			{
				this->shot_mode = ShotMode::AlternateShotMode;
			}
			else if (alternate_chance < WEAPON_CHANCE_OF_ALTERNATE + WEAPON_CHANCE_OF_ASCENDING)
			{
				this->shot_mode = ShotMode::AscendingShotMode;
			}
			else if (alternate_chance < WEAPON_CHANCE_OF_ALTERNATE + WEAPON_CHANCE_OF_ASCENDING + WEAPON_CHANCE_OF_DESCENDING)
			{
				this->shot_mode = ShotMode::DescendingShotMode;
			}
			break;
		}
		case 2:
		{
			this->ammunition->speed.y += (log(WEAPON_VSPEED_LN_MULTIPLIER*RandomizeFloatBetweenValues(BeastScale)));
			break;
		}
		case 3:
		{
			//this->ammunition->damage += RandomizeFloatBetweenRatios(value, BeastScale);
			float log_multiplier = WEAPON_DAMAGE_LN_MULTIPLIER_BONUS * (log(value * WEAPON_DAMAGE_LN_MULTIPLIER_X));

			float e_damage = WEAPON_MIN_DAMAGE_VALUE * RandomizeFloatBetweenValues(BeastScale);
			if (log_multiplier > 1)
				e_damage *= log_multiplier;
			else
				e_damage = ProrataBetweenThreshold(value, sf::Vector2f(0, e_damage));

			this->ammunition->damage += e_damage;
			this->ammunition->damage = floor(this->ammunition->damage);
			break;
		}
		default:
		{
			printf("DEBUG: error: trying to add Weapon property that does not exit.\n<!> Check that the chosen property for this Weapon match with the existing properties in the AddWeaponProperty function.\n");
			break;
		}
	}
}

void Weapon::AddBotWeaponProperty(int chosen_property, int value, sf::Vector2f BeastScale)
{
	switch (chosen_property)
	{
		case 0:
		{
			AddWeaponProperty(chosen_property, value, sf::Vector2f(BeastScale.x*BOT_WEAPON_RATE_OF_FIRE_MALUS_MULTIPLIER, BeastScale.y*BOT_WEAPON_RATE_OF_FIRE_MALUS_MULTIPLIER));
			break;
		}
		case 1:
		{
			AddWeaponProperty(chosen_property, value, sf::Vector2f(BeastScale.x*BOT_WEAPON_MULTISHOT_MALUS_MULTIPLIER, BeastScale.y*BOT_WEAPON_MULTISHOT_MALUS_MULTIPLIER));
			break;
		}
		case 2:
		{
			AddWeaponProperty(chosen_property, value, sf::Vector2f(BeastScale.x*BOT_WEAPON_VSPEED_MALUS_MULTIPLIER, BeastScale.y*BOT_WEAPON_VSPEED_MALUS_MULTIPLIER));
			break;
		}
		case 3:
		{
			AddWeaponProperty(chosen_property, value, sf::Vector2f(BeastScale.x*BOT_WEAPON_DAMAGE_MALUS_MULTIPLIER, BeastScale.y*BOT_WEAPON_DAMAGE_MALUS_MULTIPLIER));
			break;
		}
		default:
		{
			printf("DEBUG: error: trying to add Weapon property that does not exit.\n<!> Check that the chosen property for this Weapon match with the existing properties in the AddWeaponProperty function.\n");
			break;
		}
	}
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