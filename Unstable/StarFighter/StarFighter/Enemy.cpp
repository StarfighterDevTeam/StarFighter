#include "Enemy.h"

extern Game* CurrentGame;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* FX_death, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x/2, size.y/2), frameNumber, animationNumber)
{
	m_collider_type = EnemyObject;
	m_visible = true;
	m_angspeed = 0;
	m_radius = 0;
	m_FX_death = FX_death;
	m_rotation_speed = 0;
	m_face_target = false;
	m_reset_facing = false;
	m_bouncing = NoBouncing;
	m_enemyTimer = sf::seconds(0);
	m_level = 1;

	//life bars
	m_feedbackTimer = sf::seconds(0);

	m_armorBar_offsetY = - (m_size.y / 2) - (ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2) - ENEMY_HP_BAR_OFFSET_Y;
	m_shieldBar_offsetY = - (m_size.y / 2) - (1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y) - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y;
	//m_offsetBetweenHealthBars = armorBar_offsetY - shieldBar_offsetY;

	m_armorBar.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_armorBar.setFillColor(sf::Color(COLOR_GREEN_R_VALUE, COLOR_GREEN_G_VALUE, COLOR_GREEN_B_VALUE, COLOR_GREEN_A_VALUE));//green
	m_armorBar.setOutlineThickness(0);
	m_armorBar.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);

	m_armorBarContainer.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_armorBarContainer.setFillColor(sf::Color(0, 0, 0, 128));
	m_armorBarContainer.setOutlineThickness(0);
	//m_armorBarContainer->setOutlineColor(sf::Color(0, 255, 255, 128));
	m_armorBarContainer.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);

	m_shieldBar.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_shieldBar.setFillColor(sf::Color(COLOR_BLUE_R_VALUE, COLOR_BLUE_G_VALUE, COLOR_BLUE_B_VALUE, COLOR_BLUE_A_VALUE));//blue
	m_shieldBar.setOutlineThickness(0);
	m_shieldBar.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);
	m_shieldBar.setPosition(getPosition().x, getPosition().y - m_size.y / 2 - 1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y);

	m_shieldBarContainer.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_shieldBarContainer.setFillColor(sf::Color(0, 0, 0, 128));
	m_shieldBarContainer.setOutlineThickness(0);
	//m_shieldBarContainer->setOutlineColor(sf::Color(0, 255, 255, 128));
	m_shieldBarContainer.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);
	m_shieldBarContainer.setPosition(getPosition().x, getPosition().y - m_size.y / 2 - 1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y);

	try
	{
		m_font = new sf::Font();
		if (!m_font->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
		{
			// error
			//TODO: font loader
		}
		m_enemyLevel.setFont(*m_font);
		m_enemyLevel.setCharacterSize(12);
		m_enemyLevel.setColor(sf::Color::White);
		m_enemyLevel.setPosition(getPosition());
	}

	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::LERROR, ex.what());
	}
}

void Enemy::UpdateHealthBars()
{
	//health bars feedbacks
	float angle_rad = GameObject::getRotation_for_Direction((*CurrentGame).m_direction) / 180 * M_PI;
		
	m_armorBar.setPosition(getPosition().x - (m_armorBar_offsetY * sin(angle_rad)), getPosition().y + (m_armorBar_offsetY * cos(angle_rad)));
	m_armorBarContainer.setPosition(getPosition().x - (m_armorBar_offsetY * sin(angle_rad)), getPosition().y + (m_armorBar_offsetY * cos(angle_rad)));

	//TODO: screen borders constraints
	//armorBar->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBar->getPosition(), armorBar->getSize()));
	//armorBarContainer->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBarContainer->getPosition(), armorBarContainer->getSize()));

	//update size (damage)
	m_armorBar.setSize(sf::Vector2f(1.0f * m_armor / m_armor_max * ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	if (m_feedbackTimer <= sf::seconds(0))
	{
		(*CurrentGame).removeFromFeedbacks(&m_armorBar);
		(*CurrentGame).removeFromFeedbacks(&m_armorBarContainer);
	}
		
	m_shieldBar.setPosition(getPosition().x - (m_shieldBar_offsetY * sin(angle_rad)), getPosition().y + (m_shieldBar_offsetY * cos(angle_rad)));
	m_shieldBarContainer.setPosition(getPosition().x - (m_shieldBar_offsetY * sin(angle_rad)), getPosition().y + (m_shieldBar_offsetY * cos(angle_rad)));

	//TODO: screen borders constraints
	//shieldBar->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, shieldBar->getPosition(), shieldBar->getSize()));
	//shieldBarContainer->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, shieldBarContainer->getPosition(), shieldBar->getSize()));
	//if shield bar touches screen, we need to move both bars
	//armorBar->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBar->getPosition(), sf::Vector2f(armorBar->getSize().x, armorBar->getSize().y + (2 * offsetBetweenHealthBars) + (2 * shieldBarContainer->getSize().y))));
	//armorBarContainer->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBarContainer->getPosition(), sf::Vector2f(armorBar->getSize().x, armorBar->getSize().y + (2 * offsetBetweenHealthBars) + (2 * shieldBarContainer->getSize().y))));

	m_shieldBar.setSize(sf::Vector2f(1.0f * m_shield / m_shield_max * ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));

	if (m_feedbackTimer <= sf::seconds(0))
	{
		(*CurrentGame).removeFromFeedbacks(&m_shieldBar);
		(*CurrentGame).removeFromFeedbacks(&m_shieldBarContainer);
	}

	//update enemy level display
	if ((*CurrentGame).m_direction == DIRECTION_UP || (*CurrentGame).m_direction == DIRECTION_DOWN)
		m_enemyLevel.setPosition(sf::Vector2f(m_armorBarContainer.getPosition().x - m_armorBarContainer.getGlobalBounds().width / 2 - m_enemyLevel.getGlobalBounds().width / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_X, m_armorBarContainer.getPosition().y - m_enemyLevel.getGlobalBounds().height / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_Y));
	else if ((*CurrentGame).m_direction == DIRECTION_LEFT || (*CurrentGame).m_direction == DIRECTION_RIGHT)
		m_enemyLevel.setPosition(sf::Vector2f(m_armorBarContainer.getPosition().x - m_enemyLevel.getGlobalBounds().width / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_Y, m_armorBarContainer.getPosition().y - m_armorBarContainer.getGlobalBounds().height / 2 - m_enemyLevel.getGlobalBounds().height / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_X));

	if (m_feedbackTimer <= sf::seconds(0))
	{
		(*CurrentGame).removeFromFeedbacks(&m_enemyLevel);
	}
}

void Enemy::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (m_feedbackTimer > sf::seconds(0))
	{
		m_feedbackTimer -= deltaTime;
		UpdateHealthBars();
	}
	
	//dialog blocking the update?
	if ((*CurrentGame).m_waiting_for_dialog_validation || (*CurrentGame).m_end_dialog_clock.getElapsedTime().asSeconds() < END_OF_DIALOGS_DELAY)
	{
		AnimatedSprite::update(deltaTime);
		return;
	}

	//slow motion
	if (hyperspeedMultiplier < 1.0f)
	{
		m_phaseTimer += deltaTime * hyperspeedMultiplier;;
		m_enemyTimer += deltaTime * hyperspeedMultiplier;;
	}
	else
	{
		m_phaseTimer += deltaTime;
		m_enemyTimer += deltaTime;
	}

	//shield regen if not maximum
	static double shield_regen_buffer = 0;
	if (m_shield < m_shield_max && m_shield_max > 0)
	{
		if (hyperspeedMultiplier < 1.0f)
		{
			shield_regen_buffer += m_shield_regen*deltaTime.asSeconds() * hyperspeedMultiplier;
		}
		else
		{
			shield_regen_buffer += m_shield_regen*deltaTime.asSeconds();
		}
		
		if (shield_regen_buffer > 1)
		{
			double intpart;
			shield_regen_buffer = modf(shield_regen_buffer, &intpart);
			m_shield += intpart;
		}

		//canceling over-regen
		if (m_shield > m_shield_max)
		{
			m_shield = m_shield_max;
		}
	}

	//movement
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).x;
		newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).y;
	}

	float l_hyperspeedMultiplier = 1.0f;
	if (hyperspeedMultiplier < 1.0f)
	{
		l_hyperspeedMultiplier = hyperspeedMultiplier;
		newspeed.x *= l_hyperspeedMultiplier;
		newspeed.y *= l_hyperspeedMultiplier;
	}

	bool l_ghost = false;
	if (!m_phases.empty())
	{
		for (int i = 0; i < m_currentPhase->m_modifiers.size(); i++)
		{
			l_ghost = l_ghost || (m_currentPhase->m_modifiers[i] == GhostModifier);
		}
	}
	setGhost(l_ghost || hyperspeedMultiplier > 1.0f);
	m_disable_fire = hyperspeedMultiplier > 1.0f;

	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();
	
	//call bobbyPattern
	if (hyperspeedMultiplier < 1.0f)
	{
		offset = m_Pattern.GetOffset(deltaTime.asSeconds() * hyperspeedMultiplier);
	}
	else
	{
		offset = m_Pattern.GetOffset(deltaTime.asSeconds());
	}
	
	offset = GameObject::getSpeed_for_Direction((*CurrentGame).m_direction, offset);

	newposition.x += offset.x;
	newposition.y += offset.y;
	
	setPosition(newposition.x, newposition.y);

	if (m_bouncing != NoBouncing)
	{
		if (m_bouncing != BouncingVertical)
		{
			if (newposition.x < this->m_size.x / 2)
			{
				if ((*CurrentGame).m_direction == DIRECTION_UP || (*CurrentGame).m_direction == DIRECTION_DOWN)
				{
					m_Pattern.patternSpeed *= -1;
				}
				else
				{
					m_speed.x *= -1;
				}
				this->setPosition(this->m_size.x / 2, newposition.y);
			}

			else if (newposition.x > SCENE_SIZE_X - this->m_size.x / 2)
			{
				if ((*CurrentGame).m_direction == DIRECTION_UP || (*CurrentGame).m_direction == DIRECTION_DOWN)
				{
					m_Pattern.patternSpeed *= -1;
				}
				else
				{
					m_speed.x *= -1;
				}
				setPosition(SCENE_SIZE_X - this->m_size.x / 2, newposition.y);
			}
		}
		
		if (m_bouncing != BouncingHorizontal && hyperspeedMultiplier <= 1)
		{
			if (newposition.y < m_size.y / 2)
			{
				if ((*CurrentGame).m_direction == DIRECTION_UP || (*CurrentGame).m_direction == DIRECTION_DOWN)
				{
					m_speed.y *= -1;
				}
				else
				{
					m_currentPhase->m_Pattern->patternSpeed *= -1;
				}
				this->setPosition(newposition.x, this->m_size.y / 2);
			}

			else if (newposition.y > SCENE_SIZE_Y - this->m_size.y / 2)
			{
				if ((*CurrentGame).m_direction == DIRECTION_UP || (*CurrentGame).m_direction == DIRECTION_DOWN)
				{
					m_speed.y *= -1;
				}
				else
				{
					m_currentPhase->m_Pattern->patternSpeed *= -1;
				}
				this->setPosition(newposition.x, SCENE_SIZE_Y - this->m_size.y / 2);
			}
		}
	}

	//calculating the angle we want to face, if any
 	float target_angle = this->getRotation();

	//calculating the angle we want to face, if any
	bool isNearestTargetIsKnown = false;
	if (m_reset_facing)
	{
		target_angle = GameObject::getRotation_for_Direction((*CurrentGame).m_direction);	
	}
	else if (m_face_target)
	{
		target_angle = fmod(180 + GameObject::getRotation_for_Direction((*CurrentGame).m_direction) - (*CurrentGame).GetAngleToNearestGameObject(PlayerShip, this->getPosition()), 360);
		isNearestTargetIsKnown = true;
	}

	//calculating the weapons cooldown and targets
	bool isDoneFiringOnLockedTarget = true;
	for (std::vector<Weapon*>::iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++)
	{
		if ((*it)->isFiringReady(deltaTime, hyperspeedMultiplier))//update all weapons cooldown
		{
			(*it)->m_isReadyToFire = true;
			//now acquire target is the weapons needs to
			if (!isNearestTargetIsKnown)//maybe we know it already?
			{
				if ((*it)->m_target_seaking == SEAKING || ((*it)->m_target_seaking == SEMI_SEAKING && (*it)->m_rafale_index == 0))
				{
					target_angle = fmod(180 + GameObject::getRotation_for_Direction((*CurrentGame).m_direction) - (*CurrentGame).GetAngleToNearestGameObject(PlayerShip, this->getPosition()), 360);
				}
			}
		}
		else
		{
			(*it)->m_isReadyToFire = false;
		}

		//semi-seaking and rafale not ended or alternated multishot not ended need to keep the enemy oriented to the same target if it's "semi_seaking"
		if (m_face_target)
		{
			if ((*it)->m_target_seaking == SEMI_SEAKING && (*it)->m_rafale > 0 && (((*it)->m_rafale_index > 0 && (*it)->m_rafale_index < (*it)->m_rafale) || ((*it)->m_multishot > 1 && (*it)->m_shot_index > 0)))
			{
				isDoneFiringOnLockedTarget = false;
			}
		}
	}
	
	float current_angle = this->getRotation();
	float delta = current_angle - target_angle;
	if (delta > 180)
		delta -= 360;
	else if (delta < -180)
		delta += 360;

	if (!m_face_target && !m_reset_facing)
	{
		rotate(m_rotation_speed*deltaTime.asSeconds() * l_hyperspeedMultiplier);
	}
	else
	{
		if (isDoneFiringOnLockedTarget)
		{
			//now let's rotate toward the target (the player)
			if (delta >= 0)
			{
				if (abs(delta) > abs(m_rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier)
				{
					rotate(-abs(m_rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier);
				}
				else
				{
					setRotation(target_angle);
				}
			}
			else
			{
				if (abs(delta) > abs(m_rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier)
				{
					rotate(abs(m_rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier);
				}
				else
				{
					setRotation(target_angle);
				}
			}
		}
	}

	//automatic fire
	if (!m_weapons_list.empty())
	{
		if (m_isOnScene)
		{
			for (std::vector<Weapon*>::iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++)
			{
				if (!m_disable_fire)
				{
					if (m_phaseTimer.asSeconds() < (*it)->m_delay)
					{
						//do nothing: this weapon is settled to start firing later (delay)
					}
					else if ((*it)->m_isReadyToFire)
					{
						if (m_face_target && abs(delta) > 1.0f && isDoneFiringOnLockedTarget)//let's take delta>1 as an epsilon
						{
							//do nothing
						}
						else
						{
							if ((*it)->m_target_seaking == SEMI_SEAKING && (*it)->m_rafale > 0 && (((*it)->m_rafale_index > 0 && (*it)->m_rafale_index < (*it)->m_rafale) || ((*it)->m_multishot > 1 && (*it)->m_shot_index > 0)))
							{
								//semi-seaking and rafale not ended or alternated multishot not ended = no update of target or weapon position
							}
							else
							{
								//here we add delta so that we virtually move the weapon around the enemy, so that he can always shoot at 360 degrees with the same nice spread
								float theta = (getRotation() + (*it)->m_angle_offset) / 180 * M_PI;
								if ((*it)->m_target_seaking != NO_SEAKING)
								{
									theta -= delta / 180 * M_PI;
								}

								(*it)->m_weapon_current_offset.x = (*it)->m_weaponOffset.x - m_size.x / 2 * sin(theta);
								(*it)->m_weapon_current_offset.y = (*it)->m_weaponOffset.y + m_size.y / 2 * cos(theta);

								//transmitting the angle to the weapon, which will pass it to the bullets
								(*it)->m_shot_angle = theta;
							}

							(*it)->setPosition(getPosition().x + (*it)->m_weapon_current_offset.x, getPosition().y + (*it)->m_weapon_current_offset.y);
							(*it)->m_face_target = m_face_target;

							(*it)->Fire(EnemyFire, deltaTime);
							{
								m_shots_fired++;
							}
						}
					}
				}
			}
		}
	}

	AnimatedSprite::update(deltaTime);

	//damage feedback expires?
	if (m_color_timer > sf::seconds(0))
	{
		m_color_timer -= deltaTime;
		setColor(m_color);
		if (m_color_timer < sf::seconds(0))
		{
			setColor(Color(255, 255, 255, 255));
		}
	}

	//phases
	if (!m_phases.empty())
	{
		if (!m_currentPhase->m_transitions_list.empty())
		{
			this->CheckCondition();
		}
	}
}

void Enemy::RotateFeedbacks(float angle)
{
	m_armorBar.setRotation(angle);
	m_armorBarContainer.setRotation(angle);
	m_shieldBar.setRotation(angle);
	m_shieldBarContainer.setRotation(angle);
}

void Enemy::damage_from(GameObject& object)
{
	if (!m_immune)
	{
		if (m_feedbackTimer <= sf::seconds(0))
		{
			(*CurrentGame).addToFeedbacks(&m_armorBarContainer);
			(*CurrentGame).addToFeedbacks(&m_armorBar);
			if (m_shield_max > 0)
			{
				(*CurrentGame).addToFeedbacks(&m_shieldBarContainer);
				(*CurrentGame).addToFeedbacks(&m_shieldBar);
			}
			(*CurrentGame).addToFeedbacks(&m_enemyLevel);
		}

		m_feedbackTimer = sf::seconds(ENEMY_HEALTH_FEEDBACK_TIME);
		setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));

		if (object.m_damage > m_shield)
		{
			m_armor -= (object.m_damage - m_shield);
			m_shield = 0;
		}
		else
		{
			m_shield -= object.m_damage;
		}
	}
}

Enemy* Enemy::Clone()
{
	Enemy* enemy = new Enemy(getPosition(), m_speed, m_textureName, m_size, m_FX_death, m_frameNumber, m_animationNumber);

	((GameObject*)enemy)->m_armor = this->m_armor;
	((GameObject*)enemy)->m_armor_max = this->m_armor_max;
	((GameObject*)enemy)->m_shield = this->m_shield;
	((GameObject*)enemy)->m_shield_max = this->m_shield_max;
	((GameObject*)enemy)->m_shield_regen = this->m_shield_regen;
	((GameObject*)enemy)->m_damage = this->m_damage;

	for (std::vector<Weapon*>::iterator it = (m_weapons_list.begin()); it != (m_weapons_list.end()); it++)
	{
		enemy->m_weapons_list.push_back((*it)->Clone());
	}	
	
	((GameObject*)enemy)->addMoney(this->m_money);
	enemy->m_equipment_loot = this->getEquipmentLoot();
	enemy->m_weapon_loot = this->getWeaponLoot();
	enemy->m_display_name = this->m_display_name;
	enemy->m_enemy_class = this->m_enemy_class;

	enemy->m_Pattern = this->m_Pattern;
	enemy->m_angspeed = this->m_angspeed;
	enemy->m_radius = this->m_radius;

	enemy->m_rotation_speed = this->m_rotation_speed;

	if (!m_phases.empty())
	{
		enemy->m_currentPhase = this->m_currentPhase;
		enemy->setPhase(this->m_currentPhase);
		for (std::vector<Phase*>::iterator it = (this->m_phases.begin()); it != (this->m_phases.end()); it++)
		{
			enemy->m_phases.push_back((*it));
		}
	}

	enemy->m_level = this->m_level;
	enemy->m_enemyLevel.setString(to_string(this->m_level));

	return enemy;
}

Phase* Enemy::getPhase(string phaseName)
{
	for (vector<Phase*>::iterator it = this->m_phases.begin(); it != this->m_phases.end(); it++)
	{
		if ((*it)->m_name == phaseName)
		{
			return (*it);
		}
	}
	
	return NULL;
}

bool Enemy::CheckCondition()
{
	for (std::vector<ConditionTransition*>::iterator it = (this->m_currentPhase->m_transitions_list.begin()); it != (this->m_currentPhase->m_transitions_list.end()); it++)
	{
		switch ((*it)->m_condition)
		{
			case VerticalPosition:
			{
				FloatCompare result = this->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f((*it)->m_value / SCENE_SIZE_Y * SCENE_SIZE_X, (*it)->m_value));
				if (result == (*it)->m_op)
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					return true;
				}
													  
				break;
			}

			case HorizontalPosition:
			{
				FloatCompare result = this->compare_posX_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f((*it)->m_value, (*it)->m_value / SCENE_SIZE_X * SCENE_SIZE_Y));
				if (result == (*it)->m_op)
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					return true;
				}
			
				break;
			}
		
			case phaseClock:
			{
				if ((this->m_phaseTimer > sf::seconds((*it)->m_value)) && (*it)->m_op == GREATHER_THAN)
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					return true;
				}
				else if ((this->m_phaseTimer < sf::seconds((*it)->m_value)) && (*it)->m_op == LESSER_THAN)
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					return true;
				}

				break;
			}
		
			case enemyClock:
			{
				if ((this->m_enemyTimer > sf::seconds((*it)->m_value)) && (*it)->m_op == GREATHER_THAN)
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					this->m_enemyTimer = sf::seconds(0);
					return true;
				}
				else if ((this->m_enemyTimer < sf::seconds((*it)->m_value)) && (*it)->m_op == LESSER_THAN)
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					this->m_enemyTimer = sf::seconds(0);
					return true;
				}
										   
				break;
			}
		
			case LifePourcentage:
			{
				if ((100.0f * m_armor / m_armor_max >= (*it)->m_value) && (((*it)->m_op == GREATHER_THAN) || ((*it)->m_op == EQUAL_TO)))
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));

					return true;
				}
				else if ((100.0f * m_armor / m_armor_max <= (*it)->m_value) && (((*it)->m_op == LESSER_THAN) || ((*it)->m_op == EQUAL_TO)))
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					return true;
				}
				break;
			}
		
			case ShieldPourcentage:
			{
				//Caution, we don't want to be diving 0 / 0, so we need to handle separately the cases where ShieldMax worth 0 (enemy using no shield).
				if ((*it)->m_op == GREATHER_THAN)
				{
					if (m_shield_max == 0)
					{
						break;
					}
					else if (100.0f * m_shield / m_shield_max >= (*it)->m_value)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
					break;
				}
				else if ((*it)->m_op == LESSER_THAN)
				{
					if (this->m_shield_max == 0)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
					else if (100.0f * m_shield / m_shield_max <= (*it)->m_value)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
					break;
				}
				else if ((*it)->m_op == EQUAL_TO)
				{
					if (m_shield_max == 0)
					{
						if ((*it)->m_value == 0)
						{
							this->setPhase(this->getPhase((*it)->m_nextPhase_name));
							return true;
						}
						break;
					}
					else if (100.0f * m_shield / m_shield_max == (*it)->m_value)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
					break;
				}
			}

			case wakeUp:
			{
				if (this->m_wake_up)
				{
					this->setPhase(this->getPhase((*it)->m_nextPhase_name));
					return true;
				}
				break;
			}

			case EnemyProximity:
			{
				if ((*it)->m_op == GREATHER_THAN)
				{
					if ((*CurrentGame).FoundNearestGameObject(PlayerShip, this->getPosition(), (*it)->m_value) == TargetScan::TARGET_OUT_OF_RANGE)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
				}
				else if ((*it)->m_op == LESSER_THAN || (*it)->m_op == EQUAL_TO)
				{
					if ((*CurrentGame).FoundNearestGameObject(PlayerShip, this->getPosition(), (*it)->m_value) == TargetScan::TARGET_IN_RANGE)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
				}							  
				break;
			}

			case ShotsFired:
			{
				if ((*it)->m_op == GREATHER_THAN)
				{
					if (this->m_shots_fired >= (*it)->m_value)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
				}
				else if ((*it)->m_op == EQUAL_TO)
				{
					if (this->m_shots_fired == (*it)->m_value)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
				}
				else if ((*it)->m_op == LESSER_THAN)
				{
					if (this->m_shots_fired < (*it)->m_value)
					{
						this->setPhase(this->getPhase((*it)->m_nextPhase_name));
						return true;
					}
				}
				break;
			}
		}
	}

	return false;
}

void Enemy::setPhase(Phase* phase)
{
	assert(phase != NULL);
	m_currentPhase = phase;
	m_shots_fired = 0;

	m_speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, phase->m_vspeed);

	//reset old stats
	m_immune = false;
	m_face_target = false;
	m_reset_facing = false;
	m_bouncing = NoBouncing;

	setGhost(false);

	//load new stats
	for (int i = 0; i < m_currentPhase->m_modifiers.size(); i++)
	{
		switch (phase->m_modifiers[i])
		{
			case NoModifier:
			{
				//do nothing
				break;
			}
			case Immune:
			{
				m_immune = true;
				break;
			}
			case GhostModifier:
			{
				setGhost(true);
				break;
			}
			case DeathModifier:
			{
				Death();
				break;
			}
			case FaceTarget:
			{
				m_face_target = true;
				break;
			}
			case ResetFacing:
			{
				m_reset_facing = true;
				break;
			}
			case Bouncing:
			{
				m_bouncing = BouncingEverywhere;
				break;
			}
			case BouncingH:
			{
				m_bouncing = BouncingHorizontal;
				break;
			}
			case BouncingV:
			{
				m_bouncing = BouncingVertical;
				break;
			}
		}
	}

	//clearing old weapons and setting new ones
	m_weapons_list.clear();
	for (std::vector<Weapon*>::iterator it = phase->m_weapons_list.begin(); it != phase->m_weapons_list.end(); it++)
	{
		m_weapons_list.push_back((*it)->Clone());
	}

	//movement
	m_Pattern.SetPattern(phase->m_Pattern->currentPattern, phase->m_Pattern->patternSpeed, phase->m_Pattern->patternParams); //vitesse angulaire (degres/s)
	m_rotation_speed = phase->m_rotation_speed;

	//welcome shot: shot once at the beginning of the phase (actually used as a post-mortem "good-bye"shoot)
	if (phase->m_hasWelcomeShot)
	{
		float theta = this->getRotation() / 180 * M_PI;
		float weapon_offset_x = phase->m_welcomeWeapon->m_weaponOffset.x - m_size.y / 2 * sin(theta);
		float weapon_offset_y = phase->m_welcomeWeapon->m_weaponOffset.y + m_size.y / 2 * cos(theta);

		phase->m_welcomeWeapon->setPosition(this->getPosition().x + weapon_offset_x, this->getPosition().y + weapon_offset_y);
		phase->m_welcomeWeapon->m_shot_angle = theta;

		phase->m_welcomeWeapon->Fire(EnemyFire);
	}

	//setting up wake_up condition
	bool wake_up_condition_exists = false;
	for (std::vector<ConditionTransition*>::iterator it = (phase->m_transitions_list.begin()); it != (phase->m_transitions_list.end()); it++)
	{
		if ((*it)->m_condition == wakeUp)
		{
			m_wake_up = false;
			wake_up_condition_exists = true;
		}
	}
	//reset the flag "wake_up" if this phase doesn't use the condition (it means the awakening has been skipped by another phase transition
	if (!wake_up_condition_exists)
	{
		m_wake_up = true;
	}

	//waking up enemies
	if (phase->m_hasWakeUp)
	{
		(*CurrentGame).WakeUpEnemiesWithName(phase->m_wake_up_name);
	}

	//dialogs
	if (!phase->m_dialogs.empty())
	{
		if (!(*CurrentGame).m_waiting_for_dialog_validation)
		{
			(*CurrentGame).playerShip->m_is_asking_SFPanel = SFPanel_Dialog;
			size_t dialogsVectorSize = phase->m_dialogs.size();
			for (size_t i = 0; i < dialogsVectorSize; i++)
			{
				(*CurrentGame).playerShip->m_targetDialogs.push_back(phase->m_dialogs[i]->Clone());
			}
			(*CurrentGame).m_waiting_for_dialog_validation = true;
		}
	}

	m_phaseTimer = sf::seconds(0);
}

Dialog* Enemy::LoadDialog(string name)
{
	vector<vector<string> > dialogConfig = *(FileLoaderUtils::FileLoader(DIALOGS_FILE));

	for (std::vector<vector<string> >::iterator it = (dialogConfig).begin(); it != (dialogConfig).end(); it++)
	{
		if ((*it)[DIALOG_NAME].compare(name) == 0)
		{
			Dialog* dialog = new Dialog();

			dialog->m_name = name;
			dialog->m_fade_in = (bool)(stoi((*it)[DIALOG_FADE_IN]));
			dialog->m_fade_out = (bool)(stoi((*it)[DIALOG_FADE_OUT]));
			dialog->m_enemy_speaking = (*it)[DIALOG_ENEMY_SPEAKING].compare("0") != 0;
			dialog->m_duration = atof((*it)[DIALOG_DURATION].c_str());
			dialog->m_title = (*it)[DIALOG_TITLE];
			dialog->m_title = ReplaceAll(dialog->m_title, "_", " ");
			dialog->m_title = ReplaceAll(dialog->m_title, "[COMA]", ",");
			dialog->m_body = (*it)[DIALOG_BODY];
			dialog->m_body = ReplaceAll(dialog->m_body, "_", " ");
			dialog->m_body = ReplaceAll(dialog->m_body, "[COMA]", ",");
			dialog->m_picture_name = (*it)[DIALOG_PICTURE];
			dialog->m_next_dialog_name = (*it)[DIALOG_NEXT];

			TextureLoader *loader;
			loader = TextureLoader::getInstance();
			loader->loadTexture(dialog->m_picture_name, DIALOG_PANEL_PORTRAIT_SIZE_X, DIALOG_PANEL_PORTRAIT_SIZE_X);

			return dialog;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Dialog '%s'. Please check the config file", (char*)name.c_str()));
}

Phase* Enemy::LoadPhase(string name)
{
	vector<vector<string> > phaseConfig = *(FileLoaderUtils::FileLoader(PHASES_FILE));

	for (std::vector<vector<string> >::iterator it = (phaseConfig).begin(); it != (phaseConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Phase* phase = new Phase();

			phase->m_name = name;
			phase->m_display_name = (*it)[PHASE_NAME];
			phase->m_vspeed = stoi((*it)[PHASE_VSPEED]);

			//loading weapons and ammos
			for (int i = 0; i < 6; i++)
			{
				if ((*it)[PHASE_WEAPON + (i * 4)].compare("0") != 0)
				{
					Weapon* m_weapon = Enemy::LoadWeapon((*it)[PHASE_WEAPON + (i * 4)], 1, Enemy::LoadAmmo((*it)[PHASE_AMMO + (i * 4)]));
					m_weapon->m_weaponOffset.x = atof((*it)[PHASE_WEAPON_OFFSET + (i * 4)].c_str());
					m_weapon->m_delay = atof((*it)[PHASE_WEAPON_DELAY + (i * 4)].c_str());
					phase->m_weapons_list.push_back(m_weapon);
					
				}
			}

			//loading phases
			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), PHASE_PATTERN);
			phase->m_Pattern = m_bobby;

			//loading rotation speed
			phase->m_rotation_speed = stoi((*it)[PHASE_ROTATION_SPEED]);

			//loading modifier (immune to damage, etc.)
			for (int i = 0; i < 2; i++)
			{
				Modifier l_new_modifier = NoModifier;
				if ((*it)[PHASE_MODIFIER + i].compare("0") != 0)
				{
					if ((*it)[PHASE_MODIFIER + i].compare("immune") == 0)
					{
						l_new_modifier = Immune;
					}
					else if ((*it)[PHASE_MODIFIER + i].compare("ghost") == 0)
					{
						l_new_modifier = GhostModifier;
					}
					else if ((*it)[PHASE_MODIFIER + i].compare("death") == 0)
					{
						l_new_modifier = DeathModifier;
					}
					else if ((*it)[PHASE_MODIFIER + i].compare("face_target") == 0)
					{
						l_new_modifier = FaceTarget;
					}
					else if ((*it)[PHASE_MODIFIER + i].compare("reset_facing") == 0)
					{
						l_new_modifier = ResetFacing;
					}
					else if ((*it)[PHASE_MODIFIER + i].compare("bouncing") == 0)
					{
						l_new_modifier = Bouncing;
					}
					else if ((*it)[PHASE_MODIFIER + i].compare("bouncingH") == 0)
					{
						l_new_modifier = BouncingH;
					}
					else if ((*it)[PHASE_MODIFIER + i].compare("bouncingV") == 0)
					{
						l_new_modifier = BouncingV;
					}
				}

				phase->m_modifiers.push_back(l_new_modifier);
			}

			//loading welcome shot
			if ((*it)[PHASE_WELCOME_WEAPON].compare("0") != 0)
			{
				phase->m_welcomeWeapon = Enemy::LoadWeapon((*it)[PHASE_WELCOME_WEAPON], 1, Enemy::LoadAmmo((*it)[PHASE_WELCOME_AMMO]));
				phase->m_hasWelcomeShot = true;
			}
			
			//load enemies (by name) to wake up
			if ((*it)[PHASE_WAKEUP].compare("0") != 0)
			{
				phase->m_wake_up_name = (*it)[PHASE_WAKEUP];
				phase->m_hasWakeUp = true;
			}

			//loading transition to next phase
			if ((*it)[PHASE_CONDITION].compare("0") != 0)
			{
				phase->m_transitions_list.push_back(Phase::ConditionLoader((*it), PHASE_CONDITION));
			}
			if ((*it)[PHASE_CONDITION_2].compare("0") != 0)
			{
				phase->m_transitions_list.push_back(Phase::ConditionLoader((*it), PHASE_CONDITION_2));
			}

			//loading dialogs
			if ((*it)[PHASE_DIALOG_NAME].compare("0") != 0)
			{
				Dialog* dialog = Enemy::LoadDialog((*it)[PHASE_DIALOG_NAME]);
				phase->m_dialogs.push_back(dialog);
				while (!dialog->m_next_dialog_name.empty() && dialog->m_next_dialog_name.compare("0") != 0)
				{
					dialog = Enemy::LoadDialog(dialog->m_next_dialog_name);
					phase->m_dialogs.push_back(dialog);
				}
			}

			return phase;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find EnemyPhase '%s'. Please check the config file", (char*)name.c_str()));
}

void Enemy::Death()
{
	FX* myFX = m_FX_death->Clone();
	myFX->setPosition(getPosition().x, getPosition().y);
	(*CurrentGame).addToScene(myFX,ExplosionLayer, Neutral);

	//Score
	(*CurrentGame).m_hazard += m_money;
	if (!m_isOnScene)
	{
		(*CurrentGame).m_hazardSpawned -= m_money;//can only happen when using debugs or scripts to wipe all enemies even offscreen
	}

	//Player XP
	//(*CurrentGame).playerShip->gain_xp(XPTable_PerEnemyClass[this->enemy_class]);

	//Loot
	if (CreateRandomLootv2(m_enemy_class, (*CurrentGame).m_BeastScoreBonus))
	{
		GenerateLoot();
	}

	m_visible = false;
	m_isOnScene = false;
	m_GarbageMe = true;

	if (m_enemy_class < ENEMYPOOL_DELTA)
	{
		(*CurrentGame).PlaySFX(SFX_Kill);
	}
	else
	{
		(*CurrentGame).PlaySFX(SFX_BigKill);
	}
	
}

void Enemy::Destroy()
{
	(*CurrentGame).removeFromFeedbacks(&m_armorBar);
	(*CurrentGame).removeFromFeedbacks(&m_armorBarContainer);
	
	(*CurrentGame).removeFromFeedbacks(&m_shieldBar);
	(*CurrentGame).removeFromFeedbacks(&m_shieldBarContainer);
	
	(*CurrentGame).removeFromFeedbacks(&m_enemyLevel);
}

void Enemy::GenerateLoot()
{
	sf::Vector2f speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, LOOT_SPEED_Y);

	if (m_weapon_loot != NULL)
	{
		Loot* new_loot = new Loot(getPosition(), speed, m_weapon_loot->m_textureName, m_weapon_loot->m_size, m_weapon_loot->m_display_name);
		new_loot->get_weapon_from(*this);
		(*CurrentGame).addToScene((GameObject*)new_loot, LootLayer, LootObject);
	}

	else if (m_equipment_loot != NULL)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, m_equipment_loot->m_textureName, m_equipment_loot->m_size, m_equipment_loot->m_display_name);
		new_loot->get_equipment_from(*this);
		(*CurrentGame).addToScene((GameObject*)new_loot, LootLayer, LootObject);
	}

	else if (m_money > 0)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Money");
		new_loot->setMoney(m_money);
		(*CurrentGame).addToScene((GameObject*)new_loot, LootLayer, LootObject);
	}
	else
	{
		//que dalle
	}
}

bool Enemy::CreateRandomLootv2(EnemyClass loot_class, float BeastScaleBonus, bool force_BeastScale, float BeastScale_min, float BeastScale_max)
{
	setMoney(0);
	double random_number = (double)rand() / (RAND_MAX);

	if (random_number > LootTable_DroppingSomething[loot_class])
	{
		//EMPTY DROP
		return false;
	}
	else
	{
		//setting min and max beast score values
		sf::Vector2f LootTable_BeastScale[NBVAL_EnemyClass];
		for (int b = 0; b < NBVAL_EnemyClass; b++)
		{
			if (b != ENEMYPOOL_VOID && b != ENEMYPOOL_ZETA)
			{
				LootTable_BeastScale[b].x = LootTable_BeastScale_Base[b].x + BeastScaleBonus;
				LootTable_BeastScale[b].y = LootTable_BeastScale_Base[b].y + BeastScaleBonus;
			}
			else
			{
				LootTable_BeastScale[b].x = 0;
				LootTable_BeastScale[b].y = 0;
			}
		}

		//ITEM DROP
		if (random_number < LootTable_DropIsEquipment[loot_class] * LootTable_DroppingSomething[loot_class])
		{
			//Beast Scale Score randomized here within the min and max
			float BeastScaleScore = RandomizeFloatBetweenValues(LootTable_BeastScale[loot_class]);

			//Calculation of the bonus "credits" for the loot
			int loot_credits_ = ceil(BeastScaleScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(m_level + 1));

			//Equipment type
			int equipment_type_roll = rand() % ((int)NBVAL_Equipment + 1);//+1 is for the weapon type

			//"Spending credits" on item stats and assigning the equipment/weapon as a loot
			AssignRandomEquipment((EquipmentType)equipment_type_roll, loot_credits_, m_level + 1, this, BeastScaleScore);
		}

		else
		{
			//MONEY DROP
			int money = RandomizeIntBetweenRatios(1.0f * XPTable_PerEnemyClass[loot_class] * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(m_level) / 100, LootTable_BeastScale[loot_class]);
			this->m_money = money;//looting money
		}
	}

	return true;
}

bool Enemy::AssignRandomEquipment(EquipmentType equipment_type, int credits, int level, GameObject* object, float quality)
{
	switch (equipment_type)
	{
		case (int)Engine:
		{
			return object->setEquipmentLoot(Equipment::CreateRandomEngine(credits, level, quality));
		}

		case (int)Armor:
		{
			return object->setEquipmentLoot(Equipment::CreateRandomArmor(credits, level, quality));
		}

		case (int)Shield:
		{
			return object->setEquipmentLoot(Equipment::CreateRandomShield(credits, level, quality));
		}

		case (int)Module:
		{
			return object->setEquipmentLoot(Equipment::CreateRandomModule(credits, level, quality));
		}

		case (int)NBVAL_Equipment://WEAPON DROP
		{
			return object->setWeaponLoot(Weapon::CreateRandomWeapon(credits, level, false, quality));
		}
	}

	return false;
}

int Enemy::GetChosenProperty(vector<int> *properties_roll_table, int properties_to_choose_from, int p)
{
	int index = rand() % (properties_to_choose_from - p);
	int chosen_property = (*properties_roll_table)[index];

	int a = properties_roll_table->back();
	(*properties_roll_table)[properties_roll_table->size() - 1] = chosen_property;
	(*properties_roll_table)[index] = a;
	return chosen_property;
}

Weapon* Enemy::LoadWeapon(string name, int fire_direction, Ammo* ammo)
{
	vector<vector<string> > weaponConfig = *(FileLoaderUtils::FileLoader(WEAPON_FILE));

	for (std::vector<vector<string> >::iterator it = (weaponConfig).begin(); it != (weaponConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Weapon* weapon = new Weapon(ammo);
			weapon->m_display_name = (*it)[WEAPON_DISPLAY_NAME];
			weapon->m_fire_direction = Vector2i(0, fire_direction);
			weapon->m_rate_of_fire = atof((*it)[WEAPON_RATE_OF_FIRE].c_str());
			weapon->m_shot_mode = NoShotMode;

			weapon->m_multishot = stoi((*it)[WEAPON_MULTISHOT]);
			if (weapon->m_multishot > 1)
			{
				weapon->m_xspread = stoi((*it)[WEAPON_XSPREAD]);
				weapon->m_dispersion = stoi((*it)[WEAPON_DISPERSION]);
				if ((*it)[WEAPON_ALTERNATE].compare("0") != 0)
				{
					if ((*it)[WEAPON_ALTERNATE].compare("alternate") == 0)
						weapon->m_shot_mode = AlternateShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("ascending") == 0)
						weapon->m_shot_mode = AscendingShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("descending") == 0)
						weapon->m_shot_mode = DescendingShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("ascending2") == 0)
						weapon->m_shot_mode = Ascending2ShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("descending2") == 0)
						weapon->m_shot_mode = Descending2ShotMode;
				}
			}
			
			weapon->m_rafale = stoi((*it)[WEAPON_RAFALE]);
			if (weapon->m_rafale != 0)
			{
				weapon->m_rafale_cooldown = atof((*it)[WEAPON_RAFALE_COOLDOWN].c_str());
			}
				
			weapon->m_textureName = (*it)[WEAPON_IMAGE_NAME];
			weapon->m_size = sf::Vector2f(stoi((*it)[WEAPON_WIDTH]), stoi((*it)[WEAPON_HEIGHT]));
			weapon->m_frameNumber = stoi((*it)[WEAPON_FRAMES]);
			weapon->m_angle_offset = stoi((*it)[WEAPON_ANGLE_OFFSET]);

			if ((*it)[WEAPON_TARGET_SEAKING].compare("0") != 0)
			{
				if ((*it)[WEAPON_TARGET_SEAKING].compare("semi_seaking") == 0)
					weapon->m_target_seaking = SEMI_SEAKING;
				else if ((*it)[WEAPON_TARGET_SEAKING].compare("seaking") == 0)
					weapon->m_target_seaking = SEAKING;
				else if ((*it)[WEAPON_TARGET_SEAKING].compare("super_seaking") == 0)
					weapon->m_target_seaking = SUPER_SEAKING;
			}

			return weapon;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Weapon '%s'. Please check the config file", (char*)name.c_str()));
}

Ammo* Enemy::LoadAmmo(string name)
{
	vector<vector<string> > ammoConfig = *(FileLoaderUtils::FileLoader(AMMO_FILE));

	for (std::vector<vector<string> >::iterator it = (ammoConfig).begin(); it != (ammoConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Ammo* new_ammo = new Ammo(Vector2f(0, 0), Vector2f(0, stoi((*it)[AMMO_SPEED])), (*it)[AMMO_IMAGE_NAME],
				Vector2f(stoi((*it)[AMMO_WIDTH]), stoi((*it)[AMMO_HEIGHT])), stoi((*it)[AMMO_DAMAGE]), LoadFX((*it)[AMMO_FX]));
			new_ammo->m_display_name = (*it)[AMMO_NAME];
			new_ammo->m_range = stoi((*it)[AMMO_RANGE]);
			
			PatternBobby* bobby = PatternBobby::PatternLoader((*it), AMMO_PATTERN);
			new_ammo->m_Pattern.SetPattern(bobby->currentPattern, bobby->patternSpeed, bobby->patternParams);

			new_ammo->m_rotation_speed = stoi((*it)[AMMO_ROTATION_SPEED]);
			return new_ammo;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file", (char*)name.c_str()));
}

FX* Enemy::LoadFX(string name)
{
	vector<vector<string> >FXConfig = *(FileLoaderUtils::FileLoader(FX_FILE));

	for (std::vector<vector<string> >::iterator it = (FXConfig).begin(); it != (FXConfig).end(); it++)
	{
		if ((*it)[FX_TYPE].compare("explosion") == 0)
		{
			if ((*it)[FX_NAME].compare(name) == 0)
			{
				float duration = atof(((*it)[FX_DURATION]).c_str());
				FX* myFX = new FX(Vector2f(0, 0), Vector2f(0, 0), (*it)[FX_FILENAME], Vector2f(stoi((*it)[FX_WIDTH]), stoi((*it)[FX_HEIGHT])), stoi((*it)[FX_FRAMES]), sf::seconds(duration));
				myFX->m_display_name = (*it)[FX_NAME];

				return myFX;
			}
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find FX '%s'. Please check the config file", (char*)name.c_str()));

}

void Enemy::ApplyLevelModifiers()
{
	float multiplier_ = ceil(1.0f * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(m_level) / 100);

	m_armor_max = floor(m_armor_max * multiplier_);
	m_armor = m_armor_max;
	m_shield_max = floor(m_shield_max * multiplier_);
	m_shield = m_shield_max;
	m_shield_regen = floor(m_shield_regen * multiplier_);
	m_damage = floor(m_damage * multiplier_);
	for (std::vector<Weapon*>::iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++)
	{
		(*it)->m_ammunition->m_damage = ceil((*it)->m_ammunition->m_damage * multiplier_);
	}
	this->setMoney(m_money *= 1.0f * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(m_level) / 100);

	m_enemyLevel.setString(to_string(m_level));
}