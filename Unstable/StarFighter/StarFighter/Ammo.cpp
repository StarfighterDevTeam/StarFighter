#include "Ammo.h"

extern Game* CurrentGame;


//DEBUG
void Ammo::Draw(sf::RenderTexture& screen)
{
	///*DEBUG*/
	//Text text;
	//text.setFont(*(*CurrentGame).m_font[0]);
	//text.setCharacterSize(14);
	//text.setColor(sf::Color::White);
	//text.setPosition(sf::Vector2f(getPosition().x - 50, getPosition().y - 70));
	//ostringstream ss;
	//
	//ss << "\nheading: " << to_string((int)getRotation());// << " / offy: " << to_string(m_pattern.m_offset.y) << " / spd: " << to_string(int(m_pattern.m_speed));

	//missile locking position feedback
	if (m_is_missile_model == true && m_missile_target_object != NULL)
	{
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(20, 20));
		rect.setOrigin(sf::Vector2f(10, 10));
		rect.setFillColor(sf::Color(0, 0, 0, 0));

		rect.setOutlineColor(sf::Color::Red);

		rect.setOutlineThickness(2);
		rect.setPosition(m_missile_target_position);

		if (m_missile_phase > Missile_SlowDown)
			screen.draw(rect);
	}
	
	//text.setString(ss.str());
	//screen.draw(text);

	GameObject::Draw(screen);
}

Ammo::Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int damage, FX* explosion, bool is_missile_model) : GameObject(position, speed, textureName, size)
{
	m_damage = damage;
	m_armor = 1;
	m_armor_max = 1;
	m_shield = 0;
	m_shield_max = 0;
	m_shield_regen = 0;
	m_ref_speed = sqrt((speed.x*speed.x) + (speed.y*speed.y));
	m_explosion = explosion;
	m_radius = 0;
	m_angspeed = 0;
	m_offset_x = 0;
	m_isBeam = false;
	m_range = 0;
	m_current_range = 0;
	m_shot_angle = 0;
	m_display_name = "Ammo";
	m_is_missile_model = is_missile_model;

	//missile motion model
	if (is_missile_model == true)
	{
		m_missile_acceleration = RandomizeFloatBetweenValues(900, 1000);
		m_missile_deceleration = RandomizeFloatBetweenValues(340, 360);
		m_missile_speed_max = 2000;
		m_missile_speed_min = RandomizeFloatBetweenValues(180, 220);
		m_missile_turn_speed = RandomizeFloatBetweenValues(200, 240);
		m_missile_turn_speed_track = 100;
		m_missile_speed_unlocking_target = 1000;
		m_missile_phase = Missile_SlowDown;
		m_DontGarbageMe = is_missile_model;
		m_missile_target_object = NULL;
	}
}

Ammo* Ammo::Clone()
{
	Ammo* ammo = new Ammo(this->getPosition(), this->m_speed, this->m_textureName, this->m_size, this->m_damage, this->m_explosion, this->m_is_missile_model);
	ammo->m_display_name = this->m_display_name;
	ammo->m_sound_name = this->m_sound_name;

	ammo->m_pattern = this->m_pattern;
	ammo->m_radius = this->m_radius;
	ammo->m_angspeed = this->m_angspeed;
	ammo->m_range = this->m_range;
	ammo->m_rotation_speed = this->m_rotation_speed;
	ammo->m_offset_x = this->m_offset_x;
	ammo->m_isBeam = this->m_isBeam;
	ammo->m_DontGarbageMe = this->m_isBeam || this->m_is_missile_model;
	ammo->m_is_missile_model = this->m_is_missile_model;

	return ammo;
}

void Ammo::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	const float l_hyperspeedMultiplier = hyperspeedMultiplier < 1 ? hyperspeedMultiplier : 1.0;

	//missile motion model
	if (m_is_missile_model == true)
	{
		float speed = GameObject::GetVectorLength(m_speed);
		float heading = getRotation();

		//disappearing after leaving screen in final stage
		if (m_missile_phase == Missile_FinalHeading)
		{
			sf::Vector2f size = sf::Vector2f(getGlobalBounds().width, getGlobalBounds().height);
			if (getPosition().x + (size.x / 2) < 0 || getPosition().x - (size.x / 2) > SCENE_SIZE_X || getPosition().y + (size.y / 2) < 0 || getPosition().y - (size.y / 2) > SCENE_SIZE_Y)
				m_garbageMe = true;
		}

		//speed change
		const float turn_speed = m_missile_phase == Missile_SlowDown ? m_missile_turn_speed : m_missile_turn_speed_track;
		if (m_missile_phase == Missile_SlowDown)
		{
			speed -= m_missile_deceleration * deltaTime.asSeconds() * l_hyperspeedMultiplier;
			if (speed <= m_missile_speed_min)
				m_missile_phase = Missile_TrackTarget;
		}
		else
		{
			speed += m_missile_acceleration * deltaTime.asSeconds() * l_hyperspeedMultiplier;
			if (speed > m_missile_speed_unlocking_target)
				m_missile_phase = Missile_FinalHeading;
		}
		
		//acquiring target
		if (m_missile_target_object == NULL)
		{
			GameObjectType target_type = m_collider_type == FriendlyFire ? EnemyObject : PlayerShip;
			m_missile_target_object = (*CurrentGame).GetNearestGameObject(target_type, getPosition(), m_range);
		}

		//turn to target position
		if (m_missile_phase != Missile_FinalHeading)
		{
			if (m_missile_target_object != NULL)
				m_missile_target_position = m_missile_target_object->getPosition();

			float delta_angle = GameObject::GetAngleDegToTargetPosition(getPosition(), heading + (m_collider_type == EnemyFire ? 180 : 0), m_missile_target_position);
			
			if (delta_angle != 0 && m_missile_phase != Missile_FinalHeading)
			{
				if (abs(delta_angle) <= turn_speed * deltaTime.asSeconds() * l_hyperspeedMultiplier)
					heading -= delta_angle;
				else if (delta_angle > 0)
					heading -= turn_speed * deltaTime.asSeconds() * l_hyperspeedMultiplier;
				else
					heading += turn_speed * deltaTime.asSeconds() * l_hyperspeedMultiplier;
			}
		}
		
		//set heading and speed
		setRotation(heading);
		BoundToValues(speed, m_missile_speed_min, m_missile_speed_max);
		m_speed = GameObject::GetVectorFromLengthAndAngle(speed, (heading + (m_collider_type == EnemyFire ? 180 : 0)) * M_PI / 180);
	}

	m_collision_timer -= m_collision_timer > 0 ? deltaTime.asSeconds() * l_hyperspeedMultiplier : 0;

	setGhost(hyperspeedMultiplier > 1.0f);

	//range before bullet extinction (optional. put "0" not to use)
	sf::Vector2f newspeed = m_speed;
	float new_ref_speed = m_ref_speed;
	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).x;
		newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).y;
		new_ref_speed *= hyperspeedMultiplier;
	}
	else if (hyperspeedMultiplier < 1)//slow motion
	{
		newspeed.x = m_speed.x * hyperspeedMultiplier;
		newspeed.y = m_speed.y * hyperspeedMultiplier;
		new_ref_speed *= hyperspeedMultiplier;
	}

	if (m_range > 0)
	{
		m_current_range += (new_ref_speed * deltaTime.asSeconds());
		if (m_current_range > m_range)
		{
			m_visible = false;
			m_garbageMe = true;
		}
	}

	//move
	static sf::Vector2f newposition, offset, pattern_offset;
		
	newposition.x = getPosition().x + newspeed.x * deltaTime.asSeconds();
	newposition.y = getPosition().y + newspeed.y * deltaTime.asSeconds();

	//call movement pattern
	pattern_offset = m_pattern.getOffset_v2(deltaTime * l_hyperspeedMultiplier);
	offset.x = pattern_offset.x * cos(m_shot_angle) + pattern_offset.y * sin(m_shot_angle);
	offset.y = pattern_offset.x * sin(m_shot_angle) + pattern_offset.y * cos(m_shot_angle);
	newposition.x += offset.x;
	newposition.y += offset.y;

	setPosition(newposition.x, newposition.y);

	//rotation
	rotate(m_rotation_speed * deltaTime.asSeconds() * l_hyperspeedMultiplier);

	AnimatedSprite::update(deltaTime);
}