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

string Independant::getName()
{
	vector<string> s1 = TextUtils::split(this->textureName,'/');
	return *(s1.end()-1);
}

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber)
{
	this->movePattern = MovePatternType::NoMovePattern;
	this->initial_position = sf::Vector2f(position.x,position.y);
	this->pastTime = sf::Time();
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
	this->startPattern = false;

	money=0;
}

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*frameNumber, size.y);
	this->textureName = textureName;

	this->setOrigin(size.x/2, size.y/2);

	Init(position,speed,texture,frameNumber);

}

void Independant::ResetInitialPosition()
{
	if(this->collider_type == 6)
	{
		sf::Vector2f p = this->getPosition();
		this->initial_position.x = p.x;
		this->initial_position.y = p.y;
	}
}

Independant::~Independant()
{
	//Unload Texture

}

void Independant::update(sf::Time deltaTime)
{
	this->pastTime += deltaTime;

	sf::Vector2f np;
	//Basic movements
	np.x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
	np.y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();

	//TODO: check move patterns, etc
	GetPolarMovement(&np);


	this->setPosition(np.x,np.y);

	AnimatedSprite::update(deltaTime);
}

void Independant::GetPolarMovement(sf::Vector2f* np)
{
	float t = this->pastTime.asSeconds();
	switch(this->movePattern)
	{
	case MovePatternType::NoMovePattern:
		{
			//Nothing to do
			break;
		}
	case MovePatternType::_100Cos15:
		{
			//Specific patterns
			np->x = this->initial_position.x + 150*cos(1.5*t);
			break;
		}
	case MovePatternType::StarFish:
		{
			//Specific patterns
			np->x = this->initial_position.x + 75*cos(t);
			np->y += 0.5*(cos(2*t) - 5*cos(t));
			break;
		}
	default:
		{
			throw invalid_argument(TextUtils::format("Game erro: Unknow pattern # '%d'", this->movePattern));
		}
	}


}

/*
sf::Vector2f Independant::setMovePattern(float angle_rad, float radius, float triggerY, int pattern_id)//when y > triggerY, move pattern begins
{
if (getPosition().y > triggerY && !startPattern) //&& collider_type == IndependantType::EnemyObject)
{
startPattern = true;
}
if (startPattern)
{
switch(pattern_id)
{
case MovePatternType::NoMove:
{
return sf::Vector2f(0,0);
break;
}

case MovePatternType::SemiCircleDown:
{
float posX= radius*sin(angle_rad);
float posY= radius*sin(angle_rad)*cos(angle_rad);
//float posX= radius*cos(angle_rad);
//float posY= radius*sin(angle_rad);
speed.x=0;
speed.y=0;
return sf::Vector2f(posX,posY);
break;
}

case MovePatternType::Circle:
{
float posX= radius*cos(angle_rad);
float posY= radius*sin(angle_rad);
speed.x=0;
speed.y=0;
return sf::Vector2f(posX,posY);
break;
}

default:
{
return sf::Vector2f(0,0);
}
}
}
else
{
return sf::Vector2f(0,0);
}
}
*/

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

void Independant::get_money_from (Independant& independant)
{
	int loot_value = independant.getMoney();//give all the money
	money += loot_value;
	independant.addMoney(- loot_value);
}

void Independant::get_money_from (Independant& independant, int loot_value)
{
	money += loot_value;
	independant.addMoney(- loot_value);
}

int Independant::getIndependantDamage()
{
	return damage;
}

int Independant::getIndependantShield()
{
	return shield;
}

int Independant::getIndependantShieldMax()
{
	return shield_max;
}

int Independant::getIndependantShieldRegen()
{
	return shield_regen;
}

int Independant::getIndependantArmor()
{
	return armor;
}

sf::Vector2f Independant::getIndependantSpeed()
{
	return sf::Vector2f(speed.x, speed.y);
}

Independant* Independant::Clone()
{
	Independant* clone = new Independant(this->getPosition(), this->speed, this->textureName,this->m_size);
	clone->Init(clone->getPosition(),clone->speed,clone->textureName,clone->m_size);

	return clone;
}

int Independant::getMoney()
{
	return money;
}

void Independant::addMoney(int loot_value)
{
	money += loot_value;
}

void Independant::setMoney(int loot_value)
{
	money = loot_value;
}