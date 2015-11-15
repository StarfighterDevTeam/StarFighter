#include "GameObject.h"

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber, int m_animationNumber) : AnimatedSprite()
{
	Init(position, speed, textureName, size, m_frameNumber, m_animationNumber);
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
	vector<string> s1 = TextUtils::split(this->textureName, '/');
	return *(s1.end() - 1);
}

GameObject::GameObject()
{

}

void GameObject::setAnimationLine(int animation, bool keep_frame_index)
{
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
	anim->setSpriteSheet(*this->defaultAnimation.getSpriteSheet());
	for (size_t j = 0; j < this->defaultAnimation.getSize(); j++)
	{
		size_t n = j / m_frameNumber;
		//when we have reached out to the correct line of animation frames, we put this line into the animation
		if (n == animation)
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
	this->currentAnimationIndex = animation;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber, int animationNumber)
{
	this->m_animationNumber = animationNumber;
	this->m_frameNumber = frameNumber;
	this->initial_position = sf::Vector2f(position.x, position.y);
	this->m_size.x = ((*texture).getSize().x / frameNumber);
	this->m_size.y = ((*texture).getSize().y / animationNumber);

	this->defaultAnimation.setSpriteSheet(*texture);
	for (int j = 0; j < animationNumber; j++)
	{
		for (int i = 0; i < frameNumber; i++)
		{
			int x = ((*texture).getSize().x / frameNumber)*(i);
			int y = ((*texture).getSize().y / animationNumber)*(j);
			this->defaultAnimation.addFrame(sf::IntRect(x, y, this->m_size.x, this->m_size.y));
		}
	}
	
	this->setAnimationLine(0);//default starting animation is line 0 (top of the sprite sheet)
	
	this->speed = speed;
	this->setPosition(position.x, position.y);
	this->visible = true;
	this->isOnScene = false;
	this->GarbageMe = false;
	this->DontGarbageMe = true;
	this->diag = (float)sqrt(((m_size.x / 2)*(m_size.x / 2)) + ((m_size.y / 2)*(m_size.y / 2)));
	this->ghost = false;
	this->rotation_speed = 0.f;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*m_frameNumber, size.y*m_animationNumber);
	this->textureName = textureName;

	this->setOrigin(size.x / 2, size.y / 2);

	Init(position, speed, texture, m_frameNumber, m_animationNumber);
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber, sf::Uint8* pixels)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*m_frameNumber, size.y*m_animationNumber, pixels);
	this->textureName = textureName;

	this->setOrigin(size.x / 2, size.y / 2);

	Init(position, speed, texture, m_frameNumber, m_animationNumber);

}

GameObject::~GameObject()
{
	//TODO

	//delete this->equipment_loot;
	//delete this->weapon_loot;
}

void GameObject::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition;
	
	//Basic movement (initial vector)
	newposition.x = this->getPosition().x + speed.x*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + speed.y*deltaTime.asSeconds();

	this->setPosition(newposition.x, newposition.y);

	if (feedback_reset_clock.getElapsedTime().asSeconds() > GOAL_RESPAWN_COOLDOWN)
	{
		setColor(Color(255, 255, 255, 255));
	}

	AnimatedSprite::update(deltaTime);
}

void GameObject::updateAnimation(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}

void GameObject::Respawn()
{

}

void GameObject::setGhost(bool m_ghost)
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

float GameObject::GetAbsoluteSpeed()
{
	const float a = speed.x;
	const float b = speed.y;
	float s = (a * a) + (b * b);
	s = sqrt(s);
	s = floor(s);

	return s;
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

float GameObject::GetAngleRadForSpeed(sf::Vector2f curSpeed)
{
	const float a = curSpeed.x;
	const float b = curSpeed.y;

	if (a == 0 && b == 0)
		return 0.f;

	float distance_to_obj = (a * a) + (b * b);
	distance_to_obj = sqrt(distance_to_obj);

	float angle;
	angle = acos(a / distance_to_obj);

	if (b < 0)
	{
		angle = -angle;
	}

	angle += M_PI_2;
	//angle = (fmod(angle, 2 * M_PI));
	
	return angle;
}

float GameObject::GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2)
{
	assert(ref_object != NULL);
	assert(object2 != NULL);

	return GetAngleRadBetweenPositions(ref_object->getPosition(), object2->getPosition());
}

float GameObject::GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2)
{
	const sf::Vector2f diff = sf::Vector2f(ref_position.x - position2.x, ref_position.y - position2.y);
	float target_angle = GetAngleRadForSpeed(diff);

	const float a = diff.x;
	const float b = diff.y;

	float distance_to_obj = (a * a) + (b * b);
	distance_to_obj = sqrt(distance_to_obj);

	float angle;
	angle = acos(a / distance_to_obj);

	if (b < 0)
	{
		angle = -angle;
	}

	angle += M_PI_2;

	return angle;
}

void GameObject::SetSpeedVectorFromAbsoluteSpeed(float absolute_speed, float curAngle)
{
	speed.x = - absolute_speed * sin(curAngle);
	speed.y = absolute_speed * cos(curAngle);
}

float GameObject::GetDistanceBetweenObjects(GameObject* object1, GameObject* object2)
{
	assert(object1 != NULL);
	assert(object2 != NULL);

	Vector2f current_diff = sf::Vector2f(object1->getPosition().x - object2->getPosition().x, object1->getPosition().y - object2->getPosition().y);
	return GetAbsoluteSpeed(current_diff);
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

GameObject* GameObject::Clone()
{
	GameObject* clone = new GameObject(this->getPosition(), this->speed, this->textureName, this->m_size);
	clone->collider_type = this->collider_type;
	clone->layer = this->layer;

	return clone;
}

//TRON SPECIFIC
void GameObject::GetDiscoball(GameObject* discoball, float angle_collision)
{
	//see override function
}

void GameObject::PlayerContact(GameObject* player, float angle_collision)
{
	//see override function
}

void GameObject::GetPortal(GameObject* portal)
{
	//see override function
}

void GameObject::PlayHitFeedback()
{
	setColor(Color(0, 255, 0, 255));
	feedback_reset_clock.restart();
}

void GameObject::DiscoballBumper(GameObject* bumper)
{
	// see override function in class Ship
}

void GameObject::PlayerBumper(GameObject* bumper, Time deltaTime)
{
	// see override function in class Ship
}

void GameObject::UsingPortal(bool is_using)
{
	// see override function in class Ship and Discoball
}

bool GameObject::IntersectSegments(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
	//segment 1: [p0, p1], segment 2: [p2, p3]
	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		// Collision detected
		if (i_x != NULL)
			*i_x = p0_x + (t * s1_x);
		if (i_y != NULL)
			*i_y = p0_y + (t * s1_y);
		return 1;
	}

	return 0; // No collision
}

bool GameObject::isCapsuleColliding(GameObject* object, GameObject* bumper, float *i_x, float *i_y, sf::Time deltaTime)
{
	if (!object || !bumper)
		return false;

	//p_0 : current position of discoball
	//p_1 : previous position of discoball
	//p_2 : bumper segment point 1
	//p_3 : bumper segment point 2
	const float p_0x = object->getPosition().x;
	const float p_0y = object->getPosition().y;
	const float p_1x = object->getPosition().x - object->speed.x * deltaTime.asSeconds();
	const float p_1y = object->getPosition().y - object->speed.y * deltaTime.asSeconds();

	//printf("pos: %f | previous pos: %f | speed: %f (%f)\n", p_0x, p_1x, object->speed.x, object->speed.x * deltaTime.asSeconds());

	bool is_bumper_vertical = bumper->m_size.x > bumper->m_size.y ? false : true;
	const float p_2x = is_bumper_vertical ? bumper->getPosition().x : bumper->getPosition().x - bumper->m_size.x / 2;
	const float p_2y = is_bumper_vertical ? bumper->getPosition().y - bumper->m_size.y / 2 : bumper->getPosition().y;
	const float p_3x = is_bumper_vertical ? bumper->getPosition().x : bumper->getPosition().x + bumper->m_size.x / 2;
	const float p_3y = is_bumper_vertical ? bumper->getPosition().y + bumper->m_size.y / 2 : bumper->getPosition().y;

	//collision at arrival? distance of point p0 to segment [p_2, p_3]
	const float dist = sqrt(((p_3y - p_2y)*(p_0x - p_2x) + (p_3x - p_2x)*(p_0y - p_2y))*((p_3y - p_2y)*(p_0x - p_2x) + (p_3x - p_2x)*(p_0y - p_2y)) / ((p_3x - p_2x)*(p_3x - p_2x) + (p_3y - p_2y)*(p_3y - p_2y)));
	if (dist - object->m_size.x / 2 < 0)
	{
		printf("Collision at arrival position \n");
		return true;
	}
	else
	{
		//capsule collision
		const float theta = GetAngleRadForSpeed(object->speed);
		const float angle_top = M_PI - theta;
		const float angle_bottom = -theta;
		const float offset_top_x = object->m_size.x / 2 * cos(angle_top);
		const float offset_top_y = -object->m_size.x / 2 * sin(angle_top);
		const float offset_bottom_x = object->m_size.x / 2 * cos(angle_bottom);
		const float offset_bottom_y = -object->m_size.x / 2 * sin(angle_bottom);

		const float p_0Topx = p_0x + offset_top_x;
		const float p_0Topy = p_0y + offset_top_y;
		const float p_1Topx = p_1x + offset_top_x;
		const float p_1Topy = p_1y + offset_top_y;
		const float p_0Botx = p_0x + offset_bottom_x;
		const float p_0Boty = p_0y + offset_bottom_y;
		const float p_1Botx = p_1x + offset_bottom_x;
		const float p_1Boty = p_1y + offset_bottom_y;

		if (IntersectSegments(p_0Topx, p_0Topy, p_1Topx, p_1Topy, p_2x, p_2y, p_3x, p_3y, i_x, i_y)
			|| IntersectSegments(p_0Botx, p_0Boty, p_1Botx, p_1Boty, p_2x, p_2y, p_3x, p_3y, i_x, i_y))
		{
			printf("Collision on capsule movement\n");
			return true;
		}
		else
			return false;
	}	
}