#include "Independant.h"

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber, int m_animationNumber) : AnimatedSprite()
{
	Init(position, speed, textureName, size, m_frameNumber, m_animationNumber);
	this->setOrigin(origin.x, origin.y);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : AnimatedSprite()
{
	Init(position, speed, textureName, size);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture) : AnimatedSprite()
{
	Init(position, speed, texture, 1);
}

string Independant::getName()
{
	vector<string> s1 = TextUtils::split(this->textureName, '/');
	return *(s1.end() - 1);
}

Independant::Independant()
{

}

void Independant::setAnimationLine(int m_animation, bool keep_frame_index)
{
	//bulletproof verifications
	if (m_animation >= this->animationNumber)
	{
		printf("Requesting an animation line (%d) that exceeds what is allowed (%d) for this item", m_animation, this->animationNumber);
		m_animation = this->animationNumber - 1;
		if (m_animation < 0)
		{
			m_animation = 0;
		}
	}

	//now let's load the new animation
	Animation* anim = new Animation();
	anim->setSpriteSheet(*this->defaultAnimation.getSpriteSheet());
	for (size_t j = 0; j < this->defaultAnimation.getSize(); j++)
	{
		size_t n = j / this->frameNumber;
		//when we have reached out to the correct line of animation frames, we put this line into the animation
		if (n == m_animation)
		{
			anim->addFrame(this->defaultAnimation.getFrame(j));
		}
	}

	if (!keep_frame_index)
	{
		this->m_currentFrame = 0;
	}

	this->currentAnimation = anim;
	this->play(*currentAnimation);
	this->currentAnimationIndex = m_animation;
}

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int m_frameNumber, int m_animationNumber)
{
	this->animationNumber = m_animationNumber;
	this->frameNumber = m_frameNumber;
	this->initial_position = sf::Vector2f(position.x, position.y);
	this->m_size.x = ((*texture).getSize().x / m_frameNumber);
	this->m_size.y = ((*texture).getSize().y / m_animationNumber);

	this->collider_type = IndependantType::BackgroundObject;
	this->defaultAnimation.setSpriteSheet(*texture);
	for (int j = 0; j < m_animationNumber; j++)
	{
		for (int i = 0; i < m_frameNumber; i++)
		{
			int x = ((*texture).getSize().x / m_frameNumber)*(i);
			int y = ((*texture).getSize().y / m_animationNumber)*(j);
			this->defaultAnimation.addFrame(sf::IntRect(x, y, this->m_size.x, this->m_size.y));
		}
	}
	
	this->setAnimationLine(0);//default starting animation is line 0 (top of the sprite sheet)
	
	this->speed = speed;
	this->setPosition(position.x, position.y);
	this->visible = false;
	this->isOnScene = false;
	this->GarbageMe = false;
	this->DontGarbageMe = false;
	this->money = 0;
	this->diag = (float)sqrt(((m_size.x / 2)*(m_size.x / 2)) + ((m_size.y / 2)*(m_size.y / 2)));
	this->ghost = false;
	this->rotation_speed = 0.f;
	this->equipment_loot = NULL;
	this->weapon_loot = NULL;
}

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*m_frameNumber, size.y*m_animationNumber);
	this->textureName = textureName;

	this->setOrigin(size.x / 2, size.y / 2);

	Init(position, speed, texture, m_frameNumber, m_animationNumber);

}

Independant::~Independant()
{
	//TODO

	//delete this->equipment_loot;
	//delete this->weapon_loot;
}

void Independant::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = this->speed;

	if (hyperspeedMultiplier < 1)
	{
		newspeed.x = this->speed.x * hyperspeedMultiplier;
		newspeed.y = this->speed.y * hyperspeedMultiplier;
	}
	
	this->setGhost(hyperspeedMultiplier > 1.0f);
	
	//Basic movement (initial vector)
	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	this->setPosition(newposition.x, newposition.y);

	AnimatedSprite::update(deltaTime);
}

void Independant::updateAnimation(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}

void Independant::Respawn()
{

}

void Independant::setGhost(bool m_ghost)
{
	if (m_ghost == true)
	{
		this->ghost = true;
		this->setColor(Color(255, 255, 255, GHOST_ALPHA_VALUE));
	}
	else
	{
		this->ghost = false;
		this->setColor(Color(255, 255, 255, 255));
	}
	
}
//void Independant::Follow(Independant* target)
//{
//	this->setPosition(target->getPosition().x, target->getPosition().x);
//}

void Independant::damage_from(Independant& independant)
{
	setColor(Color(255,0,0,255), sf::seconds(DAMAGE_FEEDBACK_TIME));
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

bool Independant::get_money_from(Independant& independant)
{
	int loot_value = independant.getMoney();//give all the money
	money += loot_value;
	independant.addMoney(-loot_value);
	if (loot_value > 0)
		return true;
	else
		return false;
}

bool Independant::get_money_from(Independant& independant, int loot_value)
{
	money += loot_value;
	independant.addMoney(-loot_value);
	if (loot_value > 0)
		return true;
	else
		return false;
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

int Independant::getIndependantArmorMax()
{
	return armor_max;
}

sf::Vector2f Independant::getIndependantSpeed()
{
	return sf::Vector2f(speed.x, speed.y);
}

Independant* Independant::Clone()
{
	Independant* clone = new Independant(this->getPosition(), this->speed, this->textureName, this->m_size);
	clone->collider_type = this->collider_type;
	clone->layer = this->layer;

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

void Independant::Destroy()
{

}

bool Independant::get_equipment_from(Independant& independant)
{
	if (independant.equipment_loot != NULL && this->equipment_loot == NULL)
	{
		this->equipment_loot = independant.getEquipmentLoot();
		independant.equipment_loot = NULL;
		return true;
	}
	else
	{
		return false;
	}
}

bool Independant::setEquipmentLoot(Equipment* equipment)
{
	if (this->equipment_loot == NULL)
	{
		this->equipment_loot = equipment;
		return true;
	}
	else
	{
		return false;
	}
}

Equipment* Independant::getEquipmentLoot()
{
	return this->equipment_loot;
}

void Independant::releaseEquipmentLoot()
{
	this->equipment_loot = NULL;
}

bool Independant::get_weapon_from(Independant& independant)
{
	if (independant.weapon_loot != NULL && this->weapon_loot == NULL)
	{
		this->weapon_loot = independant.getWeaponLoot();
		independant.weapon_loot = NULL;
		return true;
	}
	else
	{
		return false;
	}
}

bool Independant::setWeaponLoot(Weapon* weapon)
{
	if (this->weapon_loot == NULL)
	{
		this->weapon_loot = weapon;
		return true;
	}
	else
	{
		return false;
	}
}

Weapon* Independant::getWeaponLoot()
{
	return this->weapon_loot;
}

void Independant::releaseWeaponLoot()
{
	this->weapon_loot = NULL;
}