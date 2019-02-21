#include "Choice.h"

extern Game* CurrentGame;

Choice::Choice()
{
	m_panel = NULL;
	m_picture = NULL;
}

Choice::~Choice()
{
	delete m_panel;
	delete m_picture;
}

void Choice::Destroy()
{
	delete m_panel;
	delete m_picture;

	m_panel = NULL;
	m_picture = NULL;
}

void Choice::Init(int index, string text, string portrait_filename)
{
	m_position = sf::Vector2f(0, 0);

	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(CHOICE_PANEL_SIZE_X, CHOICE_PANEL_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(CHOICE_PANEL_SIZE_X * 0.5f, CHOICE_PANEL_SIZE_Y * 0.5f));
	m_panel->m_shape_container.setFillColor(sf::Color::White);
	m_panel->m_shape_container.setOutlineThickness(4.f);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(0.f, 0.f));

	ostringstream ss_index;
	ss_index << index + 1 << ".";
	m_panel->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_panel->m_text.setCharacterSize(20);
	m_panel->m_text.setColor(sf::Color::White);
	m_panel->m_text.setString(ss_index.str());
	m_panel->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5f - 34.f, 0.f - m_panel->m_text.getGlobalBounds().height * 0.5f);

	m_picture = new GameEntity(UI_None);
	m_picture->m_shape_container.setSize(sf::Vector2f(CHOICE_VIDEO_SIZE_X, CHOICE_PANEL_SIZE_Y));
	m_picture->m_shape_container.setOrigin(sf::Vector2f(CHOICE_VIDEO_SIZE_X * 0.5f, CHOICE_PANEL_SIZE_Y * 0.5f));
	m_picture->m_shape_container.setFillColor(sf::Color::White);
	m_picture->m_shape_container.setOutlineThickness(4.f);
	m_picture->m_shape_container.setOutlineColor(sf::Color::Black);
	m_picture->m_shape_container.setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5f + CHOICE_VIDEO_SIZE_X * 0.5f, 0.f));

	m_picture->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_picture->m_text.setCharacterSize(20);
	m_picture->m_text.setColor(sf::Color::Black);
	m_picture->m_text.setString(text);
	m_picture->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5f + CHOICE_VIDEO_SIZE_X + 20, 0.f - CHOICE_PANEL_SIZE_Y * 0.5f + m_picture->m_text.getGlobalBounds().height * 0.5f + 10);

	Texture* texture = TextureLoader::getInstance()->loadTexture(portrait_filename, 280, 300);
	m_picture->setAnimation(texture, 1, 2);
	m_picture->setAnimationLine(1);
	m_picture->setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5f + CHOICE_VIDEO_SIZE_X * 0.5f, 0.f));
}

void Choice::SetPosition(sf::Vector2f position)
{
	sf::Vector2f offset = position - m_position;

	m_panel->m_shape_container.setPosition(m_panel->m_shape_container.getPosition() + offset);
	m_panel->m_text.setPosition(m_panel->m_text.getPosition() + offset);
	m_picture->m_shape_container.setPosition(m_picture->m_shape_container.getPosition() + offset);
	m_picture->m_text.setPosition(m_picture->m_text.getPosition() + offset);
	m_picture->setPosition(m_picture->getPosition() + offset);
	m_title.setPosition(m_title.getPosition() + offset);
	m_body.setPosition(m_body.getPosition() + offset);
}

bool Choice::Update(sf::Time deltaTime)
{
	if (m_panel->IsHoveredByMouse() == true)
	{
		m_picture->setAnimationLine(0);

		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			return true;
		}
	}
	else if (m_picture->m_currentAnimationIndex == 0)
	{
		m_picture->setAnimationLine(1);
	}

	return false;
}

void Choice::Draw(sf::RenderTexture& screen)
{
	screen.draw(m_panel->m_shape_container);
	screen.draw(m_panel->m_text);
	screen.draw(m_picture->m_shape_container);
	screen.draw(m_picture->m_text);
	m_picture->Draw(screen);
	screen.draw(m_title);
	screen.draw(m_body);
}