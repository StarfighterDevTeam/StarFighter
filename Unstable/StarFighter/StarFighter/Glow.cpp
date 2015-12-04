#include "Glow.h"

extern Game* CurrentGame;

using namespace sf;

Glow::Glow(GameObject* parent, sf::Color color, int glow_thickness)
{
	assert(parent != NULL);

	const int W = parent->m_size.x + glow_thickness*2;
	const int H = parent->m_size.y + glow_thickness*2;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	ostringstream ss;

	collider_type = BackgroundObject;
	m_color = color;

	for (int i = 0; i < W * H * 4; i += 4)
	{
		pixels[i] = color.r;		// R
		pixels[i + 1] = color.g;	// G
		pixels[i + 2] = color.b;	// B

		if (glow_thickness < 1 || (i / 4) <= W * glow_thickness || (i / 4) >(H - 1 * glow_thickness)*W || (i / 4) % W <= 0 + (glow_thickness - 1) || (i / 4) % W >= (W - 1 * glow_thickness)) // A
		{
			pixels[i + 3] = 255;
		}
		else
		{
			pixels[i + 3] = 0;
		}
	}
	
	if (W > H)
	{
		ss << "_H_" << H;
	}
	else if (W < H)
	{
		ss << "_V_" << H;
	}
	else
	{
		ss << "_S";
	}

	std::string s = ss.str();

	//Add outter glow effect
	GlowEffect(glow_thickness, pixels, W, H);

	Init(parent->getPosition(), sf::Vector2f(0, 0), s, sf::Vector2f(W, H), 1, 1, pixels);
}

Glow::~Glow()
{
	
}

void Glow::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}