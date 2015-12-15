#include "Goal.h"

extern Game* CurrentGame;

using namespace sf;

Goal::Goal()
{

}

Goal::Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_team = team;
}

Goal::Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size) : GameObject(position, sf::Vector2f(0, 0), textureName, size)
{
	m_team = team;
}

Goal::Goal(Teams team, sf::Vector2f position, sf::Vector2f size)
{
	m_team = team;

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

	sf::Uint8* pixels = CreateRectangleWithStroke(size, color, m_stroke_size);

	sf::Uint8* pixels_highlight = CreateRectangleWithStroke(size, color, m_stroke_size);

	//automatic naming of the texture for a unique identification
	ss << size.x << "x" << size.y;
	std::string s = ss.str();

	Init(position, sf::Vector2f(0, 0), s, sf::Vector2f(W, H), 1, 2, pixels);

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
	//start a new animation
	if (m_glow_effect)
	{
		if (m_glow_effect->m_glow_status != GlowHitAnimation)
		{
			m_glow_effect->m_glow_status = GlowHitAnimation;
			if (m_glow_effect->m_frameNumber > 1)
				m_glow_effect->m_currentFrame = 1;
		}
	}

	AnimatedSprite::update(deltaTime);
}