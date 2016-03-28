#include "GameObject.h"

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : AnimatedSprite()
{
	Init(position, speed, textureName, size, frameNumber, animationNumber);
	this->setOrigin(origin.x, origin.y);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : AnimatedSprite()
{
	Init(position, speed, textureName, size);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture) : AnimatedSprite()
{
	Init(position, speed, texture, 1);
}

string GameObject::getName()
{
	vector<string> s1 = TextUtils::split(m_textureName, '/');
	return *(s1.end() - 1);
}

GameObject::GameObject()
{

}

void GameObject::Draw(sf::RenderTexture& screen)
{
	if (m_visible && !m_transparent)
	{
		screen.draw(*this);
	}
}

void GameObject::setAnimationLine(int animation, bool keep_frame_index)
{
	//because yeah, if you don't have an animation, this is not happening
	if (m_animationNumber == 0)
	{
		return;
	}

	//are we already playing this animation?
	if (m_currentAnimationIndex == animation && (keep_frame_index || (m_currentAnimation && m_currentAnimation->getSize() == 1)))
	{
		return;
	}

	//bulletproof verifications
	if (animation >= m_animationNumber)
	{
		printf("Requesting an animation line (%d) that exceeds what is allowed (%d) for this item", animation, m_animationNumber);
		animation = m_animationNumber - 1;
		if (animation < 0)
		{
			animation = 0;
		}
	}

	//now let's load the new animation
	Animation* anim = new Animation();
	anim->setSpriteSheet(*m_defaultAnimation.getSpriteSheet());
	for (size_t j = 0; j < m_defaultAnimation.getSize(); j++)
	{
		size_t n = j / m_frameNumber;
		//when we have reached out to the correct line of animation frames, we put this line into the animation
		if (n == animation)
		{
			anim->addFrame(m_defaultAnimation.getFrame(j));
		}
	}

	if (!keep_frame_index)
	{
		m_currentFrame = 0;
	}

	if (m_currentAnimation)
		delete m_currentAnimation;
	m_currentAnimation = anim;
	play(*m_currentAnimation);
	m_currentAnimationIndex = animation;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber, int animationNumber)
{
	m_animationNumber = animationNumber;
	m_frameNumber = frameNumber;
	m_initial_position = sf::Vector2f(position.x, position.y);
	m_size.x = ((*texture).getSize().x / frameNumber);
	m_size.y = ((*texture).getSize().y / animationNumber);

	m_collider_type = BackgroundObject;
	m_defaultAnimation.setSpriteSheet(*texture);
	for (int j = 0; j < m_animationNumber; j++)
	{
		for (int i = 0; i < m_frameNumber; i++)
		{
			int x = ((*texture).getSize().x / frameNumber)*(i);
			int y = ((*texture).getSize().y / animationNumber)*(j);
			m_defaultAnimation.addFrame(sf::IntRect(x, y, m_size.x, m_size.y));
		}
	}
	
	m_currentAnimation = NULL;
	setAnimationLine(0);//default starting animation is line 0 (top of the sprite sheet)
	
	m_speed = speed;
	setPosition(position.x, position.y);
	m_visible = true;
	m_isOnScene = false;
	m_immune = false;
	m_startPattern = false;
	m_GarbageMe = false;
	m_DontGarbageMe = false;
	m_money = 0;
	m_diag = sqrt(((m_size.x / 2)*(m_size.x / 2)) + ((m_size.y / 2)*(m_size.y / 2)));
	m_transparent = false;
	m_ghost = false;
	m_rotation_speed = 0.f;
	m_disable_fire = false;
	m_wake_up = true;
	m_equipment_loot = NULL;
	m_weapon_loot = NULL;
	m_isCollidingWithInteractiveObject = No_Interaction;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*frameNumber, size.y*animationNumber);
	m_textureName = textureName;

	setOrigin(size.x / 2, size.y / 2);

	Init(position, speed, texture, frameNumber, animationNumber);
}

GameObject::~GameObject()
{
	//TODO

	//delete this->equipment_loot;
	//delete this->weapon_loot;
}

void GameObject::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	if (hyperspeedMultiplier < 1)
	{
		newspeed.x = m_speed.x * hyperspeedMultiplier;
		newspeed.y = m_speed.y * hyperspeedMultiplier;
	}
	
	setGhost(hyperspeedMultiplier > 1.0f);
	
	//Basic movement (initial vector)
	newposition.x = getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	setPosition(newposition.x, newposition.y);

	AnimatedSprite::update(deltaTime);
}

void GameObject::updateAnimation(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}

bool GameObject::NormalizeSpeed(sf::Vector2f* vector, float max_value)
{
	if (vector->x == 0 && vector->y == 0)
		return true;

	if (vector->x * vector->x + vector->y * vector->y > max_value * max_value)
	{
		float p = max_value / sqrt(vector->x * vector->x + vector->y * vector->y);
		vector->x *= p;
		vector->y *= p;

		return true;
	}

	return false;
}

void GameObject::Respawn()
{

}

void GameObject::setGhost(bool ghost)
{
	if (ghost == true)
	{
		m_ghost = true;
		setColor(Color(255, 255, 255, GHOST_ALPHA_VALUE));
	}
	else
	{
		m_ghost = false;
		setColor(Color(255, 255, 255, 255));
	}
	
}
//void GameObject::Follow(GameObject* target)
//{
//	this->setPosition(target->getPosition().x, target->getPosition().x);
//}

void GameObject::GetDamageFrom(GameObject& object)
{
	if (!m_immune)
	{
		setColor(Color(255,0,0,255), sf::seconds(DAMAGE_FEEDBACK_TIME));
		if (object.m_damage > m_shield)
		{
			m_armor -= (object.m_damage - m_shield);
			m_shield = 0;
		}
		else
		{
			m_shield -= object.m_damage;
		}		
	}
}

bool GameObject::get_money_from(GameObject& object)
{
	int loot_value = object.m_money;//give all the money
	m_money += loot_value;
	object.addMoney(-loot_value);
	if (loot_value > 0)
		return true;
	else
		return false;
}

bool GameObject::get_money_from(GameObject& object, int loot_value)
{
	m_money += loot_value;
	object.addMoney(-loot_value);
	if (loot_value > 0)
		return true;
	else
		return false;
}

GameObject* GameObject::Clone()
{
	GameObject* clone = new GameObject(this->getPosition(), this->m_speed, this->m_textureName, this->m_size);
	clone->m_display_name = this->m_display_name;
	clone->m_collider_type = this->m_collider_type;
	clone->m_layer = this->m_layer;

	return clone;
}

void GameObject::addMoney(int loot_value)
{
	m_money += loot_value;
}

void GameObject::setMoney(int loot_value)
{
	m_money = loot_value;
}

void GameObject::Death()
{

}

void GameObject::GenerateLoot()
{

}

bool GameObject::GetLoot(GameObject& object)
{
	if (this->get_money_from(object) || this->get_equipment_from(object) || this->get_weapon_from(object))
		return true;
	else
		return false;
	
}

void GameObject::GetPortal(GameObject* object)
{

}

void GameObject::GetShop(GameObject* object)
{

}

bool GameObject::get_equipment_from(GameObject& object)
{
	if (object.m_equipment_loot != NULL && m_equipment_loot == NULL)
	{
		m_equipment_loot = object.getEquipmentLoot();
		object.m_equipment_loot = NULL;
		return true;
	}
	else
	{
		return false;
	}
}

bool GameObject::setEquipmentLoot(Equipment* equipment)
{
	if (m_equipment_loot == NULL)
	{
		m_equipment_loot = equipment;
		return true;
	}
	else
	{
		return false;
	}
}

Equipment* GameObject::getEquipmentLoot()
{
	return m_equipment_loot;
}

void GameObject::releaseEquipmentLoot()
{
	m_equipment_loot = NULL;
}

bool GameObject::get_weapon_from(GameObject& object)
{
	if (object.m_weapon_loot != NULL && m_weapon_loot == NULL)
	{
		m_weapon_loot = object.getWeaponLoot();
		object.m_weapon_loot = NULL;
		return true;
	}
	else
	{
		return false;
	}
}

bool GameObject::setWeaponLoot(Weapon* weapon)
{
	if (m_weapon_loot == NULL)
	{
		m_weapon_loot = weapon;
		return true;
	}
	else
	{
		return false;
	}
}

Weapon* GameObject::getWeaponLoot()
{
	return m_weapon_loot;
}

void GameObject::releaseWeaponLoot()
{
	m_weapon_loot = NULL;
}

void GameObject::GetGrazing()
{
	//see overide function in class Ship
}

sf::Vector2f GameObject::getSpeed_for_Scrolling(Directions direction, float vspeed)
{
	sf::Vector2f speed = sf::Vector2f(0, 0);

	if (direction == DIRECTION_UP)
	{
		speed.y = vspeed;
	}
	else if (direction == DIRECTION_DOWN)
	{
		speed.y = -vspeed;
	}
	else if (direction == DIRECTION_RIGHT)
	{
		speed.x = -vspeed;
	}
	else if (direction == DIRECTION_LEFT)
	{
		speed.x = vspeed;
	}

	return speed;
}

sf::Vector2f GameObject::getSpeed_to_LocationWhileSceneSwap(Directions current_direction, Directions future_direction, float vspeed, sf::Vector2f sprite_position)
{
	sf::Vector2f speed = sf::Vector2f(0, 0);

	sf::Vector2f future_pos = GameObject::getPosition_for_Direction(future_direction, sf::Vector2f((SCENE_SIZE_X*STARTSCENE_X_RATIO), (SCENE_SIZE_Y*STARTSCENE_Y_RATIO)));
	if (future_direction == NO_DIRECTION)
	{
		future_pos = GameObject::getPosition_for_Direction(future_direction, sf::Vector2f((SCENE_SIZE_X*STARTSCENE_X_RATIO), (SCENE_SIZE_Y*STARTSCENE_X_RATIO)));
	}

	if (current_direction == DIRECTION_UP || current_direction == DIRECTION_DOWN
		|| future_direction == DIRECTION_UP || future_direction == DIRECTION_DOWN)
	{
		speed.x = vspeed * ((future_pos.x) - sprite_position.x) / SCENE_SIZE_Y;
		speed.y = vspeed * ((future_pos.y) - sprite_position.y) / SCENE_SIZE_Y;
	}

	else if (current_direction == DIRECTION_RIGHT || current_direction == DIRECTION_LEFT
		|| future_direction == DIRECTION_RIGHT || future_direction == DIRECTION_LEFT)
	{
		speed.x = vspeed * ((future_pos.x) - sprite_position.x) / SCENE_SIZE_X;
		speed.y = vspeed * ((future_pos.y) - sprite_position.y) / SCENE_SIZE_X;
	}

	return speed;
}

sf::Vector2i GameObject::getDirectionMultiplier(Directions direction)
{
	int x = 1;
	int y = 1;
	if (direction == DIRECTION_DOWN || direction == DIRECTION_RIGHT)
	{
		x *= -1;
	}
	if (direction == DIRECTION_DOWN || direction == DIRECTION_LEFT)
	{
		y *= -1;
	}

	return sf::Vector2i(x, y);
}

sf::Vector2f GameObject::getSize_for_Direction(Directions direction, sf::Vector2f size)
{
	if (direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT)
	{
		return sf::Vector2f(size.y, size.x);
	}
	else
	{
		return size;
	}
}

sf::Vector2i GameObject::getSize_for_Direction(Directions direction, sf::Vector2i size)
{
	if (direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT)
	{
		return sf::Vector2i(size.y, size.x);
	}
	else
	{
		return size;
	}
}

sf::Vector2f GameObject::getSpeed_for_Direction(Directions direction, sf::Vector2f speed)
{
	speed = GameObject::getSize_for_Direction(direction, sf::Vector2f(speed.x, speed.y));

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

float GameObject::getRotation_for_Direction(Directions direction)
{
	if (direction == DIRECTION_DOWN)
	{
		return 180;
	}
	else if (direction == DIRECTION_RIGHT)
	{
		return 90;
	}
	else if (direction == DIRECTION_LEFT)
	{
		return 270;
	}
	else
	{
		return 0;
	}
}

sf::Vector2f GameObject::getPosition_for_Direction(Directions direction, sf::Vector2f position, bool rescale)
{
	float x = position.x;
	float y = position.y;

	if (direction == DIRECTION_DOWN)
	{
		x = SCENE_SIZE_X - x;
		y = SCENE_SIZE_Y - y;
	}
	else if (direction == DIRECTION_RIGHT)
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
	else if (direction == DIRECTION_LEFT)
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

float GameObject::GetDistanceBetweenObjects(GameObject* object1, GameObject* object2)
{
	assert(object1 != NULL);
	assert(object2 != NULL);

	Vector2f current_diff = sf::Vector2f(object1->getPosition().x - object2->getPosition().x, object1->getPosition().y - object2->getPosition().y);
	return GetAbsoluteSpeed(current_diff);
}

float GameObject::GetAbsoluteSpeed(sf::Vector2f speed_)
{
	const float a = speed_.x;
	const float b = speed_.y;
	float s = (a * a) + (b * b);
	s = sqrt(s);
	s = floor(s);
	return s;
}

FloatCompare GameObject::compare_posY_withTarget_for_Direction(Directions direction, sf::Vector2f target_position)
{
	if (direction == DIRECTION_UP)
	{
		if (this->getPosition().y > target_position.y)
		{
			return GREATHER_THAN;
		}
		else if (this->getPosition().y == target_position.y)
		{
			return EQUAL_TO;
		}
		else
		{
			return LESSER_THAN;
		}
	}

	else if (direction == DIRECTION_DOWN)
	{
		if (this->getPosition().y < SCENE_SIZE_Y - target_position.y)
		{
			return GREATHER_THAN;
		}
		else if (this->getPosition().y == SCENE_SIZE_Y - target_position.y)
		{
			return EQUAL_TO;
		}
		else
		{
			return LESSER_THAN;
		}
	}

	else if (direction == DIRECTION_RIGHT)
	{
		if (this->getPosition().x < SCENE_SIZE_X - target_position.x)
		{
			return GREATHER_THAN;
		}
		if (this->getPosition().x == SCENE_SIZE_X - target_position.x)
		{
			return EQUAL_TO;
		}
		else
		{
			return LESSER_THAN;
		}
	}

	else
	{
		if (this->getPosition().x > target_position.x)
		{
			return GREATHER_THAN;
		}
		else if (this->getPosition().x == target_position.x)
		{
			return EQUAL_TO;
		}
		else
		{
			return LESSER_THAN;
		}
	}
}

FloatCompare GameObject::compare_posX_withTarget_for_Direction(Directions direction, sf::Vector2f target_position)
{
	{
		if (direction == DIRECTION_UP)
		{
			if (this->getPosition().x > target_position.x)
			{
				return GREATHER_THAN;
			}
			else if (this->getPosition().x == target_position.x)
			{
				return EQUAL_TO;
			}
			else
			{
				return LESSER_THAN;
			}
		}

		else if (direction == DIRECTION_DOWN)
		{
			if (this->getPosition().x < SCENE_SIZE_X - target_position.x)
			{
				return GREATHER_THAN;
			}
			else if (this->getPosition().x == SCENE_SIZE_X - target_position.x)
			{
				return EQUAL_TO;
			}
			else
			{
				return LESSER_THAN;
			}
		}

		else if (direction == DIRECTION_RIGHT)
		{
			if (this->getPosition().y > target_position.y)
			{
				return GREATHER_THAN;
			}
			if (this->getPosition().y == target_position.y)
			{
				return EQUAL_TO;
			}
			else
			{
				return LESSER_THAN;
			}
		}

		else
		{
			if (this->getPosition().y < SCENE_SIZE_Y - target_position.y)
			{
				return GREATHER_THAN;
			}
			else if (this->getPosition().y == SCENE_SIZE_Y - target_position.y)
			{
				return EQUAL_TO;
			}
			else
			{
				return LESSER_THAN;
			}
		}
	}
}

sf::Vector2f GameObject::setPosition_Y_for_Direction(Directions direction, sf::Vector2f target_position, bool centered)
{
	if (direction == DIRECTION_UP || direction == NO_DIRECTION)
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

	else if (direction == DIRECTION_DOWN)
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

	else if (direction == DIRECTION_RIGHT)
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

	else
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

sf::Vector2f GameObject::getRandomXSpawnPosition(Directions direction, sf::Vector2f max_enemy_size, sf::Vector2f cluster_size)
{
	//default argument for cluster dize
	if (cluster_size == sf::Vector2f(0, 0))
	{
		cluster_size = max_enemy_size;
	}

	//now calculating the starting coordinate (left)
	sf::Vector2f rand_coordinates_min = sf::Vector2f(max_enemy_size.x / 2, -cluster_size.y / 2);
	rand_coordinates_min = GameObject::getPosition_for_Direction(direction, rand_coordinates_min, false);

	//length of the allowed spread
	int i_ = GameObject::getDirectionMultiplier(direction).y;
	float allowed_spread = GameObject::getSize_for_Direction(direction, sf::Vector2f(i_*(SCENE_SIZE_X - cluster_size.x), i_*(SCENE_SIZE_Y - cluster_size.x))).x;

	//cutting clusters bigger than the scene (+ debug message)
	if ((allowed_spread*GameObject::getDirectionMultiplier(direction).y) < 0)
	{
		LOGGER_WRITE(Logger::DEBUG, TextUtils::format("ERROR: Error in calculation of 'allowed_spread' value in enemy generation. This value leads out of screen.\n"));
	}

	//random value inside the allowed spread
	float random_posX = RandomizeFloatBetweenValues(sf::Vector2f(0, allowed_spread));

	//getting position coordinates (min + random value)
	float pos_x = GameObject::getSize_for_Direction(direction, sf::Vector2f(rand_coordinates_min.x + random_posX, rand_coordinates_min.x)).x;
	float pos_y = GameObject::getSize_for_Direction(direction, sf::Vector2f(rand_coordinates_min.y, rand_coordinates_min.y + random_posX)).x;

	return sf::Vector2f(pos_x, pos_y);
}

sf::Vector2f GameObject::ApplyScreenBordersConstraints(Directions direction, sf::Vector2f position, sf::Vector2f size)
{
	sf::Vector2f new_position = position;
	sf::Vector2f l_size = GameObject::getSize_for_Direction(direction, sf::Vector2f(size.x, size.y));

	float l_overlap_left = position.x - (l_size.x / 2);
	if (l_overlap_left < 0)
	{
		new_position = sf::Vector2f((l_size.x / 2), position.y);
	}

	float l_overlap_right = position.x + (l_size.x / 2);
	if (l_overlap_right > SCENE_SIZE_X)
	{
		new_position = sf::Vector2f(SCENE_SIZE_X - (l_size.x / 2), position.y);
	}

	float l_overlap_up = position.y - (l_size.y / 2);
	if (l_overlap_up < 0)
	{
		new_position = sf::Vector2f(position.x, (l_size.y / 2));
	}

	float l_overlap_down = position.y + (l_size.y / 2);
	if (l_overlap_down > SCENE_SIZE_Y)
	{
		new_position = sf::Vector2f(position.x, SCENE_SIZE_Y - (l_size.x / 2));
	}

	return new_position;
}

void GameObject::updatePostCollision()
{
	//see override in class Ship
}

float GameObject::getFighterFloatStatValue(FighterStats stat)
{
	//see override in class Ship
	return -1;
}

int GameObject::getFighterIntStatValue(FighterStats stat)
{
	switch (stat)
	{
		case Fighter_Armor:
		{
			return m_armor;
		}
		case Fighter_ArmorMax:
		{
			return m_armor_max;
		}
		case Fighter_Shield:
		{
			return m_shield;
		}
		case Fighter_ShieldMax:
		{
			return m_shield_max;
		}
		case Fighter_ShieldRegen:
		{
			return m_shield_regen;
		}
		case Fighter_ContactDamage:
		{
			return m_damage;
		}
		default:
		{
			return -1;
		}
	}
}