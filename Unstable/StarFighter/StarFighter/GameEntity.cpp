#include "GameEntity.h"

extern Game* CurrentGame;

GameEntity::GameEntity(UI_Type ui_type)
{
	m_UI_type = ui_type;
	m_hovered = false;
	m_selected = false;
	m_default_color = sf::Color::White;
	m_ship_offset = sf::Vector2f(0.f, 0.f);
}

GameEntity::GameEntity(sf::Vector2f size, UI_Type ui_type) : GameEntity(ui_type)
{
	m_size = size;
}

void GameEntity::UpdatePosition()
{
	m_shape_container.setPosition(m_position + m_ship_offset);
	m_shape.setPosition(m_position + m_ship_offset);
	m_text.SetPosition(m_position + m_ship_offset);
	setPosition(m_position + m_ship_offset);
}

void GameEntity::Update(Time deltaTime)
{
	//update position
	UpdatePosition();

	//get inputs
	MouseAction& mouse_click = (*CurrentGame).m_mouse_click;

	//get hovered state
	if (IsHoveredByMouse() == true)
	{
		if ((*CurrentGame).m_hovered_ui != NULL && (*CurrentGame).m_hovered_ui != this)
		{
			(*CurrentGame).m_hovered_ui->m_hovered = false;
			(*CurrentGame).m_hovered_ui->m_shape_container.setOutlineColor(m_default_color);
		}
		m_hovered = true;
		(*CurrentGame).m_hovered_ui = this;
	}
	else
	{
		m_hovered = false;
	}

	//get selected state
	if (mouse_click == Mouse_LeftClick)
	{
		if (m_hovered == true && m_UI_type != UI_WaterTile && m_UI_type != UI_Room && m_UI_type != UI_RoomTile && m_UI_type != UI_Island)//types that cannot be selected
		{
			if ((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui != this)
			{
				(*CurrentGame).m_selected_ui->m_selected = false;
				(*CurrentGame).m_selected_ui->m_shape_container.setOutlineColor(m_default_color);
			}
			
			(*CurrentGame).m_selected_ui = this;
			m_selected = true;
		}
		else
		{
			m_selected = false;
		}
	}

	//apply color feedback
	m_shape_container.setOutlineColor(m_default_color);
	m_shape_container.setOutlineThickness(-1.f);

	if (m_hovered)
	{
		m_shape_container.setOutlineColor(sf::Color::Red);
		m_shape_container.setOutlineThickness(2.f);
	}
	if (m_selected)
	{
		m_shape_container.setOutlineColor(sf::Color::Green);
		m_shape_container.setOutlineThickness(2.f);
	}

	AnimatedSprite::update(deltaTime);
}

void GameEntity::Draw(sf::RenderTexture& screen)
{
	screen.draw(this->m_shape_container);
	screen.draw(this->m_shape);
	screen.draw(this->m_text);
	screen.draw(*this);
};

bool GameEntity::IsHoveredByMouse()
{
	if ((*CurrentGame).m_window_has_focus == true
		&& (((*CurrentGame).m_mouse_pos.x > m_shape_container.getPosition().x - m_shape_container.getSize().x / 2 && (*CurrentGame).m_mouse_pos.x < m_shape_container.getPosition().x + m_shape_container.getSize().x / 2
				&& (*CurrentGame).m_mouse_pos.y > m_shape_container.getPosition().y - m_shape_container.getSize().y / 2 && (*CurrentGame).m_mouse_pos.y < m_shape_container.getPosition().y + m_shape_container.getSize().y / 2)
			|| ((*CurrentGame).m_mouse_pos.x > getPosition().x - m_size.x / 2 && (*CurrentGame).m_mouse_pos.x < getPosition().x + m_size.x / 2
				&& (*CurrentGame).m_mouse_pos.y > getPosition().y - m_size.y / 2 && (*CurrentGame).m_mouse_pos.y < getPosition().y + m_size.y / 2)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GameEntity::setAnimation(sf::Texture *texture, int frameNumber, int animationNumber)
{
	m_animationNumber = animationNumber;
	m_frameNumber = frameNumber;

	m_size.x = ((*texture).getSize().x / frameNumber);
	m_size.y = ((*texture).getSize().y / animationNumber);
	setOrigin(sf::Vector2f(m_size.x / 2, m_size.y / 2));

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
}

void GameEntity::setAnimationLine(int animation, bool keep_frame_index)
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