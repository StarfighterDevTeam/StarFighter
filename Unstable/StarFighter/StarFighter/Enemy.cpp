#include "Enemy.h"

extern Game* CurrentGame;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	angspeed = 0;
	radius = 0;
	FX_death = m_FX_death;
	hasWeapon = false;
	enemy_class = EnemyClass::ENEMYPOOL_ZETA;
	hasPhases = false;
}

void Enemy::update(sf::Time deltaTime)
{
	//sheld regen if not maximum
	if (shield < shield_max)
	{
		shield += shield_regen;
		//canceling over-regen
		if (shield > shield_max)
		{
			shield = shield_max;
		}
	}

	//automatic fire
	if(isOnScene & hasWeapon)
	{
		for (std::list<Weapon*>::iterator it = (this->weapons_list.begin()); it != (this->weapons_list.end()); it++)
		{
			(*it)->weaponOffset = sf::Vector2f((this->m_size.y / 2) * (*it)->getFireDirection_for_Direction((*CurrentGame).direction).x,
				(this->m_size.y / 2) * (*it)->getFireDirection_for_Direction((*CurrentGame).direction).y);

			(*it)->setPosition(this->getPosition().x + (*it)->weaponOffset.x, this->getPosition().y + (*it)->weaponOffset.y);

			(*it)->Fire(IndependantType::EnemyFire);
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
	Independant::update(deltaTime);

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

	Enemy* enemy = new Enemy(this->getPosition(), this->speed, this->textureName, this->m_size, this->FX_death);

	((Independant*)enemy)->armor = this->getIndependantArmor();
	((Independant*)enemy)->armor_max = this->getIndependantArmorMax();
	((Independant*)enemy)->shield = this->getIndependantShield();
	((Independant*)enemy)->shield_max = this->getIndependantShieldMax();
	((Independant*)enemy)->shield_regen = this->getIndependantShieldRegen();
	((Independant*)enemy)->damage = this->getIndependantDamage();
	enemy->hasWeapon = this->hasWeapon;

	for (std::list<Weapon*>::iterator it = (this->weapons_list.begin()); it != (this->weapons_list.end()); it++)
	{
		enemy->weapons_list.push_back((*it)->Clone());
	}	
	
	((Independant*)enemy)->addMoney(this->getMoney());
	enemy->hasEquipmentLoot = this->hasEquipmentLoot;
	enemy->equipment_loot = this->getEquipmentLoot();
	enemy->hasWeaponLoot = this->hasWeaponLoot;
	enemy->weapon_loot = this->getWeaponLoot();
	enemy->display_name = this->display_name;
	enemy->enemy_class = this->enemy_class;

	enemy->Pattern = this->Pattern;
	enemy->angspeed = this->angspeed;
	enemy->radius = this->radius;

	enemy->hasPhases = this->hasPhases;
	enemy->currentPhase = this->currentPhase;
	enemy->enemyClock.restart();

	return enemy;
}

bool Enemy::CheckCondition()
{
	for (std::list<ConditionTransition*>::iterator it = (this->currentPhase->transitions_list.begin()); it != (this->currentPhase->transitions_list.end()); it++)
	{
		switch ((*it)->condition)
		{
		case ConditionType::VerticalPosition: {
												  FloatCompare result = this->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f((*it)->value / SCENE_SIZE_Y*SCENE_SIZE_X, (*it)->value));
												  if (result == (*it)->op)
												  {
														this->setPhase((*it)->nextPhase_name);
														return true;
												  }
													  
												  break;


		}
		
		case ConditionType::phaseClock:{
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
		case ConditionType::enemyClock:{
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
		case ConditionType::LifePourcentage:{
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
												//printf("vie: %d. vie max: %d. shield: %d\n", this->getIndependantArmor(), this->getIndependantArmorMax(), this->getIndependantShield());
												break;
		}
		case ConditionType::ShieldPourcentage:{
												if ((100.0f * this->getIndependantShield() / this->getIndependantShieldMax() >= (*it)->value) && (((*it)->op == FloatCompare::GREATHER_THAN) || ((*it)->op == FloatCompare::EQUAL_TO)))
												{
													this->setPhase((*it)->nextPhase_name);
													return true;
												}
												else if ((100.0f * this->getIndependantShield() / this->getIndependantShieldMax() <= (*it)->value) && (((*it)->op == FloatCompare::LESSER_THAN) || ((*it)->op == FloatCompare::EQUAL_TO)))
												{
													this->setPhase((*it)->nextPhase_name);
													return true;
												}

												break;
		}
		}
	}
	return false;
}

void Enemy::setPhase(string phase_name)
{
	Phase* phase = this->LoadPhase(phase_name);
	this->currentPhase = phase;
	this->phaseClock.restart();

	this->speed.y = phase->vspeed;

	//clearing old weapons and setting new ones
	this->weapons_list.clear();
	for (std::list<Weapon*>::iterator it = (phase->weapons_list.begin()); it != (phase->weapons_list.end()); it++)
	{
		this->weapons_list.push_back((*it)->Clone());
	}

	//setting new move pattern
	vector<float>* v = new vector<float>;
	v->push_back(phase->radius); // rayon
	v->push_back(1);  // clockwise (>)
	this->Pattern.SetPattern(phase->pattern, phase->angspeed, v); //vitesse angulaire (degres/s)
}



Phase* Enemy::LoadPhase(string name)
{
	list<vector<string>> phaseConfig = *(FileLoaderUtils::FileLoader(PHASES_FILE));

	for (std::list<vector<string>>::iterator it = (phaseConfig).begin(); it != (phaseConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Phase* phase = new Phase();

			phase->display_name = (*it)[EnemyPhaseData::PHASE_NAME];
			phase->vspeed = stoi((*it)[EnemyPhaseData::PHASE_VSPEED]);

			//loading weapons and ammos
			if ((*it)[EnemyPhaseData::PHASE_WEAPON].compare("0") != 0)
			{
				phase->weapons_list.push_back(Enemy::LoadWeapon((*it)[EnemyPhaseData::PHASE_WEAPON], 1, Enemy::LoadAmmo((*it)[EnemyPhaseData::PHASE_AMMO])));
			}
			if ((*it)[EnemyPhaseData::PHASE_WEAPON_2].compare("0") != 0)
			{
				phase->weapons_list.push_back(Enemy::LoadWeapon((*it)[EnemyPhaseData::PHASE_WEAPON_2], 1, Enemy::LoadAmmo((*it)[EnemyPhaseData::PHASE_AMMO_2])));
			}
			if ((*it)[EnemyPhaseData::PHASE_WEAPON_3].compare("0") != 0)
			{
				phase->weapons_list.push_back(Enemy::LoadWeapon((*it)[EnemyPhaseData::PHASE_WEAPON_3], 1, Enemy::LoadAmmo((*it)[EnemyPhaseData::PHASE_AMMO_3])));
			}

			//loading movement patterns
			PatternType pattern_type = PatternType::NoMovePattern;
			if ((*it)[EnemyPhaseData::PHASE_PATTERN].compare("circle") == 0)
			{
				pattern_type = PatternType::Circle_;
				phase->angspeed = stoi((*it)[EnemyPhaseData::PHASE_ANGSPEED]);
				phase->radius = stoi((*it)[EnemyPhaseData::PHASE_RADIUS]);
			}

			if ((*it)[EnemyPhaseData::PHASE_PATTERN].compare("oscillator") == 0)
			{
				pattern_type = PatternType::Oscillator;
				phase->angspeed = stoi((*it)[EnemyPhaseData::PHASE_ANGSPEED]);
				phase->radius = stoi((*it)[EnemyPhaseData::PHASE_RADIUS]);
			}

			phase->pattern = pattern_type;

			//loading transition to next phase
			if ((*it)[EnemyPhaseData::PHASE_TRANSITION].compare("0") != 0)
			{
				phase->hasTransition = true;

				//loading condition type
				ConditionType cond = ConditionType::NoCondition;
				if ((*it)[EnemyPhaseData::PHASE_CONDITION].compare("positionV") == 0)
				{
					cond = ConditionType::VerticalPosition;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION].compare("phaseClock") == 0)
				{
					cond = ConditionType::phaseClock;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION].compare("enemyClock") == 0)
				{
					cond = ConditionType::enemyClock;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION].compare("life") == 0)
				{
					cond = ConditionType::LifePourcentage;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION].compare("shield") == 0)
				{
					cond = ConditionType::ShieldPourcentage;
				}

				//loading operator type
				FloatCompare op = FloatCompare::ERROR_COMPARE;
				if ((*it)[EnemyPhaseData::PHASE_OPERATOR].compare("greater") == 0)
				{
					op = FloatCompare::GREATHER_THAN;
				}
				else if ((*it)[EnemyPhaseData::PHASE_OPERATOR].compare("lesser") == 0)
				{
					op = FloatCompare::LESSER_THAN;
				}
				else if ((*it)[EnemyPhaseData::PHASE_OPERATOR].compare("equal") == 0)
				{
					op = FloatCompare::EQUAL_TO;
				}
				else
				{
					LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("ERROR: Invalid operator found when loading condition of transition 1 of enemy phase named '%s'. Please check config file", name));
				}

				//and finally wrapping up all of this data in our list of transition conditions
				ConditionTransition* condition = new ConditionTransition(cond, op, stoi((*it)[EnemyPhaseData::PHASE_VALUE]), (*it)[EnemyPhaseData::PHASE_TRANSITION]);
				phase->transitions_list.push_back(condition);
			}

			//idem for transition 2
			if ((*it)[EnemyPhaseData::PHASE_TRANSITION_2].compare("0") != 0)
			{
				phase->hasTransition = true;

				//loading condition type
				ConditionType cond = ConditionType::NoCondition;
				if ((*it)[EnemyPhaseData::PHASE_CONDITION_2].compare("positionV") == 0)
				{
					cond = ConditionType::VerticalPosition;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION_2].compare("phaseClock") == 0)
				{
					cond = ConditionType::phaseClock;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION_2].compare("enemyClock") == 0)
				{
					cond = ConditionType::enemyClock;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION_2].compare("life") == 0)
				{
					cond = ConditionType::LifePourcentage;
				}
				else if ((*it)[EnemyPhaseData::PHASE_CONDITION_2].compare("shield") == 0)
				{
					cond = ConditionType::ShieldPourcentage;
				}

				//loading operator type
				FloatCompare op = FloatCompare::ERROR_COMPARE;
				if ((*it)[EnemyPhaseData::PHASE_OPERATOR_2].compare("greater") == 0)
				{
					op = FloatCompare::GREATHER_THAN;
				}
				else if ((*it)[EnemyPhaseData::PHASE_OPERATOR_2].compare("lesser") == 0)
				{
					op = FloatCompare::LESSER_THAN;
				}
				else if ((*it)[EnemyPhaseData::PHASE_OPERATOR_2].compare("equal") == 0)
				{
					op = FloatCompare::EQUAL_TO;
				}
				else
				{
					LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("ERROR: Invalid operator found when loading condition of transition of enemy phase named '%s'. Please check config file", name));
				}

				//and finally wrapping up all of this data in our list of transition conditions
				ConditionTransition* condition = new ConditionTransition(cond, op, stoi((*it)[EnemyPhaseData::PHASE_VALUE_2]), (*it)[EnemyPhaseData::PHASE_TRANSITION_2]);
				phase->transitions_list.push_back(condition);
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
}

void Enemy::GenerateLoot()
{
	sf::Vector2f speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, LOOT_SPEED_Y);

	if (this->hasWeaponLoot)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, this->getWeaponLoot()->textureName, sf::Vector2f(this->getWeaponLoot()->size.x, this->getWeaponLoot()->size.y), this->getWeaponLoot()->display_name);
		new_loot->get_weapon_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
	}

	else if (this->hasEquipmentLoot)
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

	Loot* loot = new Loot(this->getPosition(),sf::Vector2f(0, LOOT_SPEED_Y), LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Empty drop");

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
			case (int)EquipmentType::Airbrake:
				{
					//Initialisation
					Equipment* equipment = new Equipment();
					float e_decceleration = EQUIPMENT_MIN_DECCELLERATION_VALUE;
					float log_multiplier = EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
						e_decceleration *= log_multiplier;

					e_decceleration = floor(e_decceleration);

					equipment->Init((int)EquipmentType::Airbrake, sf::Vector2f(0,0),e_decceleration,sf::Vector2f(0,0),0,0,0,0, AIRBRAKE_FILENAME,sf::Vector2f(64,64),1,"Airbrake");

					//Adding properties
					for (int p=0; p<number_of_equipment_properties; p++)
					{
						int chosen_property = GetChosenProperty(&properties_roll_table,properties_to_choose_from,p);
						equipment->AddAirbrakeProperty(chosen_property, e_value, LootTable_BeastScale[e_class]);
					}

					this->setEquipmentLoot(equipment);
					break;
				}

			case (int)EquipmentType::Engine:
				{
					//Initialisation
					Equipment* equipment = new Equipment();
					float e_maxspeed_x = EQUIPMENT_MIN_MAXSPEED_VALUE;
					float e_maxspeed_y = EQUIPMENT_MIN_MAXSPEED_VALUE;
					float log_multiplier = EQUIPMENT_MAXSPEED_LN_MULTIPLIER_BASE * (log(e_value * EQUIPMENT_MAXSPEED_LN_MULTIPLIER_X));
					if (log_multiplier > 1)
					{
						e_maxspeed_x *= log_multiplier;
						e_maxspeed_y *= log_multiplier;
					}
					e_maxspeed_x = floor(e_maxspeed_x);
					e_maxspeed_y = floor(e_maxspeed_y);
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

					equipment->Init((int)EquipmentType::Engine, sf::Vector2f(e_maxspeed_x,e_maxspeed_y),0,
						sf::Vector2f(e_acceleration_x,e_acceleration_y),0,0,0,0, THRUSTER_FILENAME,sf::Vector2f(64,64),1,"Engine");

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
					equipment->Init((int)EquipmentType::Armor, sf::Vector2f(0,0),0,sf::Vector2f(0,0),e_armor,0,0,0, ARMOR_FILENAME,sf::Vector2f(64,64),1,"Armor");

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

					equipment->Init((int)EquipmentType::Shield, sf::Vector2f(0,0),0,sf::Vector2f(0,0),0,
						e_shield,e_shield_regen,0, SHIELD_FILENAME,sf::Vector2f(64,64),1,"Shield");

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
					equipment->Init((int)EquipmentType::Module, sf::Vector2f(0,0),0,sf::Vector2f(0,0),0,0,0,0, MODULE_FILENAME,sf::Vector2f(64,64),1,"Module");

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
					bot->hasWeapon = true;
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

	list<vector<string>> weaponConfig = *(FileLoaderUtils::FileLoader(WEAPON_FILE));

	for (std::list<vector<string>>::iterator it = (weaponConfig).begin(); it != (weaponConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Weapon* weapon = new Weapon(ammo);
			weapon->display_name = (*it)[WeaponData::WEAPON_NAME];
			weapon->fire_direction = Vector2i(0, fire_direction);
			weapon->rate_of_fire = atof((*it)[WeaponData::WEAPON_RATE_OF_FIRE].c_str());
			weapon->multishot = stoi((*it)[WeaponData::WEAPON_MULTISHOT]);
			weapon->xspread = stoi((*it)[WeaponData::WEAPON_XSPREAD]);
			weapon->shot_mode = ShotMode::NoShotMode;
			if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("0") != 0)
			{
				if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("alternate") == 0)
					weapon->shot_mode = ShotMode::AlternateShotMode;
				if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("ascending") == 0)
					weapon->shot_mode = ShotMode::AscendingShotMode;
				if ((*it)[WeaponData::WEAPON_ALTERNATE].compare("descending") == 0)
					weapon->shot_mode = ShotMode::DescendingShotMode;
			}
			weapon->dispersion = stoi((*it)[WeaponData::WEAPON_DISPERSION]);
			weapon->rafale = stoi((*it)[WeaponData::WEAPON_RAFALE]);
			if (weapon->rafale != 0)
				weapon->rafale_cooldown = atof((*it)[WeaponData::WEAPON_RAFALE_COOLDOWN].c_str());

			weapon->textureName = (*it)[WeaponData::WEAPON_IMAGE_NAME];
			weapon->size = sf::Vector2f(stoi((*it)[WeaponData::WEAPON_WIDTH]), stoi((*it)[WeaponData::WEAPON_HEIGHT]));
			weapon->frameNumber = stoi((*it)[WeaponData::WEAPON_FRAMES]);
			weapon->target_seaking = (bool)(stoi((*it)[WeaponData::WEAPON_TARGET_SEAKING]));

			return weapon;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Weapon '%s'. Please check the config file", name));

}

Ammo* Enemy::LoadAmmo(string name)
{
	list<vector<string>> ammoConfig = *(FileLoaderUtils::FileLoader(AMMO_FILE));

	for (std::list<vector<string>>::iterator it = (ammoConfig).begin(); it != (ammoConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Ammo* new_ammo = new Ammo(Vector2f(0, 0), Vector2f(0, stoi((*it)[AmmoData::AMMO_SPEED])), (*it)[AmmoData::AMMO_IMAGE_NAME],
				Vector2f(stoi((*it)[AmmoData::AMMO_WIDTH]), stoi((*it)[AmmoData::AMMO_HEIGHT])), stoi((*it)[AmmoData::AMMO_DAMAGE]), LoadFX((*it)[AmmoData::AMMO_FX]));
			new_ammo->display_name = (*it)[AmmoData::AMMO_NAME];
			new_ammo->radius = stoi((*it)[AmmoData::AMMO_RADIUS]);
			new_ammo->angspeed = stoi((*it)[AmmoData::AMMO_ANGSPEED]);

			//Loading movement pattern
			vector<float>* v = new vector<float>;
			v->push_back(new_ammo->radius); // rayon
			v->push_back(1);  // clockwise (>)

			PatternType pattern_type = PatternType::NoMovePattern;
			if ((*it)[AmmoData::AMMO_PATTERN].compare("circle") == 0)
				pattern_type = PatternType::Circle_;
			if ((*it)[AmmoData::AMMO_PATTERN].compare("oscillator") == 0)
				pattern_type = PatternType::Oscillator;

			new_ammo->Pattern.SetPattern(pattern_type, new_ammo->angspeed, v); //vitesse angulaire (degres/s)

			return new_ammo;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file", name));
}

FX* Enemy::LoadFX(string name)
{
	list<vector<string>>FXConfig = *(FileLoaderUtils::FileLoader(FX_FILE));

	for (std::list<vector<string>>::iterator it = (FXConfig).begin(); it != (FXConfig).end(); it++)
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