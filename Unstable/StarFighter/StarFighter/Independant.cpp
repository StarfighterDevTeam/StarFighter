#include "Independant.h"

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber, int m_animationNumber) : AnimatedSprite()
{
	Init(position, speed, textureName, size, m_frameNumber, m_animationNumber);
	this->setOrigin(origin.x, origin.y);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin) : AnimatedSprite()
{
	Init(position, speed, textureName, size);
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
	this->transparent = false;
	this->visible = false;
	this->immune = false;
	this->ghost = false;
	this->isOnScene = false;
	this->GarbageMe = false;
	this->DontGarbageMe = false;
	this->disable_fire = false;
	this->wake_up = true;
	this->rotation_speed = 0.f;
	this->face_target = false;
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
	for (int j = 0; j < this->defaultAnimation.getSize(); j++)
	{
		int n = j / this->frameNumber;
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
	this->immune = false;
	this->startPattern = false;
	this->GarbageMe = false;
	this->DontGarbageMe = false;
	this->money = 0;
	this->hasEquipmentLoot = false;
	this->hasWeaponLoot = false;
	this->diag = sqrt(pow(m_size.x / 2, 2) + pow(m_size.y / 2, 2));
	this->transparent = false;
	this->ghost = false;
	this->rotation_speed = 0.f;
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
	//todo ?

}

void Independant::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition, offset;

	//Basic movement (initial vector)
	newposition.x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();

	//call bobbyPattern 
		//-> NOP, you need to override it, otherwise no pattern! (because you need CurrentGame->direction, which is not available for a generic Independant item)
	//offset = Pattern.GetOffset(deltaTime.asSeconds());
	//offset = Independant::getSpeed_for_Direction( (*CurrentGame).direction, offset);
	//newposition.x += offset.x;
	//newposition.y += offset.y;

	this->setPosition(newposition.x, newposition.y);

	AnimatedSprite::update(deltaTime);
}

void Independant::updateAnimation(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}

void Independant::setVisible(bool m_visible)
{
	this->visible = m_visible;
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

void Independant::get_money_from(Independant& independant)
{
	int loot_value = independant.getMoney();//give all the money
	money += loot_value;
	independant.addMoney(-loot_value);
}

void Independant::get_money_from(Independant& independant, int loot_value)
{
	money += loot_value;
	independant.addMoney(-loot_value);
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
	clone->Init(clone->getPosition(), clone->speed, clone->textureName, clone->m_size);

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

void Independant::GetLoot(Independant& independant)
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

void Independant::GetPortal(Independant* independant)
{

}


void Independant::get_equipment_from(Independant& independant)
{
	if (independant.hasEquipmentLoot)
	{
		this->setEquipmentLoot(independant.getEquipmentLoot());
		independant.releaseEquipmentLoot();
	}
}

void Independant::setEquipmentLoot(Equipment* equipment)
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

void Independant::get_weapon_from(Independant& independant)
{
	if (independant.hasWeaponLoot)
	{
		this->setWeaponLoot(independant.getWeaponLoot());
		independant.releaseWeaponLoot();
	}
}

void Independant::setWeaponLoot(Weapon* weapon)
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

sf::Vector2f Independant::getSpeed_for_Scrolling(Directions direction, float vspeed)
{
	sf::Vector2f speed = sf::Vector2f(0, 0);

	if (direction == Directions::DIRECTION_UP)
	{
		speed.y = vspeed;
	}
	else if (direction == Directions::DIRECTION_DOWN)
	{
		speed.y = -vspeed;
	}
	else if (direction == Directions::DIRECTION_RIGHT)
	{
		speed.x = -vspeed;
	}
	else if (direction == Directions::DIRECTION_LEFT)
	{
		speed.x = vspeed;
	}

	return speed;
}

sf::Vector2f Independant::getSpeed_to_LocationWhileSceneSwap(Directions current_direction, Directions future_direction, float vspeed, sf::Vector2f sprite_position)
{
	sf::Vector2f speed = sf::Vector2f(0, 0);

	sf::Vector2f future_pos = Independant::getPosition_for_Direction(future_direction, sf::Vector2f((SCENE_SIZE_X*STARTSCENE_X_RATIO), (SCENE_SIZE_Y*STARTSCENE_Y_RATIO)));
	if (future_direction == Directions::NO_DIRECTION)
	{
		future_pos = Independant::getPosition_for_Direction(future_direction, sf::Vector2f((SCENE_SIZE_X*STARTSCENE_X_RATIO), (SCENE_SIZE_Y*STARTSCENE_X_RATIO)));
	}

	if (current_direction == Directions::DIRECTION_UP || current_direction == Directions::DIRECTION_DOWN
		|| future_direction == Directions::DIRECTION_UP || future_direction == Directions::DIRECTION_DOWN)
	{
		speed.x = vspeed * ((future_pos.x) - sprite_position.x) / SCENE_SIZE_Y;
		speed.y = vspeed * ((future_pos.y) - sprite_position.y) / SCENE_SIZE_Y;
	}

	else if (current_direction == Directions::DIRECTION_RIGHT || current_direction == Directions::DIRECTION_LEFT
		|| future_direction == Directions::DIRECTION_RIGHT || future_direction == Directions::DIRECTION_LEFT)
	{
		speed.x = vspeed * ((future_pos.x) - sprite_position.x) / SCENE_SIZE_X;
		speed.y = vspeed * ((future_pos.y) - sprite_position.y) / SCENE_SIZE_X;
	}

	return speed;
}

sf::Vector2i Independant::getDirectionMultiplier(Directions direction)
{
	int x = 1;
	int y = 1;
	if (direction == Directions::DIRECTION_DOWN || direction == Directions::DIRECTION_RIGHT)
	{
		x *= -1;
	}
	if (direction == Directions::DIRECTION_DOWN || direction == Directions::DIRECTION_LEFT)
	{
		y *= -1;
	}

	return sf::Vector2i(x, y);
}

sf::Vector2f Independant::getSize_for_Direction(Directions direction, sf::Vector2f size)
{
	if (direction == Directions::DIRECTION_LEFT || direction == Directions::DIRECTION_RIGHT)
	{
		return sf::Vector2f(size.y, size.x);
	}
	else
	{
		return size;
	}
}

sf::Vector2i Independant::getSize_for_Direction(Directions direction, sf::Vector2i size)
{
	if (direction == Directions::DIRECTION_LEFT || direction == Directions::DIRECTION_RIGHT)
	{
		return sf::Vector2i(size.y, size.x);
	}
	else
	{
		return size;
	}
}

sf::Vector2f Independant::getSpeed_for_Direction(Directions direction, sf::Vector2f speed)
{
	speed = Independant::getSize_for_Direction(direction, sf::Vector2f(speed.x, speed.y));

	if (direction == DIRECTION_DOWN)
	{
		speed.x = -speed.x;
		speed.y = -speed.y;
	}

	if (direction == DIRECTION_RIGHT)
	{
		speed.x = -speed.x;
		speed.y = speed.y;
	}

	if (direction == DIRECTION_LEFT)
	{
		speed.x = speed.x;
		speed.y = -speed.y;
	}

	return sf::Vector2f(speed.x, speed.y);
}

float Independant::getRotation_for_Direction(Directions direction)
{
	if (direction == Directions::DIRECTION_DOWN)
	{
		return 180;
	}
	else if (direction == Directions::DIRECTION_RIGHT)
	{
		return 90;
	}
	else if (direction == Directions::DIRECTION_LEFT)
	{
		return 270;
	}
	else
	{
		return 0;
	}
}

sf::Vector2f Independant::getPosition_for_Direction(Directions direction, sf::Vector2f position, bool rescale)
{
	float x = position.x;
	float y = position.y;

	if (direction == Directions::DIRECTION_DOWN)
	{
		x = SCENE_SIZE_X - x;
		y = SCENE_SIZE_Y - y;
	}
	else if (direction == Directions::DIRECTION_RIGHT)
	{
		if (rescale)
		{
			x = x * SCENE_SIZE_Y / SCENE_SIZE_X;
			y = y * SCENE_SIZE_X / SCENE_SIZE_Y;
		}
		float x_ = x;
		x = SCENE_SIZE_X - y;
		y = x_;
	}
	else if (direction == Directions::DIRECTION_LEFT)
	{
		if (rescale)
		{
			x = x * SCENE_SIZE_Y / SCENE_SIZE_X;
			y = y * SCENE_SIZE_X / SCENE_SIZE_Y;
		}
		float x_ = x;
		x = y;
		y = SCENE_SIZE_Y - x_;
	}

	return sf::Vector2f(x, y);
}

FloatCompare Independant::compare_posY_withTarget_for_Direction(Directions direction, sf::Vector2f target_position)
{
	if (direction == Directions::DIRECTION_UP)
	{
		if (this->getPosition().y > target_position.y)
		{
			return FloatCompare::GREATHER_THAN;
		}
		else if (this->getPosition().y == target_position.y)
		{
			return FloatCompare::EQUAL_TO;
		}
		else
		{
			return FloatCompare::LESSER_THAN;
		}
	}

	else if (direction == Directions::DIRECTION_DOWN)
	{
		if (this->getPosition().y < SCENE_SIZE_Y - target_position.y)
		{
			return FloatCompare::GREATHER_THAN;
		}
		else if (this->getPosition().y == SCENE_SIZE_Y - target_position.y)
		{
			return FloatCompare::EQUAL_TO;
		}
		else
		{
			return FloatCompare::LESSER_THAN;
		}
	}

	else if (direction == Directions::DIRECTION_RIGHT)
	{
		if (this->getPosition().x < SCENE_SIZE_X - target_position.x)
		{
			return FloatCompare::GREATHER_THAN;
		}
		if (this->getPosition().x == SCENE_SIZE_X - target_position.x)
		{
			return FloatCompare::EQUAL_TO;
		}
		else
		{
			return FloatCompare::LESSER_THAN;
		}
	}

	else if (direction == Directions::DIRECTION_LEFT)
	{
		if (this->getPosition().x > target_position.x)
		{
			return FloatCompare::GREATHER_THAN;
		}
		else if (this->getPosition().x == target_position.x)
		{
			return FloatCompare::EQUAL_TO;
		}
		else
		{
			return FloatCompare::LESSER_THAN;
		}
	}
}

FloatCompare Independant::compare_posX_withTarget_for_Direction(Directions direction, sf::Vector2f target_position)
{
	{
		if (direction == Directions::DIRECTION_UP)
		{
			if (this->getPosition().x > target_position.x)
			{
				return FloatCompare::GREATHER_THAN;
			}
			else if (this->getPosition().x == target_position.x)
			{
				return FloatCompare::EQUAL_TO;
			}
			else
			{
				return FloatCompare::LESSER_THAN;
			}
		}

		else if (direction == Directions::DIRECTION_DOWN)
		{
			if (this->getPosition().x < SCENE_SIZE_X - target_position.x)
			{
				return FloatCompare::GREATHER_THAN;
			}
			else if (this->getPosition().x == SCENE_SIZE_X - target_position.x)
			{
				return FloatCompare::EQUAL_TO;
			}
			else
			{
				return FloatCompare::LESSER_THAN;
			}
		}

		else if (direction == Directions::DIRECTION_RIGHT)
		{
			if (this->getPosition().y > target_position.y)
			{
				return FloatCompare::GREATHER_THAN;
			}
			if (this->getPosition().y == target_position.y)
			{
				return FloatCompare::EQUAL_TO;
			}
			else
			{
				return FloatCompare::LESSER_THAN;
			}
		}

		else if (direction == Directions::DIRECTION_LEFT)
		{
			if (this->getPosition().y < SCENE_SIZE_Y - target_position.y)
			{
				return FloatCompare::GREATHER_THAN;
			}
			else if (this->getPosition().y == SCENE_SIZE_Y - target_position.y)
			{
				return FloatCompare::EQUAL_TO;
			}
			else
			{
				return FloatCompare::LESSER_THAN;
			}
		}
	}
}

sf::Vector2f Independant::setPosition_Y_for_Direction(Directions direction, sf::Vector2f target_position, bool centered)
{
	if (direction == Directions::DIRECTION_UP || direction == Directions::NO_DIRECTION)
	{
		if (!centered)
		{
			this->setPosition(sf::Vector2f(this->getPosition().x, target_position.y));
			return sf::Vector2f(this->getPosition().x, target_position.y);
		}
		else
		{
			this->setPosition(sf::Vector2f(target_position.x, target_position.y));
			return sf::Vector2f(target_position.x, target_position.y);
		}
	}

	else if (direction == Directions::DIRECTION_DOWN)
	{
		if (!centered)
		{
			this->setPosition(sf::Vector2f(this->getPosition().x, SCENE_SIZE_Y - target_position.y));
			return sf::Vector2f(this->getPosition().x, SCENE_SIZE_Y - target_position.y);
		}
		else
		{
			this->setPosition(sf::Vector2f(target_position.x, SCENE_SIZE_Y - target_position.y));
			return sf::Vector2f(target_position.x, SCENE_SIZE_Y - target_position.y);
		}
	}

	else if (direction == Directions::DIRECTION_RIGHT)
	{
		if (!centered)
		{
			this->setPosition(sf::Vector2f(SCENE_SIZE_X - target_position.x, this->getPosition().y));
			return sf::Vector2f(SCENE_SIZE_X - target_position.x, this->getPosition().y);
		}
		else
		{
			this->setPosition(sf::Vector2f(SCENE_SIZE_X - target_position.y, target_position.x));
			return sf::Vector2f(SCENE_SIZE_X - target_position.y, target_position.x);
		}
	}

	else if (direction == Directions::DIRECTION_LEFT)
	{
		if (!centered)
		{
			this->setPosition(sf::Vector2f(target_position.x, this->getPosition().y));
			return sf::Vector2f(target_position.x, this->getPosition().y);
		}
		else
		{
			this->setPosition(sf::Vector2f(target_position.y, target_position.x));
			return sf::Vector2f(target_position.y, target_position.x);
		}
	}
}