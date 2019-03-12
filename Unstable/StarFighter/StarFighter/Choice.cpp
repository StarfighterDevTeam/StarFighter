#include "Choice.h"

extern Game* CurrentGame;

Choice::Choice()
{
	m_panel = NULL;
	m_picture = NULL;
	m_gauge = NULL;
}

Choice::~Choice()
{
	delete m_panel;
	delete m_picture;
	delete m_gauge;
}

void Choice::Destroy()
{
	delete m_panel;
	delete m_picture;
	delete m_gauge;

	m_panel = NULL;
	m_picture = NULL;
	m_gauge = NULL;
}

void Choice::Load(int index, int choiceID)
{
	if (choiceID < 2)
	{
		int ID = stoi((*CurrentGame).m_choices_config[choiceID][Choice_ID]);

		string text = (*CurrentGame).m_choices_config[choiceID][Choice_Text];
		text = StringReplace(text, "_", " ");

		string picture_name = (*CurrentGame).m_choices_config[choiceID][Choice_Picturename];

		int skill = stoi((*CurrentGame).m_choices_config[choiceID][Choice_Skill]);

		int value_max = stoi((*CurrentGame).m_choices_config[choiceID][Choice_ValueMax]);

		Init(index, text, picture_name, skill, value_max);
	}
}

void Choice::Init(int index, string text, string portrait_filename, int skill, int value_max)
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
	m_panel->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5f - 34, 0.f - m_panel->m_text.getGlobalBounds().height * 0.5);

	m_picture = new GameEntity(UI_None);
	m_picture->m_shape_container.setSize(sf::Vector2f(CHOICE_VIDEO_SIZE_X, CHOICE_PANEL_SIZE_Y));
	m_picture->m_shape_container.setOrigin(sf::Vector2f(CHOICE_VIDEO_SIZE_X * 0.5f, CHOICE_PANEL_SIZE_Y * 0.5f));
	m_picture->m_shape_container.setFillColor(sf::Color::White);
	m_picture->m_shape_container.setOutlineThickness(4.f);
	m_picture->m_shape_container.setOutlineColor(sf::Color::Black);
	m_picture->m_shape_container.setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X * 0.5, 0.f));

	m_picture->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_picture->m_text.setCharacterSize(20);
	m_picture->m_text.setColor(sf::Color::Black);
	m_picture->m_text.setString(text);
	m_picture->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X + 20, 0.f - CHOICE_PANEL_SIZE_Y * 0.5 + m_picture->m_text.getGlobalBounds().height * 0.5 + 10);

	Texture* texture = TextureLoader::getInstance()->loadTexture(portrait_filename, 280, 300);
	m_picture->setAnimation(texture, 1, 2);
	m_picture->setAnimationLine(1);
	m_picture->setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X * 0.5, 0.f));

	m_gauge = new GameEntity(UI_None);
	m_gauge->m_shape_container.setSize(sf::Vector2f(CHOICE_GAUGE_SIZE_X, CHOICE_GAUGE_SIZE_Y));
	m_gauge->m_shape_container.setOrigin(sf::Vector2f(CHOICE_GAUGE_SIZE_X * 0.5, CHOICE_GAUGE_SIZE_Y * 0.5f));
	m_gauge->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_DarkGrey_Background]);
	m_gauge->m_shape_container.setOutlineThickness(4.f);
	m_gauge->m_shape_container.setOutlineColor(sf::Color::Black);
	m_gauge->m_shape_container.setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X + 20 + CHOICE_GAUGE_SIZE_X * 0.5, 0.f - CHOICE_GAUGE_SIZE_Y * 0.5 + 40));

	m_gauge_value_max = value_max;
	m_gauge_value = 0;
	m_skill = skill;
	if (value_max > 0)
	{
		m_gauge->m_shape.setSize(sf::Vector2f(1.0f * CHOICE_GAUGE_SIZE_X * m_gauge_value / m_gauge_value_max, CHOICE_GAUGE_SIZE_Y));
		m_gauge->m_shape.setOrigin(sf::Vector2f(m_gauge->m_shape.getSize().x * 0.5, CHOICE_GAUGE_SIZE_Y * 0.5f));
		m_gauge->m_shape.setFillColor(sf::Color::Green);
		m_gauge->m_shape.setOutlineThickness(4.f);
		m_gauge->m_shape.setOutlineColor(sf::Color::Black);
		m_gauge->m_shape.setPosition(sf::Vector2f(m_gauge->m_shape_container.getPosition().x - CHOICE_GAUGE_SIZE_X * 0.5 + m_gauge->m_shape.getSize().x * 0.5, m_gauge->m_shape_container.getPosition().y));
	
		if (skill >= 0 && skill < NB_CREW_SKILLS)
		{
			m_gauge->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_gauge->m_text.setCharacterSize(20);
			m_gauge->m_text.setColor(sf::Color::Black);
			m_gauge->m_text.setString((*CurrentGame).m_dico_crew_skills[skill]);
			m_gauge->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X + 20, 0.f - CHOICE_GAUGE_SIZE_Y + 30 - m_gauge->m_text.getCharacterSize() - 10);
			m_gauge_string = m_gauge->m_text.getString();
		}
	}
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
	m_gauge->m_shape_container.setPosition(m_gauge->m_shape_container.getPosition() + offset);
	m_gauge->m_shape.setPosition(m_gauge->m_shape.getPosition() + offset);
	m_gauge->m_text.setPosition(m_gauge->m_text.getPosition() + offset);
}

bool Choice::Update(int gauge_value)
{
	bool hovering_on = false;
	if (m_panel->IsHoveredByMouse() == true)
	{
		m_picture->setAnimationLine(0);
		hovering_on = true;
	}
	else if (m_picture->m_currentAnimationIndex == 0)
	{
		m_picture->setAnimationLine(1);
	}

	//text
	ostringstream ss;
	ss << m_gauge_string << " " << m_gauge_value << "%";
	m_gauge->m_text.setString(ss.str());

	if (m_gauge_value_max > 0)
	{
		m_gauge_value = gauge_value;
		m_gauge->m_shape.setSize(sf::Vector2f(1.0f * CHOICE_GAUGE_SIZE_X * gauge_value / m_gauge_value_max, CHOICE_GAUGE_SIZE_Y));
		m_gauge->m_shape.setOrigin(sf::Vector2f(m_gauge->m_shape.getSize().x * 0.5, m_gauge->m_shape.getSize().y * 0.5));
		m_gauge->m_shape.setPosition(sf::Vector2f(m_gauge->m_shape_container.getPosition().x - CHOICE_GAUGE_SIZE_X * 0.5 + m_gauge->m_shape.getSize().x * 0.5, m_gauge->m_shape_container.getPosition().y));
	}

	return hovering_on;
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
	screen.draw(m_gauge->m_shape_container);
	if (m_gauge_value > 0)
	{
		screen.draw(m_gauge->m_shape);
	}
	screen.draw(m_gauge->m_text);
}