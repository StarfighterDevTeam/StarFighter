#include "GameObject.h"

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : AnimatedSprite()
{
	Init(position, speed, textureName, size, frameNumber, animationNumber);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : AnimatedSprite()
{
	Init(position, speed, textureName, size);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture) : AnimatedSprite()
{
	Init(position, speed, texture, 1);
}

GameObject::GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, int stroke_size)
{
	Init(position, speed, color, size, stroke_size);
}

string GameObject::GetTextureName()
{
	vector<string> s1 = TextUtils::split(m_textureName, '/');
	return *(s1.end() - 1);
}

GameObject::GameObject()
{

}

void GameObject::SetAnimationLine(int animation, bool keep_frame_index)
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

	if (m_currentAnimation)
		delete m_currentAnimation;

	m_currentAnimation = anim;
	anim = NULL;
	play(*m_currentAnimation, keep_frame_index);
	m_currentAnimationIndex = animation;
	m_heading = 0;
}

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber, int animationNumber)
{
	m_animationNumber = animationNumber;
	m_frameNumber = frameNumber;
	m_size.x = ((*texture).getSize().x / frameNumber);
	m_size.y = ((*texture).getSize().y / animationNumber);
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
	SetAnimationLine(0);//default starting animation is line 0 (top of the sprite sheet)
	
	m_speed = speed;
	m_position = position;
	m_visible = true;
	m_garbageMe = false;
	m_removeMe = false;
	m_radius = (float)sqrt(((m_size.x / 2)*(m_size.x / 2)) + ((m_size.y / 2)*(m_size.y / 2)));
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

void GameObject::Init(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, int stroke_size)
{
	sf::Uint8* pixels = GameObject::CreateRectangleWithStroke(size, color, stroke_size);
	ostringstream ss;
	ss << "rectangle_" << (int)size.x << "x" << (int)size.y << "_" << (int)color.r << "_" << (int)color.g << "_" << (int)color.b << "_" << (int)color.a;
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

	//delete equipment_loot;
	//delete weapon_loot;
}

void GameObject::Update(sf::Time deltaTime)
{
	m_previous_speed = m_speed;
	
	m_position.x += m_speed.x * deltaTime.asSeconds();
	m_position.y += m_speed.y * deltaTime.asSeconds();

	UpdateStarSectorIndex();

	BoundAngle(m_heading, 360);
	setRotation(m_heading);

	if (m_frameNumber > 1)
	{
		AnimatedSprite::Update(deltaTime);
	}
}

void GameObject::Draw(RenderTarget& screen)
{
	if (m_visible == true)
		screen.draw(*this);
}

void GameObject::UpdateAnimation(sf::Time deltaTime)
{
	AnimatedSprite::Update(deltaTime);
}

GameObject* GameObject::Clone()
{
	GameObject* clone = new GameObject(getPosition(), m_speed, m_textureName, m_size, sf::Vector2f(m_size.x/2, m_size.y/2), m_frameNumber, m_animationNumber);
	clone->m_collider = m_collider;
	clone->m_layer = m_layer;

	return clone;
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

int GameObject::GaussianBlurDistribution(int x)
{
	return x*x;
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

void GameObject::SetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle)
{
	m_speed.x = -absolute_speed * sin(curAngle);
	m_speed.y = -absolute_speed * cos(curAngle);
}

sf::Vector2f GameObject::GetSpeedVectorFromAbsoluteSpeedAndAngle(float absolute_speed, float curAngle)
{
	sf::Vector2f speed;
	speed.x = -absolute_speed * sin(curAngle);
	speed.y = -absolute_speed * cos(curAngle);
	return speed;
}

//Star Hunter
void GameObject::SetStarSectorIndex(sf::Vector2i sector_index)
{
	m_star_sector_index = sector_index;

	m_position.x = 1.f * m_star_sector_index.x * STAR_SECTOR_SIZE;
	m_position.y = 1.f * m_star_sector_index.y * STAR_SECTOR_SIZE;
}

void GameObject::UpdateStarSectorIndex()
{
	m_star_sector_index.x = (int)(m_position.x / STAR_SECTOR_SIZE + (m_position.x >= 0 ? 0.5 : -0.5));
	m_star_sector_index.y = (int)(m_position.y / STAR_SECTOR_SIZE + (m_position.y >= 0 ? 0.5 : -0.5));
}

bool GameObject::GetHitByAmmo(GameObject* ammo)
{
	//see override function in class Ship
	return false;
}