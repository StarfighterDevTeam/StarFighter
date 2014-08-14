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
	this->initial_position = sf::Vector2f(position.x,position.y);
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
	this->GarbageMe = false;
	this->DontGarbageMe = false;
	this->money=0;
	this->hasEquipmentLoot = false;
	this->hasWeaponLoot = false;
	this->diag = sqrt(pow(m_size.x/2,2) + pow(m_size.y/2,2));
	this->transparent = false;
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

Independant::~Independant()
{
	//todo ?

}

void Independant::update(sf::Time deltaTime)
{

	static sf::Vector2f newposition, offset;

	//Basic movement (initial vector)
	newposition.x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();

	//call bobbyPattern
	offset = Pattern.GetOffset(deltaTime.asSeconds());
	newposition.x += offset.x;
	newposition.y += offset.y;

	this->setPosition(newposition.x,newposition.y);

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

//void Independant::Follow(Independant* target)
//{
//	this->setPosition(target->getPosition().x, target->getPosition().x);
//}

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

void Independant::Death()
{

}

void Independant::GenerateLoot()
{
	
}

void Independant::GetLoot (Independant& independant)
{
	this->get_money_from(independant);

	if (independant.hasEquipmentLoot)
	{
		this->get_equipment_from(independant);
	}
	if (independant.hasWeaponLoot)
	{
		this->get_weapon_from(independant);
	}
}

void Independant::get_equipment_from (Independant& independant)
{
	if (independant.hasEquipmentLoot)
	{
		this->setEquipmentLoot(independant.getEquipmentLoot());
		independant.releaseEquipmentLoot();
	}
}

void Independant::setEquipmentLoot (Equipment* equipment)
{
	this->equipment_loot = equipment;
	this->hasEquipmentLoot = true;
}

Equipment* Independant::getEquipmentLoot()
{
	return this->equipment_loot;
}

void Independant::releaseEquipmentLoot()
{
	this->equipment_loot = NULL;
	free(equipment_loot);
	this->hasEquipmentLoot = false;
}

void Independant::get_weapon_from (Independant& independant)
{
	if (independant.hasWeaponLoot)
	{
		this->setWeaponLoot(independant.getWeaponLoot());
		independant.releaseWeaponLoot();
	}
}

void Independant::setWeaponLoot (Weapon* weapon)
{
	this->weapon_loot = weapon;
	this->hasWeaponLoot = true;
}

Weapon* Independant::getWeaponLoot()
{
	return this->weapon_loot;
}

void Independant::releaseWeaponLoot()
{
	this->weapon_loot = NULL;
	free(weapon_loot);
	this->hasWeaponLoot = false;
}

void Independant::CreateRandomLoot(float BeastScaleBonus)
{
	//-> Enemy::CreateRandomLoot override
}

void Independant::GetGrazing()
{
	//see overide function in class Ship
}