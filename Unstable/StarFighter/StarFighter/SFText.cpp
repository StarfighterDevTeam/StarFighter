#include "SFText.h"

// TEXTS
SFText::SFText(const sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team)
{
	setFont(*font);
	setCharacterSize(size);
	setColor(color);
	setPosition(position);

	m_visible = true;
	m_team = team;
	m_alliance = AllianceNeutral;

	m_GarbageMe = false;
}

void SFText::update(Time deltaTime)
{
	//see override function in class SFTextPop
}

SFText* SFText::Clone()
{
	SFText* clone = new SFText(getFont(), getCharacterSize(), getColor(), getPosition(), m_team);
	clone->m_alliance = this->m_alliance;
	clone->setString(getString());

	return clone;
}

// RECTANGLES
SFRectangle::SFRectangle(sf::Vector2f position, sf::Vector2f size, sf::Color color, float outline_thickness, sf::Color outline_color, PlayerTeams team)
{
	setPosition(position);
	setSize(size);
	setOrigin(size.x / 2, size.y/2);
	setFillColor(color);
	m_color = color;
	setOutlineThickness(outline_thickness);
	setOutlineColor(outline_color);

	m_visible = true;
	m_team = team;
	m_alliance = (TeamAlliances)0;

	m_GarbageMe = false;
}

SFRectangle* SFRectangle::Clone()
{
	SFRectangle* clone = new SFRectangle(getPosition(), getSize(), m_color, getOutlineThickness(), getOutlineColor(), m_team);
	clone->m_layer = m_layer;

	return clone;
}

SFGauge::SFGauge(SFText* text, SFRectangle* gauge, SFRectangle* gauge_container)
{
	//text
	setFont(*text->getFont());
	setCharacterSize(text->getCharacterSize());
	setColor(text->getColor());
	setString(text->getString());
	SFText::setPosition(text->getPosition());

	m_visible = text->m_visible;
	m_team = text->m_team;
	m_alliance = text->m_alliance;

	m_GarbageMe = false;

	//gauges
	m_gauge = gauge;
	m_container = gauge_container;
	m_curValue = 0;
}

// SF GAUGE

SFGauge::~SFGauge()
{
	if (m_container)
	{
		m_container->m_visible = false;
		m_container->m_GarbageMe = true;
	}
	if (m_gauge)
	{
		m_gauge->m_visible = false;
		m_gauge->m_GarbageMe = true;
	}
}

void SFGauge::setVisible(bool visible)
{
	m_visible = visible;
	m_gauge->m_visible = visible;
	m_container->m_visible = visible;
}

void SFGauge::setVisible(bool text_visible, bool rectangle_visible)
{
	m_visible = text_visible;
	m_gauge->m_visible = rectangle_visible;
	m_container->m_visible = rectangle_visible;
}

void SFGauge::setPosition(sf::Vector2f position)
{
	m_gauge->setPosition(sf::Vector2f(position.x + m_offset.x, position.y + m_offset.y));
	m_container->setPosition(sf::Vector2f(position.x + m_offset.x, position.y + m_offset.y));
	SFText::setPosition(sf::Vector2f(m_container->getPosition().x - getGlobalBounds().width / 2, m_container->getPosition().y - m_container->getSize().y / 2 - GAUGE_TEXT_OFFSET_Y));
}

void SFGauge::update(unsigned int curValue, unsigned int maxValue)
{
	m_curValue = curValue;
	m_maxValue = maxValue;
	if (maxValue == 0 || curValue > maxValue)
	{
		m_gauge->setSize(sf::Vector2f(m_container->getSize().x, m_container->getSize().y));
	}
	else
	{
		m_gauge->setSize(sf::Vector2f(1.0f * curValue / maxValue * m_container->getSize().x, m_gauge->getSize().y));
	}
}

void SFGauge::draw(RenderTexture& screen)
{
	if (m_visible)
	{
		if (m_container->m_visible)
			screen.draw(*m_container);

		if (m_gauge->m_visible)
			screen.draw(*m_gauge);

		screen.draw(*this);
	}
}

SFGauge* SFGauge::Clone()
{
	//SFGauge* clone = new SFGauge();
	SFGauge* clone = new SFGauge(this->SFText::Clone(), this->m_gauge->Clone(), this->m_container->Clone());
	//const sf::Font* font = this->sf::Text::getFont();
	//setFont(*font);
	//setCharacterSize(this->getCharacterSize());
	//setColor(this->getColor());
	//setString(this->getString());
	//SFText::setPosition(this->getPosition());

	//m_visible = this->m_visible;
	//m_team = this->m_team;
	//m_alliance = this->m_alliance;
	//
	//m_GarbageMe = false;

	//gauges
	//m_gauge = this->m_gauge->Clone();
	//m_container = this->m_container->Clone();
	m_curValue = this ->m_curValue;
	m_maxValue = this->m_maxValue;

	return clone;
}

void SFGauge::UpdateFillColor(Color fill_color)
{
	m_gauge->setFillColor(fill_color);
}

void SFGauge::UpdateOutlineColor(Color outline_color)
{
	m_container->setOutlineColor(outline_color);
}

void SFGauge::UpdateTextColor(Color text_color)
{
	setColor(text_color);
}