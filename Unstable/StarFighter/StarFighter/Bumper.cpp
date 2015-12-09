#include "Bumper.h"

extern Game* CurrentGame;

using namespace sf;

Bumper::Bumper()
{

}

Bumper::Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_type = type;
}

Bumper::Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size) : GameObject(position, sf::Vector2f(0, 0), textureName, size)
{
	m_type = type;
}

Bumper::Bumper(BumperType type, sf::Vector2f position, sf::Vector2f size)
{
	m_type = type;

	//pixel array creation
	const int W = size.x;
	const int H = size.y;

	m_stroke_size = BUMPER_STROKE_SIZE;

	ostringstream ss;
	ss << "bumper";
	sf::Color color;

	if (type == OnlyBlueTeamThrough)
	{
		ss << "_blue";
		m_collider_type = BumperBlueObject;
		color = { 0, 0, 255, 255 };
	}
	else if (type == OnlyRedTeamThrough)
	{
		ss << "_red";
		m_collider_type = BumperRedObject;
		color = { 255, 0, 0, 255 };
	}
	else
	{
		ss << "_green";
		m_collider_type = BumperGreenObject;
		color = { 0, 255, 0, 255 };
	}

	sf::Uint8* pixels = CreateRectangleWithStroke(size, color, m_stroke_size);

	//automatic naming of the texture for a unique identification
	ss << size.x << "x" << size.y;
	std::string s = ss.str();

	Init(position, sf::Vector2f(0, 0), s, sf::Vector2f(W, H), 1, 1, pixels);

	//Add outter glow effect
	if (BUMPER_GLOW_RADIUS > 0)
	{
		m_glow_effect = new Glow(this, color, BUMPER_GLOW_RADIUS, m_stroke_size, BUMPER_GLOW_ANIMATION_DURATION, BUMPER_GLOW_MIN_RADIUS);
	}
	else
	{
		m_glow_effect = NULL;
	}
}

Bumper::~Bumper()
{
	
}

void Bumper::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);

	if (m_glow_effect)
		m_glow_effect->setPosition(getPosition());
}

void Bumper::CollisionResponse(Time deltaTime)
{
	//start a new animation
	if (m_glow_effect->m_glow_status != GlowHitAnimation)
	{
		m_glow_effect->m_glow_status = GlowHitAnimation;
		if (m_glow_effect->m_frameNumber > 1)
			m_glow_effect->m_currentFrame = 1;
	}

	AnimatedSprite::update(deltaTime);
}