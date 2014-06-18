#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(Ammo* Ammunition)
{
	speed.x = 0;
	speed.y = 0;
	multishot = 1;
	xspread = 10;
	alternate = false;
	shot_index = 0;
	angle = 0.0f;

	firing_ready = true;

	this->ammunition = Ammunition;
}

void Weapon::CreateBullet(IndependantType m_collider_type, float offsetX)
{
	Ammo* bullet = this->ammunition->Clone();

	bullet->setPosition(getPosition().x + offsetX, getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
	bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));

	bullet->speed = this->AngleShot(this->angle, bullet->speed);//rotation of the
	bullet->rotate(-180 * this->angle / M_PI);//conversion from degres to radian

	bullet->setVisible(true);
	bullet->collider_type = m_collider_type;
	bullet->isOnScene = true;

	(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);
}

void Weapon::Fire(IndependantType m_collider_type)
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
		if (deltaClock.getElapsedTime() > sf::seconds(rate_of_fire))
		{
			firing_ready = true;
		}

		if (firing_ready)
		{
			CreateBullet(m_collider_type);

			deltaClock.restart();
			firing_ready = false;		
		}
	}
}

void Weapon::FireMultiShot(IndependantType m_collider_type)
{
	if (deltaClock.getElapsedTime() > sf::seconds(rate_of_fire))
	{
		firing_ready = true;
	}

	if (firing_ready)
	{
		if (multishot % 2 != 0) //case of an odd number of bullets
		{
			//central bullet
			CreateBullet(m_collider_type);

			if (multishot >1) // the rest of the bullets are sprea evenly on the left and right of the central bullet
			{
				for (int i=1 ; i < (((multishot-1)/2)+1) ; i++)
				{
					int s = 1;//used for symetry
					for (int j=0 ; j<2 ; j++)//2 loops: j=1 and then j=-1
					{
						CreateBullet(m_collider_type, i*s*xspread);
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
					CreateBullet(m_collider_type, (i*s*xspread) - (s*xspread/2));
					s = -s;
				}
			}
		}


		deltaClock.restart();
		firing_ready = false;		
	}
}

void Weapon::FireAlternateShot(IndependantType m_collider_type)
{
	if (deltaClock.getElapsedTime() > sf::seconds(rate_of_fire / multishot))
	{
		firing_ready = true;
	}

	if (firing_ready)
	{
		if (multishot % 2 != 0) //case of an odd number of bullets
		{
			if (shot_index % 2 != 0)
			{
				CreateBullet(m_collider_type, - (((shot_index-1)/2)+1)*xspread);
			}
			else
			{
				CreateBullet(m_collider_type, (shot_index/2)*xspread);
			}
		}
		

		if (multishot % 2 == 0) //case of an even number of bullets
		{
			if (shot_index %2 !=0)
			{
				CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2));
			}
			else
			{
				CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2));
			}
			
		}

		if (shot_index < multishot-1)
			shot_index++;
		else
			shot_index = 0;

		deltaClock.restart();
		firing_ready = false;		
	}
}

sf::Vector2f Weapon::AngleShot(float angle, sf::Vector2f m_speed)
{
	sf::Vector2f new_speed;
	new_speed.x = m_speed.y * sin (angle);
	new_speed.y = m_speed.y * cos (angle);
	return new_speed;
}

Weapon* Weapon::Clone()
{
	Weapon* weapon = new Weapon(this->ammunition->Clone());
	weapon->fire_direction = this->fire_direction;
	weapon->rate_of_fire = this->rate_of_fire;
	weapon->multishot = this->multishot;
	weapon->xspread = this->xspread;
	weapon->alternate = this->alternate;

	return weapon;
}


