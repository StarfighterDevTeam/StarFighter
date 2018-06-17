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
	vector<string> s1 = TextUtils::split(this->m_textureName, '/');
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
	if (animation >= this->m_animationNumber)
	{
		printf("Requesting an animation line (%d) that exceeds what is allowed (%d) for this item", animation, this->m_animationNumber);
		animation = this->m_animationNumber - 1;
		if (animation < 0)
		{
			animation = 0;
		}
	}

	//now let's load the new animation
	Animation* anim = new Animation();
	anim->setSpriteSheet(*this->m_defaultAnimation.getSpriteSheet());
	for (size_t j = 0; j < this->m_defaultAnimation.getSize(); j++)
	{
		size_t n = j / this->m_frameNumber;
		//when we have reached out to the correct line of animation frames, we put this line into the animation
		if (n == animation)
		{
			anim->addFrame(this->m_defaultAnimation.getFrame(j));
		}
	}

	if (m_currentAnimation)
		delete m_currentAnimation;
	this->m_currentAnimation = anim;
	anim = NULL;
	this->play(*m_currentAnimation, keep_frame_index);
	this->m_currentAnimationIndex = animation;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber, int animationNumber)
{
	m_animationNumber = animationNumber;
	m_frameNumber = frameNumber;
	m_initial_position = sf::Vector2f(position.x, position.y);
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
	m_avoiding_x = 0;
	m_avoiding_y = 0;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*frameNumber, size.y*animationNumber);
	this->m_textureName = textureName;

	this->setOrigin(size.x / 2, size.y / 2);

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

GameObject::~GameObject()
{
	//TODO

	//delete this->equipment_loot;
	//delete this->weapon_loot;
}

void GameObject::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;
	
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

float GameObject::GetAbsoluteSpeed(sf::Vector2f speed_)
{
	const float a = speed_.x;
	const float b = speed_.y;
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

float GameObject::GetAbsoluteSpeedSquared(sf::Vector2f speed_)
{
	const float a = speed_.x;
	const float b = speed_.y;
	float s = (a * a) + (b * b);

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

sf::Vector2f GameObject::GetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle)
{
	sf::Vector2f speed;
	speed.x = -absolute_speed * sin(curAngle);
	speed.y = absolute_speed * cos(curAngle);

	return speed;
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

float GameObject::GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2)
{
	Vector2f current_diff = sf::Vector2f(position1.x - position2.x, position1.y - position2.y);
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

void GameObject::ScaleSpeed(sf::Vector2f* vector, float target_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float p = target_value / sqrt(vector->x * vector->x + vector->y * vector->y);
	vector->x *= p;
	vector->y *= p;
}

void GameObject::AddSpeed(sf::Vector2f* vector, float added_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float target_value = GetAbsoluteSpeed(*vector) + added_value;
	float p = target_value / sqrt(vector->x * vector->x + vector->y * vector->y);
	vector->x *= p;
	vector->y *= p;
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
		return true;
	}

	return false; // No collision
}

float GameObject::DistancePointToSement(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float *i_x, float *i_y)
{
	//distance of point p0_ to segment [p1_, p2_]
	const float px = p2_x - p1_x;
	const float py = p2_y - p1_y;
	float u = ((p0_x - p1_x)*px + (p0_y - p1_y)*py) / (px*px + py*py);
	u = (u > 1) ? 1 : (u < 0) ? 0 : u;
	const float x = p1_x + u*px;
	const float y = p1_y + u*py;
	const float dx = x - p0_x;
	const float dy = y - p0_y;
	const float dist = sqrt(dx*dx + dy*dy);

	// Orthogonal projected point
	if (i_x != NULL)
		*i_x = x;
	if (i_y != NULL)
		*i_y = y;

	return dist;
}


int GameObject::GetPixelDistanceFromEdge(int pixel_index, int width, int height)
{
	int line = (pixel_index / 4 / width);
	int row = (pixel_index / 4) % width;

	int x_ = row;
	if (width - row - 1 < x_)
		x_ = width - row;

	int y_ = line;
	if (height - line - 1 < y_)
		y_ = height - line;

	int distance = x_;
	if (y_ < distance)
		distance = y_;

	distance++;//min value to return is 1
	return distance;
}

void GameObject::GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size)
{
	if (blur_radius > 0)
	{
		for (int i = 0; i < width * height * 4; i += 4)
		{
			if ((i / 4) <= width * blur_radius || (i / 4) >(height - 1 * blur_radius)*width || (i / 4) % width <= 0 + (blur_radius - 1) || (i / 4) % width >= (width - 1 * blur_radius))
			{
				int nominator = GaussianBlurDistribution(GetPixelDistanceFromEdge(i, width, height));
				int denominator = GaussianBlurDistribution(blur_radius + 1);
				pixels[i + 3] = (Uint8)(ceil(1.f * nominator / denominator * 255)); // Alpha
			}
			else if ((i / 4) <= width * (blur_radius + stroke_size) || (i / 4) >(height - 1 * (blur_radius + stroke_size))*width || (i / 4) % width <= 0 + ((blur_radius + stroke_size) - 1) || (i / 4) % width >= (width - 1 * (blur_radius + stroke_size)))
			{
				//stroke: do nothing
			}
			else if ((i / 4) <= width * (2 * blur_radius + stroke_size) || (i / 4) >(height - 1 * (2 * blur_radius + stroke_size))*width || (i / 4) % width <= 0 + ((2 * blur_radius + stroke_size) - 1) || (i / 4) % width >= (width - 1 * (2 * blur_radius + stroke_size)))
			{
				int nominator = GaussianBlurDistribution((blur_radius - 1) - (GetPixelDistanceFromEdge(i, width, height) - stroke_size - blur_radius - 1));
				int denominator = GaussianBlurDistribution(blur_radius + 1);
				pixels[i + 3] = (Uint8)(ceil(1.f * nominator / denominator * 255)); // Alpha
			}
		}
	}
}

int GameObject::GaussianBlurDistribution(int x)
{
	return x*x;
}

//ATLANTIS SPECIFICS
void GameObject::AddToBoidNeighbours(GameObject* boid)
{
	//see override function in class Boid
}

void GameObject::ClearBoidNeighbours()
{
	//see override function in class Boid
}

bool GameObject::IsThreat(sf::Vector2f threat_pos, float threat_diag_size, float threat_angle)
{
	float distance = GetDistanceBetweenPositions(getPosition(), threat_pos) - m_diag - threat_diag_size;

	float angle = GetAngleRadBetweenPositions(getPosition(), threat_pos) * 180 / M_PI;

	float delta_angle = angle - threat_angle;
	if (delta_angle > 180)
		delta_angle -= 360;
	else if (delta_angle < -180)
		delta_angle += 360;

	if (distance < FLEEING_RADIUS && delta_angle > -FLEEING_ANGLE / 2 && delta_angle < FLEEING_ANGLE / 2)
	{
		return true;
	}
	
	return false;
}

bool GameObject::IsPrey(sf::Vector2f prey_pos, float prey_diag_size, float prey_angle)
{
	float distance = GetDistanceBetweenPositions(getPosition(), prey_pos) - m_diag - prey_diag_size;

	float angle = GetAngleRadBetweenPositions(prey_pos, getPosition()) * 180 / M_PI;

	float delta_angle = angle - prey_angle;
	if (delta_angle > 180)
		delta_angle -= 360;
	else if (delta_angle < -180)
		delta_angle += 360;

	if (distance < PREDATOR_CHASING_RADIUS && delta_angle > -PREDATOR_CHASING_ANGLE / 2 && delta_angle < PREDATOR_CHASING_ANGLE / 2)
	{
		return true;
	}

	return false;
}

sf::Vector2f GameObject::AvoidBorders()
{
	sf::Vector2f avoid_vector = sf::Vector2f(0, 0);

	bool left_or_right = RandomizeIntBetweenValues(0, 1) == 1 ? true : false;
	int lr = left_or_right ? 1 : -1;

	if (getPosition().x - (AVOID_BORDERS_RADIUS_COEFF + 0.5) * m_size.x  < 0 && m_speed.x < 0)
	{
		lr = m_avoiding_y == 0 ? lr : m_avoiding_y;
		m_avoiding_y = m_avoiding_y == 0 ? lr : m_avoiding_y;
		avoid_vector += sf::Vector2f(0, 1.f * lr);
	}
	if (getPosition().x + (AVOID_BORDERS_RADIUS_COEFF + 0.5) * m_size.x > REF_WINDOW_RESOLUTION_X  && m_speed.x > 0)
	{
		lr = m_avoiding_y == 0 ? lr : m_avoiding_y;
		m_avoiding_y = m_avoiding_y == 0 ? lr : m_avoiding_y;
		avoid_vector += sf::Vector2f(0, 1.f * lr);
	}
	if (getPosition().y - (AVOID_BORDERS_RADIUS_COEFF + 0.5) * m_size.y < 0 && m_speed.y < 0)
	{
		lr = m_avoiding_x == 0 ? lr : m_avoiding_x;
		m_avoiding_x = m_avoiding_x == 0 ? lr : m_avoiding_x;
		avoid_vector += sf::Vector2f(1.f * lr, 0);
	}
	if (getPosition().y + (AVOID_BORDERS_RADIUS_COEFF + 0.5) * m_size.y > REF_WINDOW_RESOLUTION_Y && m_speed.y > 0)
	{
		lr = m_avoiding_x == 0 ? lr : m_avoiding_x;
		m_avoiding_x = m_avoiding_x == 0 ? lr : m_avoiding_x;
		avoid_vector += sf::Vector2f(1.f * lr, 0);
	}

	if (avoid_vector.x == 0)
	{
		m_avoiding_x = 0;
	}
	if (avoid_vector.y == 0)
	{
		m_avoiding_y = 0;
	}

	return avoid_vector;
}