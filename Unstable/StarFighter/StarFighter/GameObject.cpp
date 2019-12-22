#include "GameObject.h"

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : AnimatedSprite()
{
	Init(position, speed, textureName, size, frameNumber, animationNumber);
	setOrigin(origin.x, origin.y);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : AnimatedSprite()
{
	Init(position, speed, textureName, size);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture) : AnimatedSprite()
{
	Init(position, speed, texture, 1);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, float stroke_size)
{
	Init(position, speed, color, size, stroke_size);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, float radius, float stroke_size)
{
	Init(position, speed, color, radius, stroke_size);
}

string GameObject::getName()
{
	vector<string> s1 = TextUtils::split(m_textureName, '/');
	return *(s1.end() - 1);
}

GameObject::GameObject()
{

}

void GameObject::setAnimationLine(int animation, bool keep_frame_index)
{
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
	anim->setSpriteSheet(*this->m_defaultAnimation.getSpriteSheet());
	for (size_t j = 0; j < m_defaultAnimation.getSize(); j++)
	{
		size_t n = j / m_frameNumber;
		//when we have reached out to the correct line of animation frames, we put this line into the animation
		if (n == animation)
		{
			anim->addFrame(m_defaultAnimation.getFrame(j));
		}
	}

	if (m_currentAnimation)
		delete m_currentAnimation;

	m_currentAnimation = anim;
	anim = NULL;
	play(*m_currentAnimation, keep_frame_index);
	m_currentAnimationIndex = animation;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber, int animationNumber)
{
	m_animationNumber = animationNumber;
	m_frameNumber = frameNumber;
	m_size.x = ((*texture).getSize().x / frameNumber);
	m_size.y = ((*texture).getSize().y / animationNumber);

	m_collider_type = GameObjectType::BackgroundObject;
	m_defaultAnimation.setSpriteSheet(*texture);

	for (int j = 0; j < animationNumber; j++)
	{
		for (int i = 0; i < frameNumber; i++)
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
	m_GarbageMe = false;
	m_DontGarbageMe = false;
	m_diag = (float)sqrt(((m_size.x / 2)*(m_size.x / 2)) + ((m_size.y / 2)*(m_size.y / 2)));
	m_ghost = false;
	m_rotation_speed = 0.f;
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

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*frameNumber, size.y*animationNumber, pixels);
	m_textureName = textureName;

	setOrigin(size.x / 2, size.y / 2);

	Init(position, speed, texture, frameNumber, animationNumber);
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, float stroke_size)
{
	sf::Uint8* pixels = CreateRectangleWithStroke(size, color, stroke_size);
	ostringstream ss;
	ss << "rectangle_" << (int)size.x << "x" << (int)size.y << "_" << (int)color.r << "_" << (int)color.g << "_" << (int)color.b << "_" << (int)color.a;
	string textureName = ss.str();
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x, size.y, pixels);
	Init(position, speed, texture, 1, 1);

	setOrigin(size.x / 2, size.y / 2);
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, float radius, float stroke_size)
{
	sf::Uint8* pixels = CreateCircleWithStroke(radius, color, stroke_size);
	ostringstream ss;
	ss << "circle_" << (int)radius * 2 << "x" << (int)radius * 2 << "_" << (int)color.r << "_" << (int)color.g << "_" << (int)color.b << "_" << (int)color.a;
	string textureName = ss.str();
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, radius * 2, radius * 2, pixels);
	Init(position, speed, texture, 1, 1);

	setOrigin(radius, radius);
}

GameObject::~GameObject()
{
	//TODO

	//delete this->equipment_loot;
	//delete this->weapon_loot;
}

void GameObject::update(sf::Time deltaTime)
{
	m_previous_speed = m_speed;

	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;
	
	//Basic movement (initial vector)
	newposition.x = getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	setPosition(newposition.x, newposition.y);

	if (m_frameNumber > 1)
	{
		AnimatedSprite::update(deltaTime);
	}
}

void GameObject::Draw(RenderTarget& screen)
{
	screen.draw(*this);
}

void GameObject::updateAnimation(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
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

void GameObject::SetConditionalColor(sf::Color color, bool condition)
{
	if (condition == true)
	{
		setColor(color);
	}
	else
	{
		setColor(Color(255, 255, 255, 255));
	}
}

GameObject* GameObject::Clone()
{
	GameObject* clone = new GameObject(this->getPosition(), this->m_speed, this->m_textureName, this->m_size, sf::Vector2f(this->m_size.x/2, this->m_size.y/2), this->m_frameNumber, this->m_animationNumber);
	clone->m_collider_type = this->m_collider_type;
	clone->m_layer = this->m_layer;

	return clone;
}

float GameObject::GetAbsoluteSpeed()
{
	const float a = m_speed.x;
	const float b = m_speed.y;
	float s = (a * a) + (b * b);
	s = sqrt(s);
	s = floor(s);

	return s;
}

float GameObject::GetAbsoluteSpeedSquared()
{
	const float a = m_speed.x;
	const float b = m_speed.y;
	float s = (a * a) + (b * b);

	return s;
}

float GameObject::SetRotationToTarget(sf::Vector2f target_position)
{
	float angle_deg = GetAngleDegToTargetPosition(getPosition(), getRotation(), target_position);
	rotate(angle_deg);

	return angle_deg;
}

float GameObject::GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2)
{
	assert(ref_object != NULL);
	assert(object2 != NULL);

	return GetAngleRadBetweenPositions(ref_object->getPosition(), object2->getPosition());
}

sf::Vector2f GameObject::SetConstantSpeedToDestination(sf::Vector2f coordinates, float speed)
{
	sf::Vector2f vector_to_destination = coordinates - getPosition();

	sf::Vector2f move = vector_to_destination;
	ScaleVector(&move, speed);

	m_speed = move;

	return m_speed;
}

void GameObject::SetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle)
{
	m_speed.x = -absolute_speed * sin(curAngle);
	m_speed.y = absolute_speed * cos(curAngle);
}

float GameObject::GetDistanceBetweenObjects(GameObject* object1, GameObject* object2)
{
	assert(object1 != NULL);
	assert(object2 != NULL);

	return GetDistanceBetweenPositions(object1->getPosition(), object2->getPosition());
}

bool GameObject::BounceOnBorders(sf::Vector2f area_size)
{
	bool bounced = false;
	if (getPosition().x - m_size.x / 2 < 0 && m_speed.x < 0)
	{
		m_speed.x *= -1;
		bounced = true;
	}
	if (getPosition().x + m_size.x / 2 > area_size.x  && m_speed.x > 0)
	{
		m_speed.x *= -1;
		bounced = true;
	}
	if (getPosition().y - m_size.y / 2 < 0 && m_speed.y < 0)
	{
		m_speed.y *= -1;
		bounced = true;
	}
	if (getPosition().y + m_size.y / 2 > area_size.y && m_speed.y > 0)
	{
		m_speed.y *= -1;
		bounced = true;
	}

	return bounced;
}

bool GameObject::IsGameObjectInsideArea(sf::Vector2f coordinates, sf::Vector2f area_size)
{
	sf::Vector2f bounds = sf::Vector2f(this->getLocalBounds().width, this->getLocalBounds().height);

	return IsInsideArea(bounds, coordinates, area_size);
}