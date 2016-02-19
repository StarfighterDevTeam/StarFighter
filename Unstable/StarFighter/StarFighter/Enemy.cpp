#include "Enemy.h"

extern Game* CurrentGame;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death, int m_frameNumber, int m_animationNumber) : Independant(position, speed, textureName, size, sf::Vector2f(size.x/2, size.y/2), m_frameNumber, m_animationNumber)
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	angspeed = 0;
	radius = 0;
	FX_death = m_FX_death;
	hasPhases = false;
	rotation_speed = 0;
	face_target = false;
	reset_facing = false;
	bouncing = NoBouncing;
	enemyTimer = sf::seconds(0);
	level = 1;

	//life bars
	feedbackTimer = sf::seconds(0);

	armorBar_offsetY = - (m_size.y / 2) - (ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2) - ENEMY_HP_BAR_OFFSET_Y;
	shieldBar_offsetY = - (m_size.y / 2) - (1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y) - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y;
	//offsetBetweenHealthBars = armorBar_offsetY - shieldBar_offsetY;

	armorBar = new RectangleShape();
	armorBar->setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	armorBar->setFillColor(sf::Color(COLOR_GREEN_R_VALUE, COLOR_GREEN_G_VALUE, COLOR_GREEN_B_VALUE, COLOR_GREEN_A_VALUE));//green
	armorBar->setOutlineThickness(0);
	armorBar->setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);

	armorBarContainer = new RectangleShape();
	armorBarContainer->setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	armorBarContainer->setFillColor(sf::Color(0, 0, 0, 128));
	armorBarContainer->setOutlineThickness(0);
	//armorBarContainer->setOutlineColor(sf::Color(0, 255, 255, 128));
	armorBarContainer->setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);

	shieldBar = new RectangleShape();
	shieldBar->setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	shieldBar->setFillColor(sf::Color(COLOR_BLUE_R_VALUE, COLOR_BLUE_G_VALUE, COLOR_BLUE_B_VALUE, COLOR_BLUE_A_VALUE));//blue
	shieldBar->setOutlineThickness(0);
	shieldBar->setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);
	shieldBar->setPosition(getPosition().x, getPosition().y - m_size.y / 2 - 1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y);

	shieldBarContainer = new RectangleShape();
	shieldBarContainer->setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	shieldBarContainer->setFillColor(sf::Color(0, 0, 0, 128));
	shieldBarContainer->setOutlineThickness(0);
	//shieldBarContainer->setOutlineColor(sf::Color(0, 255, 255, 128));
	shieldBarContainer->setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);
	shieldBarContainer->setPosition(getPosition().x, getPosition().y - m_size.y / 2 - 1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y);

	try
	{
		font = new sf::Font();
		if (!font->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
		{
			// error
			//TODO: font loader
		}
		enemyLevel.setFont(*font);
		enemyLevel.setCharacterSize(12);
		enemyLevel.setColor(sf::Color::White);
		enemyLevel.setPosition(getPosition().x, getPosition().y);
	}

	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR, ex.what());
	}
}

void Enemy::UpdateHealthBars(sf::Time deltaTime)
{
	//health bars feedbacks
	if (feedbackTimer > sf::seconds(0))
	{
		feedbackTimer -= deltaTime;

		float angle_rad = Independant::getRotation_for_Direction((*CurrentGame).direction) / 180 * M_PI;
		if (armorBar)
		{
			armorBar->setPosition(this->getPosition().x - (this->armorBar_offsetY * sin(angle_rad)), this->getPosition().y + (this->armorBar_offsetY * cos(angle_rad)));
			armorBarContainer->setPosition(this->getPosition().x - (this->armorBar_offsetY * sin(angle_rad)), this->getPosition().y + (this->armorBar_offsetY * cos(angle_rad)));

			//TODO: screen borders constraints
			//armorBar->setPosition(Independant::ApplyScreenBordersConstraints((*CurrentGame).direction, armorBar->getPosition(), armorBar->getSize()));
			//armorBarContainer->setPosition(Independant::ApplyScreenBordersConstraints((*CurrentGame).direction, armorBarContainer->getPosition(), armorBarContainer->getSize()));

			//update size (damage)
			armorBar->setSize(sf::Vector2f(1.0f * armor / armor_max * ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
			if (feedbackTimer <= sf::seconds(0))
			{
				(*CurrentGame).removeFromFeedbacks(armorBar);
				(*CurrentGame).removeFromFeedbacks(armorBarContainer);
			}
		}
		if (shieldBar)
		{
			shieldBar->setPosition(this->getPosition().x - (this->shieldBar_offsetY * sin(angle_rad)), this->getPosition().y + (this->shieldBar_offsetY * cos(angle_rad)));
			shieldBarContainer->setPosition(this->getPosition().x - (this->shieldBar_offsetY * sin(angle_rad)), this->getPosition().y + (this->shieldBar_offsetY * cos(angle_rad)));

			//TODO: screen borders constraints
			//shieldBar->setPosition(Independant::ApplyScreenBordersConstraints((*CurrentGame).direction, shieldBar->getPosition(), shieldBar->getSize()));
			//shieldBarContainer->setPosition(Independant::ApplyScreenBordersConstraints((*CurrentGame).direction, shieldBarContainer->getPosition(), shieldBar->getSize()));
			//if shield bar touches screen, we need to move both bars
			//armorBar->setPosition(Independant::ApplyScreenBordersConstraints((*CurrentGame).direction, armorBar->getPosition(), sf::Vector2f(armorBar->getSize().x, armorBar->getSize().y + (2 * offsetBetweenHealthBars) + (2 * shieldBarContainer->getSize().y))));
			//armorBarContainer->setPosition(Independant::ApplyScreenBordersConstraints((*CurrentGame).direction, armorBarContainer->getPosition(), sf::Vector2f(armorBar->getSize().x, armorBar->getSize().y + (2 * offsetBetweenHealthBars) + (2 * shieldBarContainer->getSize().y))));

			shieldBar->setSize(sf::Vector2f(1.0f * shield / shield_max * ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));

			if (feedbackTimer <= sf::seconds(0))
			{
				(*CurrentGame).removeFromFeedbacks(shieldBar);
				(*CurrentGame).removeFromFeedbacks(shieldBarContainer);
			}
		}

		//update enemy level display
		enemyLevel.setPosition(sf::Vector2f(armorBarContainer->getPosition().x - armorBarContainer->getGlobalBounds().width / 2 - enemyLevel.getGlobalBounds().width / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_X, armorBarContainer->getPosition().y - enemyLevel.getGlobalBounds().height / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_Y));
		if (feedbackTimer <= sf::seconds(0))
		{
			(*CurrentGame).removeFromFeedbacks(&enemyLevel);
		}
	}
}

void Enemy::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	UpdateHealthBars(deltaTime);
	
	//slow motion
	if (hyperspeedMultiplier < 1.0f)
	{
		phaseTimer += deltaTime * hyperspeedMultiplier;;
		enemyTimer += deltaTime * hyperspeedMultiplier;;
	}
	else
	{
		phaseTimer += deltaTime;
		enemyTimer += deltaTime;
	}

	//shield regen if not maximum
	static double shield_regen_buffer = 0;
	if (shield < shield_max && shield > 0)
	{
		if (hyperspeedMultiplier < 1.0f)
		{
			shield_regen_buffer += shield_regen*deltaTime.asSeconds() * hyperspeedMultiplier;
		}
		else
		{
			shield_regen_buffer += shield_regen*deltaTime.asSeconds();
		}
		
		if (shield_regen_buffer > 1)
		{
			double intpart;
			shield_regen_buffer = modf(shield_regen_buffer, &intpart);
			shield += intpart;
		}

		//canceling over-regen
		if (shield > shield_max)
		{
			shield = shield_max;
		}
	}

	//movement
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = this->speed;

	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += Independant::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).x;
		newspeed.y += Independant::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).y;
	}

	float l_hyperspeedMultiplier = 1.0f;
	if (hyperspeedMultiplier < 1.0f)
	{
		l_hyperspeedMultiplier = hyperspeedMultiplier;
		newspeed.x *= l_hyperspeedMultiplier;
		newspeed.y *= l_hyperspeedMultiplier;
	}

	bool l_ghost = false;
	if (this->hasPhases)
	{
		for (int i = 0; i < this->currentPhase->modifiers.size(); i++)
		{
			l_ghost = l_ghost || (this->currentPhase->modifiers[i] == Ghost);
		}
	}
	this->setGhost(l_ghost || hyperspeedMultiplier > 1.0f);
	this->disable_fire = hyperspeedMultiplier > 1.0f;

	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();
	
	//call bobbyPattern
	if (hyperspeedMultiplier < 1.0f)
	{
		offset = Pattern.GetOffset(deltaTime.asSeconds() * hyperspeedMultiplier);
	}
	else
	{
		offset = Pattern.GetOffset(deltaTime.asSeconds());
	}
	
	offset = Independant::getSpeed_for_Direction((*CurrentGame).direction, offset);

	newposition.x += offset.x;
	newposition.y += offset.y;
	
	this->setPosition(newposition.x, newposition.y);

	if (this->bouncing != NoBouncing)
	{
		if (this->bouncing != BouncingVertical)
		{
			if (newposition.x < this->m_size.x / 2)
			{
				if ((*CurrentGame).direction == DIRECTION_UP || (*CurrentGame).direction == DIRECTION_DOWN)
				{
					this->Pattern.patternSpeed *= -1;
				}
				else
				{
					this->speed.x *= -1;
				}
				this->setPosition(this->m_size.x / 2, newposition.y);
			}

			else if (newposition.x > SCENE_SIZE_X - this->m_size.x / 2)
			{
				if ((*CurrentGame).direction == DIRECTION_UP || (*CurrentGame).direction == DIRECTION_DOWN)
				{
					this->Pattern.patternSpeed *= -1;
				}
				else
				{
					this->speed.x *= -1;
				}
				this->setPosition(SCENE_SIZE_X - this->m_size.x / 2, newposition.y);
			}
		}
		
		if (this->bouncing != BouncingHorizontal && hyperspeedMultiplier <= 1)
		{
			if (newposition.y < this->m_size.y / 2)
			{
				if ((*CurrentGame).direction == DIRECTION_UP || (*CurrentGame).direction == DIRECTION_DOWN)
				{
					this->speed.y *= -1;
				}
				else
				{
					this->currentPhase->Pattern->patternSpeed *= -1;
				}
				this->setPosition(newposition.x, this->m_size.y / 2);
			}

			else if (newposition.y > SCENE_SIZE_Y - this->m_size.y / 2)
			{
				if ((*CurrentGame).direction == DIRECTION_UP || (*CurrentGame).direction == DIRECTION_DOWN)
				{
					this->speed.y *= -1;
				}
				else
				{
					this->currentPhase->Pattern->patternSpeed *= -1;
				}
				this->setPosition(newposition.x, SCENE_SIZE_Y - this->m_size.y / 2);
			}
		}
	}

	//calculating the angle we want to face, if any
 	float target_angle = this->getRotation();

	//calculating the angle we want to face, if any
	bool isNearestTargetIsKnown = false;
	if (this->reset_facing)
	{
		target_angle = Independant::getRotation_for_Direction((*CurrentGame).direction);	
	}
	else if (this->face_target)
	{
		target_angle = fmod(180 + Independant::getRotation_for_Direction((*CurrentGame).direction) - (*CurrentGame).GetAngleToNearestIndependant(IndependantType::PlayerShip, this->getPosition()), 360);
		isNearestTargetIsKnown = true;
	}

	//calculating the weapons cooldown and targets
	bool isDoneFiringOnLockedTarget = true;
	for (std::vector<Weapon*>::iterator it = this->weapons_list.begin(); it != this->weapons_list.end(); it++)
	{
		if ((*it)->isFiringReady(deltaTime, hyperspeedMultiplier))//update all weapons cooldown
		{
			(*it)->m_isReadyToFire = true;
			//now acquire target is the weapons needs to
			if (!isNearestTargetIsKnown)//maybe we know it already?
			{
				if ((*it)->target_seaking == SEAKING || ((*it)->target_seaking == SEMI_SEAKING && (*it)->rafale_index == 0))
				{
					target_angle = fmod(180 + Independant::getRotation_for_Direction((*CurrentGame).direction) - (*CurrentGame).GetAngleToNearestIndependant(IndependantType::PlayerShip, this->getPosition()), 360);
				}
			}
		}
		else
		{
			(*it)->m_isReadyToFire = false;
		}

		//semi-seaking and rafale not ended or alternated multishot not ended need to keep the enemy oriented to the same target if it's "semi_seaking"
		if (this->face_target)
		{
			if ((*it)->target_seaking == SEMI_SEAKING && (*it)->rafale > 0 && (((*it)->rafale_index > 0 && (*it)->rafale_index < (*it)->rafale) || ((*it)->multishot > 1 && (*it)->shot_index > 0)))
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

	if (!this->face_target && !this->reset_facing)
	{
		this->rotate(this->rotation_speed*deltaTime.asSeconds() * l_hyperspeedMultiplier);
	}
	else
	{
		if (isDoneFiringOnLockedTarget)
		{
			//now let's rotate toward the target (the player)
			if (delta >= 0)
			{
				if (abs(delta) > abs(this->rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier)
				{
					this->rotate(-abs(this->rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier);
				}
				else
				{
					this->setRotation(target_angle);
				}
			}
			else
			{
				if (abs(delta) > abs(this->rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier)
				{
					this->rotate(abs(this->rotation_speed)*deltaTime.asSeconds() * l_hyperspeedMultiplier);
				}
				else
				{
					this->setRotation(target_angle);
				}
			}
		}
	}

	//automatic fire
	if (!this->weapons_list.empty())
	{
		if (this->isOnScene)
		{
			for (std::vector<Weapon*>::iterator it = this->weapons_list.begin(); it != this->weapons_list.end(); it++)
			{
				if (!this->disable_fire)
				{
					if (this->phaseTimer.asSeconds() < (*it)->delay)
					{
						//do nothing: this weapon is settled to start firing later (delay)
					}
					else if ((*it)->m_isReadyToFire)
					{
						if (this->face_target && abs(delta) > 1.0f && isDoneFiringOnLockedTarget)//let's take delta>1 as an epsilon
						{
							//do nothing
						}
						else
						{
							if ((*it)->target_seaking == SEMI_SEAKING && (*it)->rafale > 0 && (((*it)->rafale_index > 0 && (*it)->rafale_index < (*it)->rafale) || ((*it)->multishot > 1 && (*it)->shot_index > 0)))
							{
								//semi-seaking and rafale not ended or alternated multishot not ended = no update of target or weapon position
							}
							else
							{
								//here we add delta so that we virtually move the weapon around the enemy, so that he can always shoot at 360 degrees with the same nice spread
								float theta = (this->getRotation() + (*it)->angle_offset) / 180 * M_PI;
								if ((*it)->target_seaking != NO_SEAKING)
								{
									theta -= delta / 180 * M_PI;
								}

								(*it)->weapon_current_offset.x = (*it)->weaponOffset.x - this->m_size.x / 2 * sin(theta);
								(*it)->weapon_current_offset.y = (*it)->weaponOffset.y + this->m_size.y / 2 * cos(theta);

								//transmitting the angle to the weapon, which will pass it to the bullets
								(*it)->shot_angle = theta;
							}

							(*it)->setPosition(this->getPosition().x + (*it)->weapon_current_offset.x, this->getPosition().y + (*it)->weapon_current_offset.y);
							(*it)->face_target = this->face_target;

							(*it)->Fire(IndependantType::EnemyFire, deltaTime, hyperspeedMultiplier);
							{
								m_shots_fired++;
							}
						}
					}
				}
			}
		}
	}
	
	//sheld regen if not maximum
	if (shield < getIndependantShield())
	{
		shield += getIndependantShieldRegen();
		//canceling over-regen
		if (shield > getIndependantShield())
		{
			shield = getIndependantShield();
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
	if (this->hasPhases)
	{
		if (this->currentPhase->hasTransition)
		{
			this->CheckCondition();
		}
	}
}

void Enemy::RotateFeedbacks(float angle)
{
	armorBar->setRotation(angle);
	armorBarContainer->setRotation(angle);
	shieldBar->setRotation(angle);
	shieldBarContainer->setRotation(angle);
}

void Enemy::damage_from(Independant& independant)
{
	if (!immune)
	{
		if (feedbackTimer <= sf::seconds(0))
		{
			(*CurrentGame).addToFeedbacks(armorBarContainer);
			(*CurrentGame).addToFeedbacks(armorBar);
			if (shield_max > 0)
			{
				(*CurrentGame).addToFeedbacks(shieldBarContainer);
				(*CurrentGame).addToFeedbacks(shieldBar);
			}
			(*CurrentGame).addToFeedbacks(&enemyLevel);
		}

		feedbackTimer = sf::seconds(ENEMY_HEALTH_FEEDBACK_TIME);
		setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));

		if (independant.damage > shield)
		{
			armor -= (independant.damage - shield);
			shield = 0;
		}
		else
		{
			shield -= independant.damage;
		}
	}
}

Enemy* Enemy::Clone()
{
	sf::Vector2f s = this->speed;

	Enemy* enemy = new Enemy(this->getPosition(), this->speed, this->textureName, this->m_size, this->FX_death, this->frameNumber, this->animationNumber);

	((Independant*)enemy)->armor = this->getIndependantArmor();
	((Independant*)enemy)->armor_max = this->getIndependantArmorMax();
	((Independant*)enemy)->shield = this->getIndependantShield();
	((Independant*)enemy)->shield_max = this->getIndependantShieldMax();
	((Independant*)enemy)->shield_regen = this->getIndependantShieldRegen();
	((Independant*)enemy)->damage = this->getIndependantDamage();

	for (std::vector<Weapon*>::iterator it = (this->weapons_list.begin()); it != (this->weapons_list.end()); it++)
	{
		enemy->weapons_list.push_back((*it)->Clone());
	}	
	
	((Independant*)enemy)->addMoney(this->getMoney());
	enemy->equipment_loot = this->getEquipmentLoot();
	enemy->weapon_loot = this->getWeaponLoot();
	enemy->display_name = this->display_name;
	enemy->enemy_class = this->enemy_class;

	enemy->Pattern = this->Pattern;
	enemy->angspeed = this->angspeed;
	enemy->radius = this->radius;

	enemy->rotation_speed = this->rotation_speed;

	if (this->hasPhases)
	{
		enemy->hasPhases = this->hasPhases;
		enemy->currentPhase = this->currentPhase;
		enemy->setPhase(this->currentPhase);
		for (std::vector<Phase*>::iterator it = (this->phases.begin()); it != (this->phases.end()); it++)
		{
			enemy->phases.push_back((*it));
		}
	}

	enemy->level = this->level;
	enemy->enemyLevel.setString(to_string(this->level));

	return enemy;
}

Phase* Enemy::getPhase(string phaseName)
{
	for (vector<Phase*>::iterator it = this->phases.begin(); it != this->phases.end(); it++)
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
	for (std::vector<ConditionTransition*>::iterator it = (this->currentPhase->transitions_list.begin()); it != (this->currentPhase->transitions_list.end()); it++)
	{
		switch ((*it)->condition)
		{
			case ConditionType::VerticalPosition:
			{
				FloatCompare result = this->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f((*it)->value / SCENE_SIZE_Y * SCENE_SIZE_X, (*it)->value));
				if (result == (*it)->op)
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					return true;
				}
													  
				break;
			}

			case ConditionType::HorizontalPosition:
			{
				FloatCompare result = this->compare_posX_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f((*it)->value, (*it)->value / SCENE_SIZE_X * SCENE_SIZE_Y));
				if (result == (*it)->op)
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					return true;
				}
			
				break;
			}
		
			case ConditionType::phaseClock:
			{
				if ((this->phaseTimer > sf::seconds((*it)->value)) && (*it)->op == FloatCompare::GREATHER_THAN)
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					return true;
				}
				else if ((this->phaseTimer < sf::seconds((*it)->value)) && (*it)->op == FloatCompare::LESSER_THAN)
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					return true;
				}

				break;
			}
		
			case ConditionType::enemyClock:
			{
				if ((this->enemyTimer > sf::seconds((*it)->value)) && (*it)->op == FloatCompare::GREATHER_THAN)
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					this->enemyTimer = sf::seconds(0);
					return true;
				}
				else if ((this->enemyTimer < sf::seconds((*it)->value)) && (*it)->op == FloatCompare::LESSER_THAN)
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					this->enemyTimer = sf::seconds(0);
					return true;
				}
										   
				break;
			}
		
			case ConditionType::LifePourcentage:
			{
				if ((100.0f * this->getIndependantArmor() / this->getIndependantArmorMax() >= (*it)->value) && (((*it)->op == FloatCompare::GREATHER_THAN) || ((*it)->op == FloatCompare::EQUAL_TO)))
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));

					return true;
				}
				else if ((100.0f * this->getIndependantArmor() / this->getIndependantArmorMax() <= (*it)->value) && (((*it)->op == FloatCompare::LESSER_THAN) || ((*it)->op == FloatCompare::EQUAL_TO)))
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					return true;
				}
				break;
			}
		
			case ConditionType::ShieldPourcentage:
			{
				//Caution, we don't want to be diving 0 / 0, so we need to handle separately the cases where ShieldMax worth 0 (enemy using no shield).
				if ((*it)->op == FloatCompare::GREATHER_THAN)
				{
					if (this->getIndependantShieldMax() == 0)
					{
						break;
					}
					else if (100.0f * this->getIndependantShield() / this->getIndependantShieldMax() >= (*it)->value)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
					break;
				}
				else if ((*it)->op == FloatCompare::LESSER_THAN)
				{
					if (this->getIndependantShieldMax() == 0)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
					else if (100.0f * this->getIndependantShield() / this->getIndependantShieldMax() <= (*it)->value)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
					break;
				}
				else if ((*it)->op == FloatCompare::EQUAL_TO)
				{
					if (this->getIndependantShieldMax() == 0)
					{
						if ((*it)->value == 0)
						{
							this->setPhase(this->getPhase((*it)->nextPhase_name));
							return true;
						}
						break;
					}
					else if (100.0f * this->getIndependantShield() / this->getIndependantShieldMax() == (*it)->value)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
					break;
				}
			}

			case ConditionType::wakeUp:
			{
				if (this->wake_up)
				{
					this->setPhase(this->getPhase((*it)->nextPhase_name));
					return true;
				}
				break;
			}

			case ConditionType::EnemyProximity:
			{
				if ((*it)->op == FloatCompare::GREATHER_THAN)
				{
					if ((*CurrentGame).FoundNearestIndependant(IndependantType::PlayerShip, this->getPosition(), (*it)->value) == TargetScan::TARGET_OUT_OF_RANGE)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
				}
				else if ((*it)->op == FloatCompare::LESSER_THAN || (*it)->op == FloatCompare::EQUAL_TO)
				{
					if ((*CurrentGame).FoundNearestIndependant(IndependantType::PlayerShip, this->getPosition(), (*it)->value) == TargetScan::TARGET_IN_RANGE)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
				}							  
				break;
			}

			case ConditionType::ShotsFired:
			{
				if ((*it)->op == FloatCompare::GREATHER_THAN)
				{
					if (this->m_shots_fired >= (*it)->value)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
				}
				else if ((*it)->op == FloatCompare::EQUAL_TO)
				{
					if (this->m_shots_fired == (*it)->value)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
						return true;
					}
				}
				else if ((*it)->op == FloatCompare::LESSER_THAN)
				{
					if (this->m_shots_fired < (*it)->value)
					{
						this->setPhase(this->getPhase((*it)->nextPhase_name));
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
	this->currentPhase = phase;
	this->m_shots_fired = 0;

	this->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, phase->vspeed);

	//reset old stats
	this->immune = false;
	this->setGhost(false);
	this->face_target = false;
	this->reset_facing = false;
	this->bouncing = NoBouncing;

	//load new stats
	for (int i = 0; i < this->currentPhase->modifiers.size(); i++)
	{
		switch (phase->modifiers[i])
		{
			case Modifier::NoModifier:
			{
				//do nothing
				break;
			}
			case Modifier::Immune:
			{
				this->immune = true;
				break;
			}
			case Modifier::Ghost:
			{
				this->setGhost(true);
				break;
			}
			case Modifier::Death:
			{
				this->Death();
				break;
			}
			case Modifier::FaceTarget:
			{
				this->face_target = true;
				break;
			}
			case Modifier::ResetFacing:
			{
				this->reset_facing = true;
				break;
			}
			case Modifier::Bouncing:
			{
				this->bouncing = BouncingEverywhere;
				break;
			}
			case Modifier::BouncingH:
			{
				this->bouncing = BouncingHorizontal;
				break;
			}
			case Modifier::BouncingV:
			{
				this->bouncing = BouncingVertical;
				break;
			}
		}
	}

	//clearing old weapons and setting new ones
	this->weapons_list.clear();
	for (std::vector<Weapon*>::iterator it = phase->weapons_list.begin(); it != phase->weapons_list.end(); it++)
	{
		this->weapons_list.push_back((*it)->Clone());
	}

	//movement
	this->Pattern.SetPattern(phase->Pattern->currentPattern, phase->Pattern->patternSpeed, phase->Pattern->patternParams); //vitesse angulaire (degres/s)
	this->rotation_speed = phase->rotation_speed;

	//welcome shot: shot once at the beginning of the phase (actually used as a post-mortem "good-bye"shoot)
	if (phase->hasWelcomeShot)
	{
		float theta = this->getRotation() / 180 * M_PI;
		float weapon_offset_x = phase->welcomeWeapon->weaponOffset.x - this->m_size.y / 2 * sin(theta);
		float weapon_offset_y = phase->welcomeWeapon->weaponOffset.y + this->m_size.y / 2 * cos(theta);

		phase->welcomeWeapon->setPosition(this->getPosition().x + weapon_offset_x, this->getPosition().y + weapon_offset_y);
		phase->welcomeWeapon->shot_angle = theta;

		phase->welcomeWeapon->Fire(IndependantType::EnemyFire);
	}

	//setting up wake_up condition
	bool wake_up_condition_exists = false;
	for (std::vector<ConditionTransition*>::iterator it = (phase->transitions_list.begin()); it != (phase->transitions_list.end()); it++)
	{
		if ((*it)->condition == ConditionType::wakeUp)
		{
			this->wake_up = false;
			wake_up_condition_exists = true;
		}
	}
	//reset the flag "wake_up" if this phase doesn't use the condition (it means the awakening has been skipped by another phase transition
	if (!wake_up_condition_exists)
	{
		this->wake_up = true;
	}

	//waking up enemies
	if (phase->hasWakeUp)
	{
		(*CurrentGame).WakeUpEnemiesWithName(phase->wake_up_name);
	}

	this->phaseTimer = sf::seconds(0);
}

Phase* Enemy::LoadPhase(string name)
{
	vector<vector<string>> phaseConfig = *(FileLoaderUtils::FileLoader(PHASES_FILE));

	for (std::vector<vector<string>>::iterator it = (phaseConfig).begin(); it != (phaseConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Phase* phase = new Phase();

			phase->m_name = name;
			phase->display_name = (*it)[EnemyPhaseData::PHASE_NAME];
			phase->vspeed = stoi((*it)[EnemyPhaseData::PHASE_VSPEED]);

			//loading weapons and ammos
			for (int i = 0; i < 6; i++)
			{
				if ((*it)[PHASE_WEAPON + (i * 4)].compare("0") != 0)
				{
					Weapon* m_weapon = Enemy::LoadWeapon((*it)[PHASE_WEAPON + (i * 4)], 1, Enemy::LoadAmmo((*it)[PHASE_AMMO + (i * 4)]));
					m_weapon->weaponOffset.x = atof((*it)[PHASE_WEAPON_OFFSET + (i * 4)].c_str());
					m_weapon->delay = atof((*it)[PHASE_WEAPON_DELAY + (i * 4)].c_str());
					phase->weapons_list.push_back(m_weapon);
					
				}
			}

			//loading phases
			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), EnemyPhaseData::PHASE_PATTERN);
			phase->Pattern = m_bobby;

			//loading rotation speed
			phase->rotation_speed = stoi((*it)[EnemyPhaseData::PHASE_ROTATION_SPEED]);

			//loading modifier (immune to damage, etc.)
			
			for (int i = 0; i < 2; i++)
			{
				Modifier l_new_modifier = Modifier::NoModifier;
				if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("0") != 0)
				{
					if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("immune") == 0)
					{
						l_new_modifier = Modifier::Immune;
					}
					else if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("ghost") == 0)
					{
						l_new_modifier = Modifier::Ghost;
					}
					else if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("death") == 0)
					{
						l_new_modifier = Modifier::Death;
					}
					else if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("face_target") == 0)
					{
						l_new_modifier = Modifier::FaceTarget;
					}
					else if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("reset_facing") == 0)
					{
						l_new_modifier = Modifier::ResetFacing;
					}
					else if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("bouncing") == 0)
					{
						l_new_modifier = Modifier::Bouncing;
					}
					else if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("bouncingH") == 0)
					{
						l_new_modifier = Modifier::BouncingH;
					}
					else if ((*it)[EnemyPhaseData::PHASE_MODIFIER + i].compare("bouncingV") == 0)
					{
						l_new_modifier = Modifier::BouncingV;
					}
				}

				phase->modifiers.push_back(l_new_modifier);
			}

			//loading welcome shot
			if ((*it)[EnemyPhaseData::PHASE_WELCOME_WEAPON].compare("0") != 0)
			{
				phase->welcomeWeapon = Enemy::LoadWeapon((*it)[EnemyPhaseData::PHASE_WELCOME_WEAPON], 1, Enemy::LoadAmmo((*it)[EnemyPhaseData::PHASE_WELCOME_AMMO]));
				phase->hasWelcomeShot = true;
			}
			
			//load enemies (by name) to wake up
			if ((*it)[EnemyPhaseData::PHASE_WAKEUP].compare("0") != 0)
			{
				phase->wake_up_name = (*it)[EnemyPhaseData::PHASE_WAKEUP];
				phase->hasWakeUp = true;
			}

			//loading transition to next phase
			if ((*it)[EnemyPhaseData::PHASE_CONDITION].compare("0") != 0)
			{
				phase->hasTransition = true;
				phase->transitions_list.push_back(Phase::ConditionLoader((*it), EnemyPhaseData::PHASE_CONDITION));
			}
			if ((*it)[EnemyPhaseData::PHASE_CONDITION_2].compare("0") != 0)
			{
				phase->hasTransition = true;
				phase->transitions_list.push_back(Phase::ConditionLoader((*it), EnemyPhaseData::PHASE_CONDITION_2));
			}

			return phase;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find EnemyPhase '%s'. Please check the config file", name));
}

void Enemy::Death()
{
	FX* myFX = this->FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	(*CurrentGame).addToScene(myFX,LayerType::ExplosionLayer, IndependantType::Neutral);

	//Score
	(*CurrentGame).hazard += this->money;

	//Player XP
	//(*CurrentGame).playerShip->gain_xp(XPTable_PerEnemyClass[this->enemy_class]);

	//Loot
	if (CreateRandomLootv2(this->enemy_class, (*CurrentGame).BeastScoreBonus))
	{
		GenerateLoot();
	}

	this->visible = false;
	this->isOnScene = false;
	this->GarbageMe = true;
}

void Enemy::Destroy()
{
	if (armorBar)
	{
		(*CurrentGame).removeFromFeedbacks(armorBar);
		(*CurrentGame).removeFromFeedbacks(armorBarContainer);
	}
	if (shieldBar)
	{
		(*CurrentGame).removeFromFeedbacks(shieldBar);
		(*CurrentGame).removeFromFeedbacks(shieldBarContainer);
	}
	(*CurrentGame).removeFromFeedbacks(&enemyLevel);
}

void Enemy::GenerateLoot()
{
	sf::Vector2f speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, LOOT_SPEED_Y);

	if (this->weapon_loot != NULL)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, this->getWeaponLoot()->textureName, sf::Vector2f(this->getWeaponLoot()->size.x, this->getWeaponLoot()->size.y), this->getWeaponLoot()->display_name);
		new_loot->get_weapon_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::LootLayer, IndependantType::LootObject);
	}

	else if (this->equipment_loot != NULL)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, this->getEquipmentLoot()->textureName, sf::Vector2f(this->getEquipmentLoot()->size.x, this->getEquipmentLoot()->size.y), this->getEquipmentLoot()->display_name);
		new_loot->get_equipment_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::LootLayer, IndependantType::LootObject);
	}

	else if (this->money > 0)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Money");
		new_loot->get_money_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::LootLayer, IndependantType::LootObject);
	}
	else
	{
		//que dalle
	}
}

bool Enemy::CreateRandomLootv2(EnemyClass loot_class, float BeastScaleBonus, bool force_BeastScale, float BeastScale_min, float BeastScale_max)
{
	double random_number = (double)rand() / (RAND_MAX);

	if (random_number > LootTable_DroppingSomething[loot_class])
	{
		//EMPTY DROP
		return false;
	}
	else
	{
		//setting min and max beast score values
		sf::Vector2f LootTable_BeastScale[EnemyClass::NBVAL_EnemyClass];
		for (int b = 0; b < EnemyClass::NBVAL_EnemyClass; b++)
		{
			if (b != EnemyClass::ENEMYPOOL_VOID && b != EnemyClass::ENEMYPOOL_ZETA)
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
			int loot_credits_ = ceil(BeastScaleScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(this->level));

			//Equipment type
			int equipment_type_roll = rand() % ((int)EquipmentType::NBVAL_Equipment + 1);//+1 is for the weapon type

			//-----CHOSING RANDOM PROPERTIES-----
			int properties_to_choose_from = LootTable_MaxPropertiesPerEquipmentType[equipment_type_roll];

			//"Spending credits" on item stats
			switch (equipment_type_roll)
			{
				case (int)EquipmentType::Engine:
				{
					this->setEquipmentLoot(Equipment::CreateRandomEngine(loot_credits_, this->level));
					break;
				}

				case (int)EquipmentType::Armor:
				{
					this->setEquipmentLoot(Equipment::CreateRandomArmor(loot_credits_, this->level));
					break;
				}
				
				case (int)EquipmentType::Shield:
				{
					this->setEquipmentLoot(Equipment::CreateRandomShield(loot_credits_, this->level));
					break;
				}

				case (int)EquipmentType::Module:
				{				   
					this->setEquipmentLoot(Equipment::CreateRandomModule(loot_credits_, this->level));
					break;
				}

				case (int)EquipmentType::NBVAL_Equipment://WEAPON DROP
				{
					this->setWeaponLoot(Weapon::CreateRandomWeapon(loot_credits_, this->level, false));
					break;
				}
			}
		}

		else
		{
			//MONEY DROP
			int money = RandomizeIntBetweenRatios(1.0f * XPTable_PerEnemyClass[loot_class] * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level) / 100, LootTable_BeastScale[loot_class]);
			this->addMoney(money);//looting money
		}
	}

	return true;
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
	vector<vector<string>> weaponConfig = *(FileLoaderUtils::FileLoader(WEAPON_FILE));

	for (std::vector<vector<string>>::iterator it = (weaponConfig).begin(); it != (weaponConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Weapon* weapon = new Weapon(ammo);
			weapon->display_name = (*it)[WeaponData::WEAPON_DISPLAY_NAME];
			weapon->fire_direction = Vector2i(0, fire_direction);
			weapon->rate_of_fire = atof((*it)[WeaponData::WEAPON_RATE_OF_FIRE].c_str());
			weapon->shot_mode = ShotMode::NoShotMode;

			weapon->multishot = stoi((*it)[WeaponData::WEAPON_MULTISHOT]);
			if (weapon->multishot > 1)
			{
				weapon->xspread = stoi((*it)[WeaponData::WEAPON_XSPREAD]);
				weapon->dispersion = stoi((*it)[WeaponData::WEAPON_DISPERSION]);
				if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("0") != 0)
				{
					if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("alternate") == 0)
						weapon->shot_mode = AlternateShotMode;
					if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("ascending") == 0)
						weapon->shot_mode = AscendingShotMode;
					if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("descending") == 0)
						weapon->shot_mode = DescendingShotMode;
					if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("ascending2") == 0)
						weapon->shot_mode = Ascending2ShotMode;
					if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("descending2") == 0)
						weapon->shot_mode = Descending2ShotMode;
				}
			}
			
			weapon->rafale = stoi((*it)[WeaponData::WEAPON_RAFALE]);
			if (weapon->rafale != 0)
			{
				weapon->rafale_cooldown = atof((*it)[WeaponData::WEAPON_RAFALE_COOLDOWN].c_str());
			}
				
			weapon->textureName = (*it)[WeaponData::WEAPON_IMAGE_NAME];
			weapon->size = sf::Vector2f(stoi((*it)[WeaponData::WEAPON_WIDTH]), stoi((*it)[WeaponData::WEAPON_HEIGHT]));
			weapon->frameNumber = stoi((*it)[WeaponData::WEAPON_FRAMES]);
			weapon->angle_offset = stoi((*it)[WeaponData::WEAPON_ANGLE_OFFSET]);

			if ((*it)[WeaponData::WEAPON_TARGET_SEAKING].compare("0") != 0)
			{
				if ((*it)[WeaponData::WEAPON_TARGET_SEAKING].compare("semi_seaking") == 0)
					weapon->target_seaking = SEMI_SEAKING;
				else if ((*it)[WeaponData::WEAPON_TARGET_SEAKING].compare("seaking") == 0)
					weapon->target_seaking = SEAKING;
				else if ((*it)[WeaponData::WEAPON_TARGET_SEAKING].compare("super_seaking") == 0)
					weapon->target_seaking = SUPER_SEAKING;
			}

			return weapon;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Weapon '%s'. Please check the config file", name));
}

Ammo* Enemy::LoadAmmo(string name)
{
	vector<vector<string>> ammoConfig = *(FileLoaderUtils::FileLoader(AMMO_FILE));

	for (std::vector<vector<string>>::iterator it = (ammoConfig).begin(); it != (ammoConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Ammo* new_ammo = new Ammo(Vector2f(0, 0), Vector2f(0, stoi((*it)[AmmoData::AMMO_SPEED])), (*it)[AmmoData::AMMO_IMAGE_NAME],
				Vector2f(stoi((*it)[AmmoData::AMMO_WIDTH]), stoi((*it)[AmmoData::AMMO_HEIGHT])), stoi((*it)[AmmoData::AMMO_DAMAGE]), LoadFX((*it)[AmmoData::AMMO_FX]));
			new_ammo->display_name = (*it)[AmmoData::AMMO_NAME];
			new_ammo->range = stoi((*it)[AmmoData::AMMO_RANGE]);
			
			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), AmmoData::AMMO_PATTERN);
			new_ammo->Pattern.SetPattern(m_bobby->currentPattern, m_bobby->patternSpeed, m_bobby->patternParams);

			new_ammo->rotation_speed = stoi((*it)[AmmoData::AMMO_ROTATION_SPEED]);
			return new_ammo;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file", name));
}

FX* Enemy::LoadFX(string name)
{
	vector<vector<string>>FXConfig = *(FileLoaderUtils::FileLoader(FX_FILE));

	for (std::vector<vector<string>>::iterator it = (FXConfig).begin(); it != (FXConfig).end(); it++)
	{
		if ((*it)[FXData::FX_TYPE].compare("explosion") == 0)
		{
			if ((*it)[FXData::FX_NAME].compare(name) == 0)
			{
				float duration = atof(((*it)[FXData::FX_DURATION]).c_str());
				FX* myFX = new FX(Vector2f(0, 0), Vector2f(0, 0), (*it)[FXData::FX_FILENAME], Vector2f(stoi((*it)[FXData::FX_WIDTH]), stoi((*it)[FXData::FX_HEIGHT])), stoi((*it)[FXData::FX_FRAMES]), sf::seconds(duration));
				myFX->display_name = (*it)[FXData::FX_NAME];

				return myFX;
			}
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find FX '%s'. Please check the config file", name));

}

void Enemy::ApplyLevelModifiers()
{
	float multiplier_ = ceil(1.0f * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(this->level) / 100);

	this->armor_max = floor(this->armor_max * multiplier_);
	this->shield_max = floor(this->shield_max * multiplier_);
	this->shield_regen = floor(this->shield_regen * multiplier_);
	this->damage = floor(this->damage * multiplier_);
	for (std::vector<Weapon*>::iterator it = this->weapons_list.begin(); it != this->weapons_list.end(); it++)
	{
		(*it)->ammunition->damage = ceil((*it)->ammunition->damage * multiplier_);
	}

	enemyLevel.setString(to_string(this->level));
}