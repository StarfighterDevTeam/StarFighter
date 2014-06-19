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
	angle = 0.f;
	dispersion = 180.f;

	firing_ready = true;

	this->ammunition = Ammunition;
}

void Weapon::CreateBullet(IndependantType m_collider_type, float offsetX, float dispersion)
{
	Ammo* bullet = this->ammunition->Clone();

	bullet->setPosition(getPosition().x + offsetX, getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
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

			// the rest of the bullets are spread evenly on the left and right of the central bullet
			if (multishot >1) 
			{
				for (int i=1 ; i < (((multishot-1)/2)+1) ; i++)
				{
					int s = 1;//used for symetry
					for (int j=0 ; j<2 ; j++)//2 loops: j=1 and then j=-1
					{
						CreateBullet(m_collider_type, i*s*xspread, i*s*dispersion/2/((multishot-1)/2));
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
					CreateBullet(m_collider_type, (i*s*xspread) - (s*xspread/2), i*s*dispersion/(multishot-1) - s*(dispersion/(multishot-1)/2));
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
				//CreateBullet(m_collider_type, - (((shot_index-1)/2)+1)*xspread);
				CreateBullet(m_collider_type, - (((shot_index-1)/2)+1)*xspread, - (((shot_index-1)/2)+1)*dispersion/2/((multishot-1)/2));
			}
			else
			{
				//CreateBullet(m_collider_type, (shot_index/2)*xspread);
				CreateBullet(m_collider_type, (shot_index/2)*xspread, (shot_index/2)*dispersion/2/((multishot-1)/2));
			}
		}
		

		if (multishot % 2 == 0) //case of an even number of bullets
		{
			if (shot_index %2 !=0)
			{
				//CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2));
				CreateBullet(m_collider_type, - (((shot_index/2)+1)*xspread) + (xspread/2), - ((shot_index/2)+1)*dispersion/(multishot-1) + dispersion/(multishot-1)/2 );
			}
			else
			{
				//CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2));
				CreateBullet(m_collider_type,(((shot_index/2)+1)*xspread) - (xspread/2), ((shot_index/2)+1)*dispersion/(multishot-1) - (dispersion/(multishot-1)/2) );
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
	weapon->fire_direction = this->fire_direction;
	weapon->rate_of_fire = this->rate_of_fire;
	weapon->multishot = this->multishot;
	weapon->xspread = this->xspread;
	weapon->alternate = this->alternate;

	return weapon;
}


