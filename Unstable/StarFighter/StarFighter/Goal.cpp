#include "Goal.h"

extern Game* CurrentGame;

using namespace sf;

Goal::Goal()
{
	m_hit = false;
}

Goal::Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_team = team;
	m_hit = false;
}

Goal::Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size) : GameObject(position, sf::Vector2f(0, 0), textureName, size)
{
	m_team = team;
	m_hit = false;
}

Goal::Goal(Teams team, sf::Vector2f position, sf::Vector2f size)
{
	m_team = team;
	m_hit = false;

	//pixel array creation
	const int W = size.x;
	const int H = size.y;

	m_stroke_size = GOAL_STROKE_SIZE;

	ostringstream ss;
	ss << "goal";

	sf::Color color;

	if (m_team == BlueTeam)
	{
		ss << "_blue";
		m_collider_type = GoalBlueObject;
		color = { 0, 0, 255, 255 };
	}
	else//if (m_team == RedTeam)
	{
		ss << "_red";
		m_collider_type = GoalRedObject;
		color = { 255, 0, 0, 255 };
	}

	sf::Uint8* pixels_normal = CreateRectangleWithStroke(size, color, m_stroke_size);

	//creation of a highlighted version for hit feedback
	sf::Color color_highlight = { 0, 255, 0, 255 };
	sf::Uint8* pixels_highlight = CreateRectangleWithStroke(size, color_highlight, m_stroke_size);

	//automatic naming of the texture for a unique identification
	ss << size.x << "x" << size.y;
	std::string s = ss.str();
	this->m_textureName = s;

	//checking if texture laready exists. if not, create a fake one, to be filled few lines of code below
	//we want to create two animations: normal, and highlighted
	sf::Uint8* pixels_animation = new sf::Uint8[W * H * 4 * 2];
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	bool existing_texture = loader->getTexture(s);
	sf::Texture* texture = loader->loadTexture(s, W, H*2, pixels_animation);

	//updating the newly created texture, if it wasn't existing before
	if (!existing_texture)
	{
		//creating a transparent default background
		for (int p = 0; p < W * H * 4 * 2; p++)
		{
			pixels_animation[p] = 0;
			pixels_animation[p + 1] = 0;
			pixels_animation[p + 2] = 0;
			pixels_animation[p + 3] = 0;
		}
		texture->update(pixels_animation, W, H*2, 0, 0);

		//create one frame for each of the 2 animations
		texture->update(pixels_normal, W, H, 0, 0);
		texture->update(pixels_highlight, W, H, 0, H);
	}

	setOrigin(W / 2, H / 2);
	Init(position, sf::Vector2f(0, 0), texture, 1, 2);

	//Add outter glow effect
	if (GOAL_GLOW_RADIUS > 0)
	{
		m_glow_effect = new Glow(this, color, GOAL_GLOW_RADIUS, m_stroke_size, GOAL_GLOW_ANIMATION_DURATION, GOAL_GLOW_MIN_RADIUS);
	}
	else
	{
		m_glow_effect = NULL;
	}
}

Goal::~Goal()
{
	
}

void Goal::CollisionResponse(Time deltaTime)
{
	//start a new glow animation
	if (m_glow_effect)
	{
		if (m_glow_effect->m_glow_status != GlowHitAnimation)
		{
			m_glow_effect->m_glow_status = GlowHitAnimation;
			if (m_glow_effect->m_frameNumber > 1)
				m_glow_effect->m_currentFrame = 1;
		}
	}

	//changing goal color
	setAnimationLine(GoalHighlighted, true);

	//flag
	m_hit = true;
	m_hit_clock.restart();

	AnimatedSprite::update(deltaTime);
}

void Goal::update(Time deltaTime)
{
	if (m_hit && m_hit_clock.getElapsedTime().asSeconds() > DELAY_FOR_GOAL_FADE_OUT)
	{
		GarbageMe = true;
		if (m_glow_effect)
			m_glow_effect->GarbageMe = true;
	}
	AnimatedSprite::update(deltaTime);
}