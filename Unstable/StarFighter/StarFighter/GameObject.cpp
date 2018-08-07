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

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size)
{
	Init(position, speed, color, size);
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

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size)
{
	sf::Uint8* pixels = GameObject::CreateRectangleWithStroke(size, color, 0);
	ostringstream ss;
	ss << "rectangle_" << (int)size.x << "_" << (int)size.y << "_" << color.r << "_" << color.g << "_" << color.b << "_" << color.a;
	string textureName = ss.str();
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x, size.y, pixels);
	Init(position, speed, texture, 1, 1);

	setOrigin(size.x / 2, size.y / 2);
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

float GameObject::GetVectorLength(sf::Vector2f vector)
{
	const float a = vector.x;
	const float b = vector.y;
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

float GameObject::GetVectorLengthSquared(sf::Vector2f vector)
{
	const float a = vector.x;
	const float b = vector.y;
	float s = (a * a) + (b * b);

	return s;
}

void GameObject::GetAngleRadAndLengthOfVector(sf::Vector2f vector, float* output_length, float* output_angle)
{
	const float a = vector.x;
	const float b = vector.y;

	if (a == 0 && b == 0)
	{
		*output_length = 0.f;
		*output_angle = 0.f;
		return;
	}

	*output_length = (a * a) + (b * b);
	*output_length = sqrt(*output_length);

	*output_angle = acos(a / *output_length);

	if (b < 0)
	{
		*output_angle = -*output_angle;
	}

	*output_angle += M_PI_2;
}

float GameObject::GetAngleRadToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position)
{
	float angle = GameObject::GetAngleRadBetweenPositions(target_position, ref_position);
	float delta_angle = angle - (ref_rotation_in_deg * M_PI / 180.f);
	if (delta_angle > M_PI)
		delta_angle -= M_PI * 2;
	else if (delta_angle < -M_PI)
		delta_angle += M_PI * 2;

	return delta_angle;
}

float GameObject::GetAngleDegToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position)
{
	float angle = GameObject::GetAngleRadBetweenPositions(target_position, ref_position) * 180.f / M_PI;
	float delta_angle = angle - ref_rotation_in_deg;
	if (delta_angle > 180)
		delta_angle -= 180.f * 2;
	else if (delta_angle < -180)
		delta_angle += 180.f * 2;

	return delta_angle;
}

float GameObject::GetAngleRadForVector(sf::Vector2f vector)
{
	const float a = vector.x;
	const float b = vector.y;

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

sf::Vector2f GameObject::RotateVector(sf::Vector2f vector, float angle_rad)
{
	float length;
	float angle;
	GetAngleRadAndLengthOfVector(vector, &length, &angle);

	angle += angle_rad;

	sf::Vector2f output_vector = GetVectorFromLengthAndAngle(length, angle);

	return output_vector;
}

void GameObject::SetVectorRotation(sf::Vector2f* vector, float angle_rad)
{
	float length = GetVectorLength(*vector);
	sf::Vector2f new_vector = GetVectorFromLengthAndAngle(length, angle_rad);
}

sf::Vector2f GameObject::GetVectorFromLengthAndAngle(float absolute_speed, float curAngle)
{
	sf::Vector2f speed;
	speed.x = -absolute_speed * sin(curAngle);
	speed.y = absolute_speed * cos(curAngle);

	return speed;
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
	float target_angle = GetAngleRadForVector(diff);

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

sf::Vector2f GameObject::SetSpeedForConstantSpeedToDestination(sf::Vector2f coordinates, float speed)
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

float GameObject::GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2)
{
	Vector2f current_diff = sf::Vector2f(position1.x - position2.x, position1.y - position2.y);
	return GetVectorLength(current_diff);
}

float GameObject::GetDistanceSquaredBetweenPositions(sf::Vector2f position1, sf::Vector2f position2)
{
	Vector2f current_diff = sf::Vector2f(position1.x - position2.x, position1.y - position2.y);
	return GetVectorLengthSquared(current_diff);
}

bool GameObject::NormalizeVector(sf::Vector2f* vector, float max_value)
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

void GameObject::ScaleVector(sf::Vector2f* vector, float target_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float p = target_value / sqrt(vector->x * vector->x + vector->y * vector->y);
	vector->x *= p;
	vector->y *= p;
}

void GameObject::AddValueToVector(sf::Vector2f* vector, float added_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float target_value = GetVectorLength(*vector) + added_value;
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

sf::Uint8* GameObject::CreateRectangleWithStroke(sf::Vector2f size, sf::Color color, int stroke_size)
{
	//pixel array creation
	const int W = size.x;
	const int H = size.y;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	for (int i = 0; i < W * H * 4; i += 4)
	{
		pixels[i] = color.r;			// R
		pixels[i + 1] = color.g;		// G
		pixels[i + 2] = color.b;	// B
		pixels[i + 3] = 255;
	}
	//inside stroke
	for (int i = 0; i < W * H * 4; i += 4)
	{
		if (stroke_size < 1 || (i / 4) <= W * (stroke_size) || (i / 4) >(H - 1 * (stroke_size))*W || (i / 4) % W <= 0 + ((stroke_size)-1) || (i / 4) % W >= (W - 1 * (stroke_size))) // A
		{
			pixels[i + 3] = 255;
		}
		else
		{
			pixels[i + 3] = RECTANGLE_INSIDE_ALPHA;
		}
	}

	return pixels;
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