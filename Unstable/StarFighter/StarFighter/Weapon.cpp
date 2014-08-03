#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(Ammo* Ammunition)
{
	speed.x = 0;
	speed.y = 0;
	multishot = 1;
	xspread = 10;
	rate_of_fire = 0.4f;
	alternate = false;
	shot_index = 0;
	angle = 0.f;
	dispersion = 0.f;
	firing_ready = true;
	rafale_cooldown = 0.8f;
	rafale = 0;
	rafale_index = 0;

	this->ammunition = Ammunition;
}

void Weapon::CreateBullet(IndependantType m_collider_type, float offsetX, float dispersion)
{
	Ammo* bullet = this->ammunition->Clone();

	bullet->setPosition(getPosition().x + offsetX + OffsetWeapon(dispersion+angle).x, getPosition().y  + OffsetWeapon(dispersion+angle).y);
	//bullet->setPosition(getPosition().x + offsetX + OffsetWeapon(dispersion+angle).x, getPosition().y - ((bullet->m_size.y/2)*(fire_direction.y)) + OffsetWeapon(dispersion+angle).y);
	//bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));

	bullet->speed = this->AngleShot(this->angle + dispersion, bullet->ref_speed * fire_direction.y);
	if (m_collider_type == IndependantType::FriendlyFire)
	{
		bullet->rotate(this->angle + dispersion);
	}
	else
	{
		bullet->rotate(this->angle + dispersion + 180);
	}
	bullet->setVisible(true);
	bullet->collider_type = m_collider_type;
	bullet->isOnScene = true;

	(*CurrentGame).addToScene(bullet,LayerType::PlayerShipLayer, m_collider_type);
}

bool Weapon::isFiringReady()
{
	if (rafale > 0 && deltaClock.getElapsedTime() > sf::seconds(rafale_cooldown))//if you wait long enough, you can reset your rafale
	{
		firing_ready = true;
		rafale_index = 0;
		shot_index = 0;
	}

	if (rafale > 0 && rafale_index > rafale-1)
	{
		if (deltaClock.getElapsedTime() > sf::seconds(rafale_cooldown))
		{
			firing_ready = true;
			rafale_index = 0;
		}
	}
	else
	{
		if (alternate && multishot > 1)
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
			if (!alternate)
			{
				FireMultiShot(m_collider_type);
			}
			else
			{
				FireAlternateShot(m_collider_type);
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
		if (multishot >1) 
		{
			for (int i=1 ; i < (((multishot-1)/2)+1) ; i++)
			{
				int s = 1;//used for symetry
				for (int j=0 ; j<2 ; j++)//2 loops: j=1 and then j=-1
				{
					CreateBullet(m_collider_type, i*s*xspread, i*s*dispersion/2/((multishot-1)/2) +angle);
					s = -s;
				}
			}
		}
	}

	if (multishot % 2 == 0 && multishot != 0) //case of an even number of bullets
	{
		for (int i=1 ; i < (((multishot-1)/2)+2) ; i++)
		{
			int s = 1;//used for symetry
			for (int j=0 ; j<2 ; j++)//2 loops: j=1 and then j=-1
			{
				CreateBullet(m_collider_type, (i*s*xspread) - (s*xspread/2), i*s*dispersion/(multishot-1) - s*(dispersion/(multishot-1)/2) +angle);
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
			CreateBullet(m_collider_type, - (((shot_index-1)/2)+1)*xspread, - (((shot_index-1)/2)+1)*dispersion/2/((multishot-1)/2) +angle);
		}
		else
		{
			//CreateBullet(m_collider_type, (shot_index/2)*xspread);
			CreateBullet(m_collider_type, (shot_index/2)*xspread, (shot_index/2)*dispersion/2/((multishot-1)/2) +angle);
		}
	}

	if (multishot % 2 == 0) //case of an even number of bullets
	{
		if (shot_index %2 !=0)
		{
			//CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2));
			CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2), - ((shot_index/2)+1)*dispersion/(multishot-1) + dispersion/(multishot-1)/2 +angle);
		}
		else
		{
			//CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2));
			CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2), ((shot_index/2)+1)*dispersion/(multishot-1) - (dispersion/(multishot-1)/2) +angle );
		}

	}

	if (shot_index < multishot-1)
		shot_index++;
	else
	{
		shot_index = 0;	
	}

}

sf::Vector2f Weapon::AngleShot(float angle, float m_ref_speed)
{
	sf::Vector2f new_speed;
	angle = - angle * M_PI / 180;
	new_speed.x = m_ref_speed * sin (angle);
	new_speed.y = m_ref_speed * cos (angle);
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
	weapon->alternate = this->alternate;
	weapon->dispersion = this->dispersion;
	weapon->rafale = this->rafale;
	weapon->rafale_cooldown = this->rafale_cooldown;

	return weapon;
}


sf::Vector2f Weapon::OffsetWeapon(float angle)
{
	//default values

	float x = 0;
	float y = - this->weaponOffset.x;
	/*
	if (angle >= -45.f && angle <= 45.f)
	{
	x = - weaponOffset.y * tan(angle);
	y = - weaponOffset.y;
	printf("OFFSET BONUS X, Y:%f, %f\n",x, y);
	}

	if (angle > 45.f && angle <= 135.f)
	{
	x = weaponOffset.x;
	y = - weaponOffset.x * tan(angle-45);
	}

	if (angle > 135.f && angle <= 180.f)
	{
	x = - weaponOffset.y * tan(angle-135);
	y = + weaponOffset.y;
	}

	if (angle >= -135.f && angle <= -45.f)
	{
	x = - weaponOffset.x;
	y = + weaponOffset.x * tan(angle+45);;
	}

	if (angle > -180.f && angle < -135.f)
	{
	x = - weaponOffset.y * tan(angle+135);
	y = + weaponOffset.y;
	}*/

	return sf::Vector2f(x,y);
}

#define WEAPON_RATE_OF_FIRE_MULTIPLIER				5
#define WEAPON_MULTISHOT_MULTIPLIER					2
#define WEAPON_CHANCE_OF_DISPERSION					0.50
#define WEAPON_DISPERSION_MAX_ANGLE					170
#define WEAPON_CHANCE_OF_ALTERNATE					0.25
#define WEAPON_VSPEED_LN_MULTIPLIER					20

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

			if (this->multishot * this->xspread > 32)
				this->xspread = (32 / this->multishot);

			double dispersion_chance = (double) rand() / (RAND_MAX);
			if (dispersion_chance < WEAPON_CHANCE_OF_DISPERSION)
			{
				int dispersion_roll = rand() % (WEAPON_DISPERSION_MAX_ANGLE+1);
				this->dispersion = dispersion_roll;
			}

			double alternate_chance = (double) rand() / (RAND_MAX);
			if (alternate_chance < WEAPON_CHANCE_OF_ALTERNATE)
			{
				this->alternate = true;
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