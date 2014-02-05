#include "Independant.h"


Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber)  : AnimatedSprite()
{
	Init(position,speed,textureName,size,frameNumber);
	this->setOrigin(origin.x, origin.y);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin)  : AnimatedSprite()
{
	Init(position,speed,textureName,size);
	this->setOrigin(origin.x, origin.y);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : AnimatedSprite()
{
	Init(position,speed,textureName,size);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture) : AnimatedSprite()
{
	Init(position,speed,texture,1);
}

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber)
{
	this->m_size.x = ((*texture).getSize().x/frameNumber);
	this->m_size.y = ((*texture).getSize().y);

	this->collider_type = IndependantType::Background;
	this->defaultAnimation.setSpriteSheet(*texture);
	for(int i=0; i< frameNumber;i++)
	{
		int x =  ((*texture).getSize().x/frameNumber)*(i);
		this->defaultAnimation.addFrame(sf::IntRect(x, 0, this->m_size.x, this->m_size.y));
	}
	this->currentAnimation = &defaultAnimation;
	this->play(*currentAnimation);

	this->speed = speed;
	this->setPosition(position.x,position.y);
	this->visible = false;
	this->isOnScene = false;
	this->immune = false;
}

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(textureName, size.x, size.y);

	this->setOrigin(size.x/2, size.y/2);

	Init(position,speed,texture,frameNumber);
}

Independant::~Independant()
{
	//Unload Texture

}

void Independant::update(sf::Time deltaTime)
{
	float x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
	float y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();
	this->setPosition(x,y);

	AnimatedSprite::update(deltaTime);
}

void Independant::updateAnimation(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}


void Independant::setVisible(bool m_visible)
{
	this->visible=m_visible;
}

void Independant::Respawn()
{

}

void Independant::damage_from (Independant& independant)
{
	if (!immune)
	{
		if (independant.damage > shield)
		{
			armor -= (independant.damage - shield);
			shield = 0;
		}
		else
		{
			shield -= independant.damage;
		}
	}
}

int Independant::getIndependantDamage()
{
	return damage;
}

int Independant::getIndependantShield()
{
	return shield;
}

int Independant::getIndependantArmor()
{
	return armor;
}
	
sf::Vector2f Independant::getIndependantSpeed()
{
	return sf::Vector2f(speed.x, speed.y);
}

bool Independant::collide_with (const Independant& independant)
{
	// If not visibe, let's not even bother with the collision
	if (!visible || !(independant.visible))
		return false;

	if (!isOnScene || !(independant.isOnScene))
		return false;
	/*
	// Some types of independant don't colide with each others
	// 0=bg; 1=ship ; 2=friendly fire ; 3= enemy fire ; 4=enemy;	
	else if (   (independant.collider_type==IndependantType::Background || collider_type==IndependantType::Background ) 

		|| (independant.collider_type==IndependantType::PlayerShip && (collider_type==IndependantType::Background || collider_type==IndependantType::FriendlyFire)) 

		|| (independant.collider_type==IndependantType::FriendlyFire && (collider_type==IndependantType::Background || collider_type==IndependantType::PlayerShip || collider_type==IndependantType::EnemyFire)) 

		|| (independant.collider_type==IndependantType::EnemyFire && (collider_type==IndependantType::Background || collider_type==IndependantType::FriendlyFire || collider_type==IndependantType::EnemyObject  || collider_type==IndependantType::EnemyFire)) 

		|| (independant.collider_type==IndependantType::EnemyObject && (collider_type==IndependantType::Background || collider_type==IndependantType::FriendlyFire  || collider_type==IndependantType::EnemyObject || collider_type==IndependantType::EnemyFire))	)		

		return false;
	*/
	// Preliminary test : are the two sprites even aligned ?
	if (independant.getPosition().x - (independant.m_size.x/2) > getPosition().x + (m_size.x/2)

		|| independant.getPosition().x + (independant.m_size.x/2) < getPosition().x - (m_size.x/2)


		|| independant.getPosition().y + (independant.m_size.y/2) < getPosition().y - (m_size.y/2)


		|| independant.getPosition().y - (independant.m_size.y/2) > getPosition().y + (m_size.y/2))
		return false;                        

	// Second test : are the corners included in the other sprite, or vice versa ?
	else
	{
		if ( (   ( (getPosition().x - (m_size.x/2) < independant.getPosition().x - (independant.m_size.x/2)) && (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // up-left corner
			&& (getPosition().y - (m_size.y/2) < independant.getPosition().y - (independant.m_size.y/2)) && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) )

			||  ( (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2)) && (independant.getPosition().x + (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // up-right corner
			&& ( getPosition().y - (m_size.y/2) < independant.getPosition().y - (independant.m_size.y/2)) && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) )

			||  ( (getPosition().x - (m_size.x/2) < independant.getPosition().x - (independant.m_size.x/2)) && (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // down-left corner
			&& (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) && (independant.getPosition().y + (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) )

			||  ( (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2)) && (independant.getPosition().x + (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // down-right corner
			&& (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) && (independant.getPosition().y + (independant.m_size.y/2) < getPosition().y + (m_size.y/2)))  )

			||

			(  ((independant.getPosition().x - (independant.m_size.x/2) < getPosition().x - (m_size.x/2)) && (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))   // up-left corner
			&& (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y - (m_size.y/2)) && (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) )
			//return true;
			|| ( (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2)) && (getPosition().x + (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))  // up-right corner
			&& (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y - (m_size.y/2)) && (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) )
			//return true;
			|| ( (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x - (m_size.x/2)) && (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))   // down-left corner
			&& (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) && (getPosition().y + (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) )
			//return true;
			|| ( (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2)) && (getPosition().x + (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))   // down-right corner
			&& (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) && (getPosition().y + (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)))  )  )

			return true;
		else
			return false;
	}
	return false;
}