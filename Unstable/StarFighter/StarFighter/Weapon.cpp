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
	firing_ready = true;

	this->ammunition = Ammunition;
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
			Ammo* bullet = this->ammunition->Clone();
			bullet->setPosition(getPosition().x,getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
			bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
			bullet->setVisible(true);
			bullet->collider_type = m_collider_type;
			bullet->isOnScene = true;

			(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);

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
			Ammo* bullet = this->ammunition->Clone();
			bullet->setPosition(getPosition().x,getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
			bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
			bullet->setVisible(true);
			bullet->collider_type = m_collider_type;
			bullet->isOnScene = true;

			(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);

			if (multishot >1) // the rest of the bullets are sprea evenly on the left and right of the central bullet
			{
				for (int i=1 ; i < (((multishot-1)/2)+1) ; i++)
				{
					int s = 1;//used for symetry
					for (int j=0 ; j<2 ; j++)//2 loops: j=1 and then j=-1
					{
						Ammo* bullet2 = this->ammunition->Clone();
						bullet2->setPosition(getPosition().x + (i*s*xspread),getPosition().y + ((bullet2->m_size.y/2)*(fire_direction.y)));
						bullet2->speed = sf::Vector2f(bullet2->speed.x*(fire_direction.x),bullet2->speed.y*(fire_direction.y));
						bullet2->setVisible(true);
						bullet2->collider_type = m_collider_type;
						bullet2->isOnScene = true;
						s = -s;
						(*CurrentGame).addToScene(bullet2,PlayerShipLayer, m_collider_type);
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
					Ammo* bullet = this->ammunition->Clone();
					bullet->setPosition(getPosition().x + (i*s*xspread) - (s*xspread/2),getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
					bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
					bullet->setVisible(true);
					bullet->collider_type = m_collider_type;
					bullet->isOnScene = true;
					s = -s;
					(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);
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
				Ammo* bullet = this->ammunition->Clone();
				bullet->setPosition(getPosition().x - (((shot_index-1)/2)+1)*xspread,getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
				bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
				bullet->setVisible(true);
				bullet->collider_type = m_collider_type;
				bullet->isOnScene = true;
				(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);
			}
			else
			{
				Ammo* bullet = this->ammunition->Clone();
				bullet->setPosition(getPosition().x + (shot_index/2)*xspread,getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
				bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
				bullet->setVisible(true);
				bullet->collider_type = m_collider_type;
				bullet->isOnScene = true;
				(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);
			}
		}
		

		if (multishot % 2 == 0) //case of an even number of bullets
		{
			if (shot_index %2 !=0)
			{
				Ammo* bullet = this->ammunition->Clone();
				bullet->setPosition(getPosition().x - (((shot_index/2)+1)*xspread) + (xspread/2),getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
				bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
				bullet->setVisible(true);
				bullet->collider_type = m_collider_type;
				bullet->isOnScene = true;
				(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);
			}
			else
			{
				Ammo* bullet = this->ammunition->Clone();
				bullet->setPosition(getPosition().x + (((shot_index/2)+1)*xspread) - (xspread/2),getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
				bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
				bullet->setVisible(true);
				bullet->collider_type = m_collider_type;
				bullet->isOnScene = true;
				(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);
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


