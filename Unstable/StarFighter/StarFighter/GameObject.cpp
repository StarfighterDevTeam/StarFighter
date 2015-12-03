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

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*frameNumber, size.y*animationNumber);
	this->textureName = textureName;

	this->setOrigin(size.x / 2, size.y / 2);

	Init(position, speed, texture, frameNumber, animationNumber);
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber, sf::Uint8* pixels)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, size.x*frameNumber, size.y*animationNumber, pixels);
	this->textureName = textureName;

	this->setOrigin(size.x / 2, size.y / 2);

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

float GameObject::GetAbsoluteSpeedSquared()
{
	const float a = speed.x;
	const float b = speed.y;
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

//void GameObject::DiscoballBumper(GameObject* bumper, sf::Time deltaTime)
//{
//	// see override function in class Ship
//}

void GameObject::PlayerBumper(GameObject* bumper, Time deltaTime)
{
	// see override function in class Ship
}

void GameObject::UsingPortal(bool is_using)
{
	// see override function in class Ship and Discoball
}

void GameObject::CheckIfPlayerDiscoballBumped(Time deltaTime)
{
	// see override function in class Ship
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

bool GameObject::isCapsuleColliding(GameObject* object, GameObject* bumper, sf::Time deltaTime)
{
	if (!object || !bumper)
		return false;

	if (!object->visible || !(bumper->visible))
		return false;

	//ghost is a property that prevents an object from colliding, so by definition we exclude it
	if (object->ghost || (bumper->ghost))
		return false;

	if (!object->isOnScene || !(bumper->isOnScene))
		return false;

	//p0_ : current position of moving object
	//P0_radius : current position with added radius of the circle
	//p1_ : previous position of moving object
	//p2_, p3_, p4_, p5_ : bumper corner points
	const float p0_x = object->getPosition().x;
	const float p0_y = object->getPosition().y;
	const float p1_x = object->getPosition().x - object->speed.x * deltaTime.asSeconds();
	const float p1_y = object->getPosition().y - object->speed.y * deltaTime.asSeconds();
	const float dx = p0_x - p1_x;
	const float dy = p0_y - p1_y;

	const float p2_x = bumper->getPosition().x - bumper->m_size.x / 2;
	const float p2_y = bumper->getPosition().y - bumper->m_size.y / 2;
	const float p3_x = bumper->getPosition().x + bumper->m_size.x / 2;
	const float p3_y = bumper->getPosition().y - bumper->m_size.y / 2;
	const float p4_x = bumper->getPosition().x + bumper->m_size.x / 2;
	const float p4_y = bumper->getPosition().y + bumper->m_size.y / 2;
	const float p5_x = bumper->getPosition().x - bumper->m_size.x / 2;
	const float p5_y = bumper->getPosition().y + bumper->m_size.y / 2;

	const float segment[4][4] = { p2_x, p2_y, p3_x, p3_y, p3_x, p3_y, p4_x, p4_y, p4_x, p4_y, p5_x, p5_y, p5_x, p5_y, p2_x, p2_y };

	//1. collision with borders during movement (projection of the center of the object, interesecting segments)?
	bool segment_is_colliding[4] = { false, false, false, false };
	sf::Vector2f segment_collision_coordinate[4];

	for (int i = 0; i < 4; i++)
	{
		segment_is_colliding[i] = IntersectSegments(p0_x, p0_y, p1_x, p1_y, segment[i][0], segment[i][1], segment[i][2], segment[i][3], &segment_collision_coordinate[i].x, &segment_collision_coordinate[i].y);
	}

	//2. collision with borders on arrival?
	bool segment_is_colliding_on_arrival[4] = { false, false, false, false };
	float segment_collision_at_arrival_distance[4];

	if (segment_is_colliding[0] + segment_is_colliding[1] + segment_is_colliding[2] + segment_is_colliding[3] == 0)
	{
		float dist;
		for (int i = 0; i < 4; i++)
		{
			//collision at arrival? Calculation of distance of point p0 to segment
			dist = DistancePointToSement(p0_x, p0_y, segment[i][0], segment[i][1], segment[i][2], segment[i][3]);
			segment_is_colliding_on_arrival[i] = (dist - object->m_size.x / 2 < 0);
			segment_collision_at_arrival_distance[i] = dist;
		}
	}

	//3. collision with corners?
	bool corner_is_colliding[4] = { false, false, false, false };

	for (int i = 0; i < 4; i++)
	{
		corner_is_colliding[i] = DistancePointToSement(segment[i][0], segment[i][1], p0_x, p0_y, p1_x, p1_y) < object->m_size.x / 2;//object is a circle
	}
	
	if (segment_is_colliding[0] + segment_is_colliding[1] + segment_is_colliding[2] + segment_is_colliding[3] 
		+ segment_is_colliding_on_arrival[0] + segment_is_colliding_on_arrival[1] + segment_is_colliding_on_arrival[2] + segment_is_colliding_on_arrival[3]
		+ corner_is_colliding[0] + corner_is_colliding[1] + corner_is_colliding[2] + corner_is_colliding[3] == 0)
		return false;//no collision
	else
	{
		//choose the closest collision
		float shortest_distance = -1;
		int solution = -1;

		//solutions from 1. (borders on movement)
		for (int i = 0; i < 4; i++)
		{
			if (segment_is_colliding[i])
			{
				float dx = p0_x - segment_collision_coordinate[i].x;
				float dy = p0_y - segment_collision_coordinate[i].y;
				float squared_distance = dx*dx + dy*dy;
				if (shortest_distance < 0 || squared_distance < shortest_distance)
				{
					shortest_distance = squared_distance;
					solution = i;
				}
			}
		}

		//solutions from 2. (borders on arrival)
		for (int i = 0; i < 4; i++)
		{
			if (segment_is_colliding_on_arrival[i])
			{
				float squared_distance = segment_collision_at_arrival_distance[i] * segment_collision_at_arrival_distance[i];
				if (shortest_distance < 0 || squared_distance < shortest_distance)
				{
					shortest_distance = squared_distance;
					solution = i;
				}
			}
		}

		//solutions from 3. (corners)
		for (int i = 0; i < 4; i++)
		{
			if (corner_is_colliding[i])
			{
				float dx = p0_x - segment[i][0];
				float dy = p0_y - segment[i][1];
				float squared_distance = dx*dx + dy*dy;
				if (shortest_distance < 0 || squared_distance < shortest_distance)
				{
					shortest_distance = squared_distance;
					solution = i + 4;
				}
			}
		}

		//reducing speed based on how much the object already penetrated the bumper
		if (solution < 4 && segment_collision_coordinate[solution].x > 0)
		{
			object->speed.x = p0_x - segment_collision_coordinate[solution].x;
			object->speed.y = p0_x - segment_collision_coordinate[solution].y;
		}

		//printf("Collision segment: %d\n", solution);

		object->CollisionResponse(bumper, (CollisionSide)solution);
		//0 : top
		//1 : right
		//2 : bottom
		//3 : left
		//4 : upper left corner
		//5 : upper right corner
		//6 : bottom right corner
		//7 : bottom left corner

		return true;
	}
}

void GameObject::CollisionResponse(GameObject* bumper, CollisionSide collision)
{
	//see override function in class Ship and Discoball
}

void GameObject::CollisionResponse(GameObject* bumper, CollisionSide collision, bool bouncing)
{
	if (bumper)
	{
		switch (collision)
		{
			case NoCollision:
				break;
			case Collision_Top:
			{
				setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y - bumper->m_size.y / 2 - m_size.y / 2));
				speed.y *= -1 * bouncing;
				break;
			}
			case Collision_Bottom:
			{
				setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y + bumper->m_size.y / 2 + m_size.y / 2));
				speed.y *= -1 * bouncing;
				break;
			}
			case Collision_Right:
			{
				setPosition(sf::Vector2f(bumper->getPosition().x + bumper->m_size.x / 2 + m_size.x / 2, getPosition().y));
				speed.x *= -1 * bouncing;
				break;
			}
			case Collision_Left:
			{
				setPosition(sf::Vector2f(bumper->getPosition().x - bumper->m_size.x / 2 - m_size.x / 2, getPosition().y));
				speed.x *= -1 * bouncing;
				break;
			}
			case Collision_TopLeft:
			{
				setPosition(sf::Vector2f(bumper->getPosition().x - bumper->m_size.x / 2 - m_size.x / 2, bumper->getPosition().y - bumper->m_size.y / 2 - m_size.y / 2));
				speed.x *= -1 * bouncing;
				speed.y *= -1 * bouncing;
				break;
			}
			case Collision_TopRight:
			{
				setPosition(sf::Vector2f(bumper->getPosition().x + bumper->m_size.x / 2 + m_size.x / 2, bumper->getPosition().y - bumper->m_size.y / 2 - m_size.y / 2));
				speed.x *= -1 * bouncing;
				speed.y *= -1 * bouncing;
				break;
			}
			case Collision_BottomRight:
			{
				setPosition(sf::Vector2f(bumper->getPosition().x + bumper->m_size.x / 2 + m_size.x / 2, bumper->getPosition().y + bumper->m_size.y / 2 + m_size.y / 2));
				speed.x *= -1 * bouncing;
				speed.y *= -1 * bouncing;
				break;
			}
			case Collision_BottomLeft:
			{
				setPosition(sf::Vector2f(bumper->getPosition().x - bumper->m_size.x / 2 - m_size.x / 2, bumper->getPosition().y + bumper->m_size.y / 2 + m_size.y / 2));
				speed.x *= -1 * bouncing;
				speed.y *= -1 * bouncing;
				break;
			}
		}
	}
}

bool GameObject::isCapsuleCollidingDuringMovement(GameObject* object, float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y)
{
	const float theta = GetAngleRadForSpeed(object->speed);
	const float angle_top = M_PI - theta + M_PI_2 - (p1_x == p2_x) * M_PI; // p1_x == p2_x if horizontal, else is vertical (diagonal not supported)
	const float angle_bottom = angle_top - M_PI;
	const float offset_top_x = object->m_size.x / 2 * sin(angle_top);
	const float offset_top_y = -object->m_size.x / 2 * cos(angle_top);
	const float offset_bottom_x = object->m_size.x / 2 * sin(angle_bottom);
	const float offset_bottom_y = -object->m_size.x / 2 * cos(angle_bottom);

	const float p0_Topx = p0_x + offset_top_x;
	const float p0_Topy = p0_y + offset_top_y;
	const float p1_Topx = p1_x + offset_top_x;
	const float p1_Topy = p1_y + offset_top_y;
	const float p0_Botx = p0_x + offset_bottom_x;
	const float p0_Boty = p0_y + offset_bottom_y;
	const float p1_Botx = p1_x + offset_bottom_x;
	const float p1_Boty = p1_y + offset_bottom_y;

	if (IntersectSegments(p0_Topx, p0_Topy, p1_Topx, p1_Topy, p2_x, p2_y, p3_x, p3_y)
		|| IntersectSegments(p0_Botx, p0_Boty, p1_Botx, p1_Boty, p2_x, p2_y, p3_x, p3_y))
	{
		//avoids to count a collision when leaving a position that was pixel-perfectly-not in collision
		if (DistancePointToSement(p1_Topx, p1_Topy, p2_x, p2_y, p3_x, p3_y) > 0 && DistancePointToSement(p1_Botx, p1_Boty, p2_x, p2_y, p3_x, p3_y) > 0)
		{
			//printf("Collision on capsule movement\n");
			return true;
		}
	}

	return false;
}