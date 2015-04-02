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
}

void Enemy::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	//shield regen if not maximum
	static double shield_regen_buffer = 0;
	if (shield < shield_max)
	{
		shield_regen_buffer += shield_regen*deltaTime.asSeconds();
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
		newspeed = this->getSpeedYMultiplier_for_Direction((*CurrentGame).direction, hyperspeedMultiplier);
	}
	else if (hyperspeedMultiplier < 1)
	{
		newspeed.x = this->speed.x * hyperspeedMultiplier;
		newspeed.y = this->speed.y * hyperspeedMultiplier;
	}

	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	//call bobbyPattern
	offset = Pattern.GetOffset(deltaTime.asSeconds());
	offset = Independant::getSpeed_for_Direction((*CurrentGame).direction, offset);
	newposition.x += offset.x;
	newposition.y += offset.y;

	this->setPosition(newposition.x, newposition.y);

	//rotation
	//calculating the angle we want to face, if any
	float target_angle = 0;
	if (this->reset_facing)
	{
		target_angle = Independant::getRotation_for_Direction((*CurrentGame).direction);	
	}
	else
	{
		target_angle = fmod(180 + Independant::getRotation_for_Direction((*CurrentGame).direction) - (*CurrentGame).GetAngleToNearestIndependant(IndependantType::PlayerShip, this->getPosition()), 360);
	}

	float current_angle = this->getRotation();
	float delta = current_angle - target_angle;
	if (delta > 180)
		delta -= 360;
	else if (delta < -180)
		delta += 360;

	bool isDoneFiringOnLockedTarget = true;

	if (!this->face_target && !this->reset_facing)
	{
		this->rotate(this->rotation_speed*deltaTime.asSeconds());
	}
	else
	{
		//if one of the weapon is semi-seaking and the enemy has to face the target, then it cannot rotate until he's done firing
		for (std::vector<Weapon*>::iterator it = this->weapons_list.begin(); it != this->weapons_list.end(); it++)
		{
			if ((*it)->target_seaking == TargetSeaking::SEMI_SEAKING && (*it)->rafale_index > 0 && (*it)->rafale_index < (*it)->rafale)
			{
				isDoneFiringOnLockedTarget = false;
			}
		}

		if (isDoneFiringOnLockedTarget)
		{
			//now let's rotate toward the target (the player)
			if (delta >= 0)
			{
				if (abs(delta) > abs(this->rotation_speed)*deltaTime.asSeconds())
				{
					this->rotate(-abs(this->rotation_speed)*deltaTime.asSeconds());
				}
				else
				{
					this->setRotation(target_angle);
				}
			}
			else
			{
				if (abs(delta) > abs(this->rotation_speed)*deltaTime.asSeconds())
				{
					this->rotate(abs(this->rotation_speed)*deltaTime.asSeconds());
				}
				else
				{
					this->setRotation(target_angle);
				}
			}
		}
	}

	//automatic fire
	if (this->isOnScene && !this->weapons_list.empty())
	{
		for (std::vector<Weapon*>::iterator it = this->weapons_list.begin(); it != this->weapons_list.end(); it++)
		{
			if (this->face_target && abs(delta) > 1.0f && isDoneFiringOnLockedTarget)//let's take delta>1 as an epsilon
			{
				//do nothing
			}
			else
			{
				float theta = this->getRotation() / 180 * M_PI;
				float weapon_offset_x = (*it)->weaponOffset.x - this->m_size.y / 2 * sin(theta);
				float weapon_offset_y = (*it)->weaponOffset.y + this->m_size.y / 2 * cos(theta);

				(*it)->setPosition(this->getPosition().x + weapon_offset_x, this->getPosition().y + weapon_offset_y);

				//transmitting the angle to the weapon, which will pass it to the bullets
				(*it)->shot_angle = theta;
				(*it)->face_target = this->face_target;

				(*it)->Fire(IndependantType::EnemyFire);
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
	}

	AnimatedSprite::update(deltaTime);

	//phases
	if (this->hasPhases)
	{
		if (this->currentPhase->hasTransition)
		{
			this->CheckCondition();
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
	}

	return enemy;
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
					this->setPhase((*it)->nextPhase_name);
					return true;
				}
													  
				break;
			}

			case ConditionType::HorizontalPosition:
			{
				FloatCompare result = this->compare_posX_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f((*it)->value, (*it)->value / SCENE_SIZE_X * SCENE_SIZE_Y));
				if (result == (*it)->op)
				{
					this->setPhase((*it)->nextPhase_name);
					return true;
				}
			
				break;
			}
		
			case ConditionType::phaseClock:
			{
				if ((this->phaseClock.getElapsedTime() > sf::seconds((*it)->value)) && (*it)->op == FloatCompare::GREATHER_THAN)
				{
					this->setPhase((*it)->nextPhase_name);
					return true;
				}
				else if ((this->phaseClock.getElapsedTime() < sf::seconds((*it)->value)) && (*it)->op == FloatCompare::LESSER_THAN)
				{
					this->setPhase((*it)->nextPhase_name);
					return true;
				}

				break;
			}
		
			case ConditionType::enemyClock:
			{
				if ((this->enemyClock.getElapsedTime() > sf::seconds((*it)->value)) && (*it)->op == FloatCompare::GREATHER_THAN)
				{
					this->setPhase((*it)->nextPhase_name);
					this->enemyClock.restart();
					return true;
				}
				else if ((this->enemyClock.getElapsedTime() < sf::seconds((*it)->value)) && (*it)->op == FloatCompare::LESSER_THAN)
				{
					this->setPhase((*it)->nextPhase_name);
					this->enemyClock.restart();
					return true;
				}
										   
				break;
			}
		
			case ConditionType::LifePourcentage:
			{
				if ((100.0f * this->getIndependantArmor() / this->getIndependantArmorMax() >= (*it)->value) && (((*it)->op == FloatCompare::GREATHER_THAN) || ((*it)->op == FloatCompare::EQUAL_TO)))
				{
					this->setPhase((*it)->nextPhase_name);

					return true;
				}
				else if ((100.0f * this->getIndependantArmor() / this->getIndependantArmorMax() <= (*it)->value) && (((*it)->op == FloatCompare::LESSER_THAN) || ((*it)->op == FloatCompare::EQUAL_TO)))
				{
					this->setPhase((*it)->nextPhase_name);
					return true;
				}
				//Debug:
				//printf("vie: %d. vie max: %d. shield: %d\n", this->getIndependantArmor(), this->getIndependantArmorMax(), this->getIndependantShield());
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
						this->setPhase((*it)->nextPhase_name);
						return true;
					}
					break;
				}
				else if ((*it)->op == FloatCompare::LESSER_THAN)
				{
					if (this->getIndependantShieldMax() == 0)
					{
						this->setPhase((*it)->nextPhase_name);
						return true;
					}
					else if (100.0f * this->getIndependantShield() / this->getIndependantShieldMax() <= (*it)->value)
					{
						this->setPhase((*it)->nextPhase_name);
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
							this->setPhase((*it)->nextPhase_name);
							return true;
						}
						break;
					}
					else if (100.0f * this->getIndependantShield() / this->getIndependantShieldMax() == (*it)->value)
					{
						this->setPhase((*it)->nextPhase_name);
						return true;
					}
					break;
				}
			}

			case ConditionType::wakeUp:
			{
				if (this->wake_up)
				{
					this->setPhase((*it)->nextPhase_name);
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
						this->setPhase((*it)->nextPhase_name);
						return true;
					}
				}
				else if ((*it)->op == FloatCompare::LESSER_THAN || (*it)->op == FloatCompare::EQUAL_TO)
				{
					if ((*CurrentGame).FoundNearestIndependant(IndependantType::PlayerShip, this->getPosition(), (*it)->value) == TargetScan::TARGET_IN_RANGE)
					{
						this->setPhase((*it)->nextPhase_name);
						return true;
					}
				}							  
				break;
			}
		}
	}

	return false;
}

void Enemy::setPhase(Phase* m_phase)
{
	this->currentPhase = m_phase;
	this->phaseClock.restart();

	this->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, m_phase->vspeed);

	//reset old stats
	this->immune = false;
	this->setGhost(false);
	this->face_target = false;

	//load new stats
	switch (m_phase->modifier)
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
	}

	//clearing old weapons and setting new ones
	this->weapons_list.clear();
	for (std::vector<Weapon*>::iterator it = m_phase->weapons_list.begin(); it != m_phase->weapons_list.end(); it++)
	{
		this->weapons_list.push_back((*it)->Clone());
	}

	//movement
	this->Pattern.SetPattern(m_phase->Pattern->currentPattern, m_phase->Pattern->patternSpeed, m_phase->Pattern->patternParams); //vitesse angulaire (degres/s)
	this->rotation_speed = m_phase->rotation_speed;

	//welcome shot: shot once at the beginning of the phase (actually used as a post-mortem "good-bye"shoot)
	if (m_phase->hasWelcomeShot)
	{
		float theta = this->getRotation() / 180 * M_PI;
		float weapon_offset_x = m_phase->welcomeWeapon->weaponOffset.x - this->m_size.y / 2 * sin(theta);
		float weapon_offset_y = m_phase->welcomeWeapon->weaponOffset.y + this->m_size.y / 2 * cos(theta);

		m_phase->welcomeWeapon->setPosition(this->getPosition().x + weapon_offset_x, this->getPosition().y + weapon_offset_y);
		m_phase->welcomeWeapon->shot_angle = theta;

		m_phase->welcomeWeapon->Fire(IndependantType::EnemyFire);
	}

	//setting up wake_up condition
	bool wake_up_condition_exists = false;
	for (std::vector<ConditionTransition*>::iterator it = (m_phase->transitions_list.begin()); it != (m_phase->transitions_list.end()); it++)
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
	if (m_phase->hasWakeUp)
	{
		(*CurrentGame).WakeUpEnemiesWithName(m_phase->wake_up_name);
	}
}

void Enemy::setPhase(string phase_name)
{
	Phase* phase = this->LoadPhase(phase_name);
	this->setPhase(phase);
	
}

Phase* Enemy::LoadPhase(string name)
{
	vector<vector<string>> phaseConfig = *(FileLoaderUtils::FileLoader(PHASES_FILE));

	for (std::vector<vector<string>>::iterator it = (phaseConfig).begin(); it != (phaseConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Phase* phase = new Phase();

			phase->display_name = (*it)[EnemyPhaseData::PHASE_NAME];
			phase->vspeed = stoi((*it)[EnemyPhaseData::PHASE_VSPEED]);

			//loading weapons and ammos
			if ((*it)[EnemyPhaseData::PHASE_WEAPON].compare("0") != 0)
			{
				Weapon* m_weapon = Enemy::LoadWeapon((*it)[EnemyPhaseData::PHASE_WEAPON], 1, Enemy::LoadAmmo((*it)[EnemyPhaseData::PHASE_AMMO]));
				m_weapon->weaponOffset.x = stoi((*it)[EnemyPhaseData::PHASE_WEAPON_OFFSET]);
				phase->weapons_list.push_back(m_weapon);
			}

			if ((*it)[EnemyPhaseData::PHASE_WEAPON_2].compare("0") != 0)
			{
				Weapon* m_weapon = Enemy::LoadWeapon((*it)[EnemyPhaseData::PHASE_WEAPON_2], 1, Enemy::LoadAmmo((*it)[EnemyPhaseData::PHASE_AMMO_2]));
				m_weapon->weaponOffset.x = stoi((*it)[EnemyPhaseData::PHASE_WEAPON_OFFSET_2]);
				phase->weapons_list.push_back(m_weapon);
			}

			if ((*it)[EnemyPhaseData::PHASE_WEAPON_3].compare("0") != 0)
			{
				Weapon* m_weapon = Enemy::LoadWeapon((*it)[EnemyPhaseData::PHASE_WEAPON_3], 1, Enemy::LoadAmmo((*it)[EnemyPhaseData::PHASE_AMMO_3]));
				m_weapon->weaponOffset.x = stoi((*it)[EnemyPhaseData::PHASE_WEAPON_OFFSET_3]);
				phase->weapons_list.push_back(m_weapon);
			}

			//loading phases
			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), EnemyPhaseData::PHASE_PATTERN);
			phase->Pattern = m_bobby;

			//loading rotation speed
			phase->rotation_speed = stoi((*it)[EnemyPhaseData::PHASE_ROTATION_SPEED]);

			//loading modifier (immune to damage, etc.)
			phase->modifier = Modifier::NoModifier;
			if ((*it)[EnemyPhaseData::PHASE_MODIFIER].compare("0") != 0)
			{
				if ((*it)[EnemyPhaseData::PHASE_MODIFIER].compare("immune") == 0)
				{
					phase->modifier = Modifier::Immune;
				}
				else if ((*it)[EnemyPhaseData::PHASE_MODIFIER].compare("ghost") == 0)
				{
					phase->modifier = Modifier::Ghost;
				}
				else if ((*it)[EnemyPhaseData::PHASE_MODIFIER].compare("death") == 0)
				{
					phase->modifier = Modifier::Death;
				}
				else if ((*it)[EnemyPhaseData::PHASE_MODIFIER].compare("face_target") == 0)
				{
					phase->modifier = Modifier::FaceTarget;
				}
				else if ((*it)[EnemyPhaseData::PHASE_MODIFIER].compare("reset_facing") == 0)
				{
					phase->modifier = Modifier::ResetFacing;
				}
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
			
			//loading transitions to next phase
			if ((*it)[EnemyPhaseData::PHASE_CONDITION].compare("0") != 0)
			{
				phase->hasTransition = true;
				phase->transitions_list.push_back(Phase::ConditionLoader((*it), EnemyPhaseData::PHASE_CONDITION));
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

	this->visible = false;
	this->isOnScene = false;
	this->GarbageMe = true;
}

void Enemy::GenerateLoot()
{
	sf::Vector2f speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, LOOT_SPEED_Y);

	if (this->weapon_loot != NULL)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, this->getWeaponLoot()->textureName, sf::Vector2f(this->getWeaponLoot()->size.x, this->getWeaponLoot()->size.y), this->getWeaponLoot()->display_name);
		new_loot->get_weapon_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
	}

	else if (this->equipment_loot != NULL)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, this->getEquipmentLoot()->textureName, sf::Vector2f(this->getEquipmentLoot()->size.x, this->getEquipmentLoot()->size.y), this->getEquipmentLoot()->display_name);
		new_loot->get_equipment_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
	}

	else if (this->money > 0)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Money");
		new_loot->get_money_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
	}
	else
	{
		//que dalle
	}
}

void Enemy::CreateRandomLoot(float BeastScaleBonus)
{
	int e_value = this->getMoney();
	this->setMoney(0);
	int e_class = (int)this->enemy_class;

	//Loot* loot = new Loot(this->getPosition(),sf::Vector2f(0, LOOT_SPEED_Y), LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Empty drop");

	double random_number = (double) rand() / (RAND_MAX);

	if (random_number > LootTable_DroppingSomething[e_class])
	{	
		//empty drop
	}
	else 
	{
		sf::Vector2f LootTable_BeastScale[EnemyClass::NBVAL_EnemyClass];
		for (int b=0; b<EnemyClass::NBVAL_EnemyClass; b++)
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

    	if (random_number < LootTable_DropIsEquipment[e_class]*LootTable_DroppingSomething[e_class])
		{

			//-----NUMBER OF PROPERTIES-----
  			float BeastScaleScore = RandomizeFloatBetweenValues(LootTable_BeastScale[e_class]);
			int number_of_equipment_properties = 0;
			bool epic_drop = false;
			for (int i=1; i<NUMBER_OF_BEASTSCALE_THRESHOLD; i++) 
			{
				if (BeastScaleScore > LootTable_BeastScaleThreshold[i])
				{
					//guaranteed equipment properties
					number_of_equipment_properties++;
				}
				else
				{
					//one additional chance of adding a property based on the prorata between the two surrounding threshold values
					double extra_property_roll = (double) rand() / (RAND_MAX);
					float prorata = ProrataBetweenThreshold(BeastScaleScore, sf::Vector2f (LootTable_BeastScaleThreshold[i-1],LootTable_BeastScaleThreshold[i]));
					if (extra_property_roll > prorata)
					{
						if (number_of_equipment_properties < NB_MAX_PROPERTIES_FOR_NON_EPIC_EQUIPMENT)
						{
							number_of_equipment_properties++;
						}
						else
						{
							epic_drop = true; //3 guaranteed properties + a winning roll on the 4th trades for an epic drop
						}
					}
					break; //end of the loop
				}
			}

			//looting money instead of an equipment with 0 properties
			if (number_of_equipment_properties == 0)
			{
				int money = RandomizeIntBetweenRatios(e_value, LootTable_BeastScale[e_class]);
				this->addMoney(money);
				return;
			}

			//-----EQUIPMENT TYPE-----
			int equipment_type_roll = rand() % ((int)EquipmentType::NBVAL_EQUIPMENT + 1);//+1 is for the weapon type

			//-----CHOSING RANDOM PROPERTIES-----
			int properties_to_choose_from = LootTable_MaxPropertiesPerEquipmentType[equipment_type_roll];

			vector<int> properties_roll_table(properties_to_choose_from);
			for (int i=0; i<properties_to_choose_from; i++)
				properties_roll_table[i] = i;

			if (number_of_equipment_properties > properties_to_choose_from)
				number_of_equipment_properties = properties_to_choose_from;//on va essayer d'éviter ce cas, car le joueur gaspille des propriétés puisqu'on en a pas conçu suffisamment

			switch (equipment_type_roll)
			{
				//case (int)EquipmentType::Airbrake:
				//{
				//	//Initialisation
				//	Equipment* equipment = new Equipment();
				//	float e_decceleration = EQUIPMENT_MIN_DECCELLERATION_VALUE;
				//	float log_multiplier = EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_X));
				//	if (log_multiplier > 1)
				//		e_decceleration *= log_multiplier;
				//
				//	e_decceleration = floor(e_decceleration);
				//
				//	equipment->Init((int)EquipmentType::Airbrake, 0, 0, e_decceleration, 0.f,0,0,0,0, AIRBRAKE_FILENAME,sf::Vector2f(64,64),1,"Airbrake");
				//
				//	//Adding properties
				//	for (int p=0; p<number_of_equipment_properties; p++)
				//	{
				//		int chosen_property = GetChosenProperty(&properties_roll_table,properties_to_choose_from,p);
				//		equipment->AddAirbrakeProperty(chosen_property, e_value, LootTable_BeastScale[e_class]);
				//	}
				//
				//	this->setEquipmentLoot(equipment);
				//	break;
				//}

				case (int)EquipmentType::Engine:
				{
					//Initialisation
					Equipment* equipment = new Equipment();
					float e_maxspeed = EQUIPMENT_MIN_MAXSPEED_VALUE;
					float log_multiplier = EQUIPMENT_MAXSPEED_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_MAXSPEED_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
					{
						e_maxspeed *= log_multiplier;
					}
					e_maxspeed = floor(e_maxspeed);
					float e_acceleration_x = EQUIPMENT_MIN_ACCELLERATION_VALUE;
					float e_acceleration_y = EQUIPMENT_MIN_ACCELLERATION_VALUE;

					log_multiplier = EQUIPMENT_ACCELLERATION_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_ACCELLERATION_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
					{
						e_acceleration_x *= log_multiplier;
						e_acceleration_y *= log_multiplier;
					}
					e_acceleration_x = floor(e_acceleration_x);
					e_acceleration_y = floor(e_acceleration_y);

					equipment->Init((int)EquipmentType::Engine, e_maxspeed, 0, e_acceleration_x, 0.f,0,0,0,0, THRUSTER_FILENAME,sf::Vector2f(64,64),1,"Engine");

					//Adding properties
					for (int p=0; p<number_of_equipment_properties; p++)
					{
						int chosen_property = GetChosenProperty(&properties_roll_table,properties_to_choose_from,p);
						equipment->AddEngineProperty(chosen_property, e_value, LootTable_BeastScale[e_class]);
					}

					this->setEquipmentLoot(equipment);
					break;
				}

				case (int)EquipmentType::Armor:
				{
					//Initialisation
					Equipment* equipment = new Equipment();
					float e_armor = EQUIPMENT_MIN_ARMOR_VALUE;
					float log_multiplier = EQUIPMENT_ARMOR_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_ARMOR_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
						e_armor *= log_multiplier;

					e_armor = floor(e_armor);
					equipment->Init((int)EquipmentType::Armor, 0, 0, 0.f, 0.f, e_armor,0,0,0, ARMOR_FILENAME,sf::Vector2f(64,64),1,"Armor");

					//Adding properties
					for (int p=0; p<number_of_equipment_properties; p++)
					{
						int chosen_property = GetChosenProperty(&properties_roll_table,properties_to_choose_from,p);
						equipment->AddArmorProperty(chosen_property, e_value, LootTable_BeastScale[e_class]);
					}

					this->setEquipmentLoot(equipment);
					break;
				}

				case (int)EquipmentType::Shield:
				{
					//Initialisation
					Equipment* equipment = new Equipment();
					float e_shield = EQUIPMENT_MIN_SHIELD_VALUE;
					float log_multiplier = EQUIPMENT_SHIELD_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_SHIELD_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
						e_shield *= log_multiplier;
					e_shield = floor(e_shield);
					float e_shield_regen = EQUIPMENT_MIN_SHIELD_REGEN_VALUE;

					log_multiplier = EQUIPMENT_SHIELD_REGEN_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_SHIELD_REGEN_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
						e_shield_regen *= log_multiplier;
					e_shield_regen = floor(e_shield_regen);

					equipment->Init((int)EquipmentType::Shield, 0, 0, 0.f, 0.f, 0, e_shield, e_shield_regen,0, SHIELD_FILENAME,sf::Vector2f(64,64),1,"Shield");

					//Adding properties
					for (int p=0; p<number_of_equipment_properties; p++)
					{
						int chosen_property = GetChosenProperty(&properties_roll_table,properties_to_choose_from,p);
						equipment->AddShieldProperty(chosen_property, e_value, LootTable_BeastScale[e_class]);
					}

					this->setEquipmentLoot(equipment);
					break;
				}

				case (int)EquipmentType::Module:
				{
					//Initialisation
					Equipment* equipment = new Equipment();
					equipment->Init((int)EquipmentType::Module, 0, 0, 0.f, 0.f, 0,0,0,0, MODULE_FILENAME,sf::Vector2f(64,64),1,"Module");

					Bot* bot = new Bot(sf::Vector2f (0,0), sf::Vector2f (0,0), "Assets/2D/bot.png", sf::Vector2f (32,32));
					bot->display_name = "Bot gerard";
					bot->radius = 500;
					bot->vspeed = 300;
					bot->spread = sf::Vector2f(-50,0);

					vector<float>* v = new vector<float>;
					//v->push_back(bot->radius); // rayon 500px
					//v->push_back(1);  // clockwise (>)
					PatternType pattern_type = PatternType::NoMovePattern;
					bot->Pattern.SetPattern(pattern_type,bot->vspeed,v); //vitesse angulaire (degres/s)

					FX* fx = new FX(sf::Vector2f (0,0), sf::Vector2f (0,0), "Assets/2D/FX_explosion_S_blue.png", sf::Vector2f (320,236), 2, sf::seconds(0.4f));

					float e_damage = WEAPON_MIN_DAMAGE_VALUE;
					float log_multiplier = WEAPON_DAMAGE_LN_MULTIPLIER_BASE * (log(e_value * WEAPON_DAMAGE_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
						e_damage *= log_multiplier;
					e_damage = floor(e_damage);
					Ammo* ammo = new Ammo(this->getPosition(),sf::Vector2f (0,WEAPON_MIN_VSPEED_VALUE),"Assets/2D/laser_blue.png",sf::Vector2f (4,16),e_damage, fx);
					Weapon* weapon = new Weapon(ammo);
					weapon->display_name = "laser_blue";
					weapon->fire_direction = Vector2i(0,-1);
					weapon->textureName = "Assets/2D/laser_red.png";
					weapon->size = sf::Vector2f(64,64);
					weapon->frameNumber = 1;
					weapon->rate_of_fire = WEAPON_MIN_RATE_OF_FIRE_VALUE;

					bot->weapon = weapon;
					equipment->bot = bot;

					//Scaling with value


					//Adding properties
					for (int p=0; p<number_of_equipment_properties; p++)
					{
						int chosen_property = GetChosenProperty(&properties_roll_table,properties_to_choose_from,p);
						equipment->AddModuleProperty(chosen_property, e_value, LootTable_BeastScale[e_class]);
					}

					this->setEquipmentLoot(equipment);
					break;
				}

				case (int)EquipmentType::NBVAL_EQUIPMENT://weapon drop
				{
					// Initialisation
					FX* fx = new FX(sf::Vector2f (0,0), sf::Vector2f (0,0), "Assets/2D/FX_explosion_S_blue.png", sf::Vector2f (320,236), 2, sf::seconds(0.4f));
					float e_damage = WEAPON_MIN_DAMAGE_VALUE;
					float log_multiplier = WEAPON_DAMAGE_LN_MULTIPLIER_BASE * (log(e_value * WEAPON_DAMAGE_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
						e_damage *= log_multiplier;
					e_damage = floor(e_damage);
					Ammo* ammo = new Ammo(this->getPosition(),sf::Vector2f (0,WEAPON_MIN_VSPEED_VALUE),"Assets/2D/laser_blue.png",sf::Vector2f (4,16),e_damage, fx);
					Weapon* weapon = new Weapon(ammo);
					weapon->display_name = "laser_blue";
					weapon->fire_direction = Vector2i(0,-1);
					weapon->textureName = "Assets/2D/laser_red.png";
					weapon->size = sf::Vector2f(64,64);
					weapon->frameNumber = 1;
					weapon->rate_of_fire = WEAPON_MIN_RATE_OF_FIRE_VALUE;

					//Adding properties
					for (int p=0; p<number_of_equipment_properties; p++)
					{
						int chosen_property = GetChosenProperty(&properties_roll_table,properties_to_choose_from,p);
						weapon->AddWeaponProperty(chosen_property, e_value, LootTable_BeastScale[e_class]);
					}

					this->setWeaponLoot(weapon);
					break;
				}

				default:
				{
					printf("DEBUG: error: the loot type chosen for this drop does not exist.\n <!> Check if the equipment_type_roll values match with the existing equipment types.\n");
					break;
				}
			}
		}

		else
		{
			int money = RandomizeIntBetweenRatios(e_value, LootTable_BeastScale[e_class]);
			this->addMoney(money);//looting money
		}
	}
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
			weapon->display_name = (*it)[WeaponData::WEAPON_NAME];
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
						weapon->shot_mode = ShotMode::AlternateShotMode;
					if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("ascending") == 0)
						weapon->shot_mode = ShotMode::AscendingShotMode;
					if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("descending") == 0)
						weapon->shot_mode = ShotMode::DescendingShotMode;
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
					weapon->target_seaking = TargetSeaking::SEMI_SEAKING;
				else if ((*it)[WeaponData::WEAPON_TARGET_SEAKING].compare("seaking") == 0)
					weapon->target_seaking = TargetSeaking::SEAKING;
				else if ((*it)[WeaponData::WEAPON_TARGET_SEAKING].compare("super_seaking") == 0)
					weapon->target_seaking = TargetSeaking::SUPER_SEAKING;
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