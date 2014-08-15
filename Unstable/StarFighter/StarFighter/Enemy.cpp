#include "Enemy.h"

extern Game* CurrentGame;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	movepattern_type = 0;//type de pattern hardcodé pour l'instant
	FX_death = m_FX_death;
	hasWeapon = false;
	enemy_class = EnemyClass::ENEMYPOOL_ZETA;
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
		weapon->weaponOffset = sf::Vector2f((this->m_size.x / 2) + (weapon->ammunition->m_size.x / 2), (this->m_size.y / 2) - (weapon->ammunition->m_size.y / 2));
		weapon->setPosition(this->getPosition().x, this->getPosition().y);

		weapon->Fire(IndependantType::EnemyFire);

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
}

Enemy* Enemy::Clone()
{
	sf::Vector2f s = this->speed;

	Enemy* enemy = new Enemy(this->getPosition(), this->speed, this->textureName, this->m_size, this->FX_death);

	((Independant*)enemy)->armor = this->getIndependantArmor();
	((Independant*)enemy)->shield = this->getIndependantShield();
	((Independant*)enemy)->shield_regen = this->getIndependantShieldRegen();
	((Independant*)enemy)->damage = this->getIndependantDamage();
	enemy->hasWeapon = this->hasWeapon;
	if (enemy->hasWeapon)
		enemy->weapon = this->weapon->Clone();

	((Independant*)enemy)->addMoney(this->getMoney());
	enemy->hasEquipmentLoot = this->hasEquipmentLoot;
	enemy->equipment_loot = this->getEquipmentLoot();
	enemy->hasWeaponLoot = this->hasWeaponLoot;
	enemy->weapon_loot = this->getWeaponLoot();
	enemy->display_name = this->display_name;
	enemy->enemy_class = this->enemy_class;

	return enemy;
}

void Enemy::Death()
{
	FX* myFX = this->FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	(*CurrentGame).addToScene(myFX,LayerType::ExplosionLayer, IndependantType::Neutral);
}

void Enemy::GenerateLoot()
{
	sf::Vector2f speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, LOOT_SPEED_Y, false);

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

					equipment->Init((int)EquipmentType::Airbrake, sf::Vector2f(0,0),e_decceleration,sf::Vector2f(0,0),0,0,0,AIRBRAKE_FILENAME,sf::Vector2f(64,64),1,"Airbrake");

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
						sf::Vector2f(e_acceleration_x,e_acceleration_y),0,0,0,THRUSTER_FILENAME,sf::Vector2f(64,64),1,"Engine");

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
					equipment->Init((int)EquipmentType::Armor, sf::Vector2f(0,0),0,sf::Vector2f(0,0),e_armor,0,0,ARMOR_FILENAME,sf::Vector2f(64,64),1,"Armor");

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
						e_shield,e_shield_regen,SHIELD_FILENAME,sf::Vector2f(64,64),1,"Shield");

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
					equipment->Init((int)EquipmentType::Module, sf::Vector2f(0,0),0,sf::Vector2f(0,0),0,0,0,MODULE_FILENAME,sf::Vector2f(64,64),1,"Module");

					Bot* bot = new Bot(sf::Vector2f (0,0), sf::Vector2f (0,0), "Assets/2D/enemyY.png", sf::Vector2f (35,35));
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