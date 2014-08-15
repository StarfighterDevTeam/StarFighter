#include "Independant.h"


Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber) : AnimatedSprite()
{
	Init(position, speed, textureName, size, frameNumber);
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

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber)
{
	this->initial_position = sf::Vector2f(position.x, position.y);
	this->m_size.x = ((*texture).getSize().x / frameNumber);
	this->m_size.y = ((*texture).getSize().y);

	this->collider_type = IndependantType::Background;
	this->defaultAnimation.setSpriteSheet(*texture);
	for (int i = 0; i < frameNumber; i++)
	{
		int x = ((*texture).getSize().x / frameNumber)*(i);
		this->defaultAnimation.addFrame(sf::IntRect(x, 0, this->m_size.x, this->m_size.y));
	}
	this->currentAnimation = &defaultAnimation;
	this->play(*currentAnimation);

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
}

void Independant::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*frameNumber, size.y);
	this->textureName = textureName;

	this->setOrigin(size.x / 2, size.y / 2);

	Init(position, speed, texture, frameNumber);

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

float Independant::getPosition_from_SceneBorderOffset(Directions direction, sf::Vector2f offset, bool outside_screen)
{
	int reverse = 0;
	int opposite = 1;

	if (direction == Directions::DIRECTION_DOWN || direction == Directions::DIRECTION_RIGHT)
	{
		reverse = 1;
		opposite *= -1;
	}

	if (!outside_screen)
	{
		opposite *= -1;
	}

	if (direction == Directions::DIRECTION_UP || direction == Directions::DIRECTION_DOWN)
	{
		float y = 0;
		y = (reverse * SCENE_SIZE_Y) - (opposite*offset.y);
		return y;
	}
	else if (direction == Directions::DIRECTION_RIGHT || direction == Directions::DIRECTION_LEFT)
	{
		float x = 0;
		x = (reverse * SCENE_SIZE_X) - (opposite*offset.x);
		return x;
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error in Independant::compareCoordinate_to_ScreenBorder. Direction given in argument cannot be (0,0)\n");
		return 0;
	}
}

bool Independant::isPositionPastDistance_to_ScreenBorder(Directions direction, sf::Vector2f coordinates, sf::Vector2f sprite_position, bool outside_scene, bool player_side)
{
	float pos = 0;
	float coor = 0;
	int offset = 0;
	int opposite = 1;

	if (player_side)
	{
		opposite *= -1;
	}

	if (direction == Directions::DIRECTION_UP)
	{
		pos = opposite*sprite_position.y;
		coor = opposite*Independant::getPosition_from_SceneBorderOffset(direction, coordinates, outside_scene);
		coor += offset*Independant::getFirstSceneOffset(direction).y;
	}
	else if (direction == Directions::DIRECTION_DOWN)
	{
		pos = opposite * (-sprite_position.y);
		coor = opposite * (-Independant::getPosition_from_SceneBorderOffset(direction, coordinates, outside_scene));
		coor += offset*Independant::getFirstSceneOffset(direction).y;
	}
	else if (direction == Directions::DIRECTION_RIGHT)
	{
		pos = opposite * (-sprite_position.x);
		coor = opposite * (-Independant::getPosition_from_SceneBorderOffset(direction, coordinates, outside_scene));
		coor += offset*Independant::getFirstSceneOffset(direction).x;
	}
	else if (direction == Directions::DIRECTION_LEFT)
	{
		pos = opposite * sprite_position.x;
		coor = opposite * Independant::getPosition_from_SceneBorderOffset(direction, coordinates, outside_scene);
		coor += offset*Independant::getFirstSceneOffset(direction).x;
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error in Independant::isPositionPastDistance_to_ScreenBorder. Direction given in argument cannot be (0,0)\n");
	}

	if (pos >= coor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float Independant::getPosition_on_PerpendicularAxis(Directions direction, bool centered, bool random, sf::Vector2f left_margin_to_screen_border, sf::Vector2f right_margin_to_screen_border)
{

	float mid = 0;
	if (direction == Directions::DIRECTION_UP || direction == Directions::DIRECTION_DOWN)
	{
		mid = SCENE_SIZE_X / 2;
	}
	else if (direction == Directions::DIRECTION_RIGHT || direction == Directions::DIRECTION_LEFT)
	{
		mid = SCENE_SIZE_Y / 2;
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error in Independant::getPosition_on_PerpendicularAxis. Direction given in argument cannot be (0,0)\n");
	}

	//case: centered
	if (centered)
	{
		return mid;
	}

	//case: random
	else if (random)
	{
		float pos = 0;
		if (direction == Directions::DIRECTION_UP || direction == Directions::DIRECTION_DOWN)
		{
			float min_marker = left_margin_to_screen_border.x;
			float max_marker = SCENE_SIZE_X - right_margin_to_screen_border.x;

			pos = RandomizeFloatBetweenValues(sf::Vector2f(min_marker, max_marker));

			return pos;
		}
		else if (direction == Directions::DIRECTION_RIGHT || direction == Directions::DIRECTION_LEFT)
		{
			float min_marker = left_margin_to_screen_border.y;
			float max_marker = SCENE_SIZE_Y - right_margin_to_screen_border.y;

			pos = RandomizeFloatBetweenValues(sf::Vector2f(min_marker, max_marker));

			return pos;
		}
		else
		{
			LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error in Independant::getPosition_on_PerpendicularAxis. Direction given in argument cannot be (0,0)\n");
		}

		return pos;
	}

	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error in Independant::getPosition_on_PerpendicularAxis. Please specify a true boolean: centered, or random distribution on the axis.\n");
		return 0;
	}
}

sf::Vector2f Independant::getSpeed_for_Scrolling(Directions direction, float vspeed, bool player_side)// /!\ Direction up means the sprite is scrolling down, unless it's playerside = true
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

	if (player_side)
	{
		speed.x *= -1;
		speed.y *= -1;
	}
	return speed;
}

sf::Vector2f Independant::getFirstSceneOffset(Directions direction)
{
	sf::Vector2f offset = sf::Vector2f(0, 0);

	if (direction == Directions::DIRECTION_UP)
	{
		offset.y = SCENE_SIZE_Y;
	}
	else if (direction == Directions::DIRECTION_DOWN)
	{
		offset.y = -SCENE_SIZE_Y;
	}
	else if (direction == Directions::DIRECTION_RIGHT)
	{
		offset.x = -SCENE_SIZE_X;
	}
	else if (direction == Directions::DIRECTION_LEFT)
	{
		offset.x = SCENE_SIZE_X;
	}

	return offset;
}

sf::Vector2f Independant::getCoordinates_for_Spawn(bool first_scene, Directions direction, sf::Vector2f coordinates, bool outside_screen, bool centered,
	bool keep_perpendicular_axis, sf::Vector2f position, bool random, sf::Vector2f left_margin_to_screen_border, sf::Vector2f right_margin_to_screen_border)
{
	sf::Vector2f pos = sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2);

	if (direction != Directions::NO_DIRECTION)
	{
		float x = Independant::getPosition_on_PerpendicularAxis(direction, centered, random, left_margin_to_screen_border, right_margin_to_screen_border);
		float y = Independant::getPosition_from_SceneBorderOffset(direction, coordinates, outside_screen);

		if (direction == Directions::DIRECTION_UP || direction == Directions::DIRECTION_DOWN)
		{
			pos = sf::Vector2f(x, y);
			if (keep_perpendicular_axis)
			{
				pos.x = position.x;
			}
		}
		else if (direction == Directions::DIRECTION_RIGHT || direction == Directions::DIRECTION_LEFT)
		{
			pos = sf::Vector2f(y, x);
			if (keep_perpendicular_axis)
			{
				pos.y = position.y;
			}
		}

		if (first_scene)
		{
			pos.x += Independant::getFirstSceneOffset(direction).x;
			pos.y += Independant::getFirstSceneOffset(direction).y;
		}
	}


	return pos;
}

sf::Vector2f Independant::getSpeed_to_LocationWhileSceneSwap(Directions current_direction, Directions future_direction, float vspeed, sf::Vector2f sprite_position)
{
	sf::Vector2f speed = sf::Vector2f(0, 0);

	sf::Vector2f future_pos = Independant::getCoordinates_for_Spawn(false, future_direction, sf::Vector2f((SCENE_SIZE_X*STARTSCENE_Y_RATIO), (SCENE_SIZE_Y*STARTSCENE_Y_RATIO)), false, true);

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

sf::Vector2f Independant::getSpeed_for_Direction(Directions direction, sf::Vector2f size, bool player_side)
{
	float x = getSize_for_Direction(direction, size).x;
	float y = getSize_for_Direction(direction, size).y;

	if (direction == Directions::DIRECTION_DOWN || direction == Directions::DIRECTION_RIGHT)
	{
		x *= -1;
		y *= -1;
	}

	if (player_side)
	{
		x *= -1;
		y *= -1;
	}

	return sf::Vector2f(x, y);
}

sf::Vector2i Independant::getFireDirection(Directions direction, bool player_side)
{
	int dirY = 1;
	int dirX = 0;

	if (player_side)
	{
		dirY *= -1;
	}

	if (direction == Directions::DIRECTION_DOWN || direction == Directions::DIRECTION_LEFT)
	{
		dirY *= -1;
	}

	if (direction == Directions::DIRECTION_RIGHT)
	{
		dirX = -dirY;
		dirY = 0;
	}

	return sf::Vector2i(dirX, dirY);
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