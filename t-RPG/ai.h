#pragma once
#include "classes.h"
#include "functions.h"
#include "actions.h"


std::string General(Person* p, std::vector<std::vector<Tile>>& map, Person* obs) {
	//Eat, Drink
	static int fruit_buys = 0;
	static int num_buys = 0;
	int cooked_food = 0;

	//pick up gold on the ground
	for (int i = 0; i < map[p->y][p->x].items.size(); i++) {
		if (map[p->y][p->x].items[i].i == GOLD) {
			return Get(p, map[p->y][p->x].items, i, obs);
		}
	}

	for (int k = 0; k < INVENTORY_SIZE; k++) { //Check an ape's inventory for grub
		if (p->inventory[k].name != "null" && !items[p->inventory[k].i].body_part) {
			if (items[p->inventory[k].i].hunger > 0) {
				cooked_food += items[p->inventory[k].i].hunger;
				if (p->state.hunger > items[p->inventory[k].i].hunger) {
					static int cooked_eaten = 0;
					if (items[p->inventory[k].i].cook == -1 || (p->state.hunger >= 100 && p->hp.val < p->hp.max / 2)) {
						return Eat(p, k, obs);
					}
				}
			}
			else if (items[p->inventory[k].i].thirst > 0 && p->state.thirst > items[p->inventory[k].i].thirst) {
				return Eat(p, k, obs);
			}
		}
	}
	//thirsty, but no drink.
	if (p->state.thirst > 15) {
		//Look for a river
		for (int i = HERE; i <= SOUTH; i++) {
			if (map[p->y][p->x].water[i] == 'r') {
				p->state.thirst -= 15;
				p->state.misery += 4;
				return relationName(p, obs) + " drinks *BLUE*river water*BLUE*.";
			}
		}
		if (p->state.thirst > 40) {
			Spot min = findSpot(map, p, Spot(p->x, p->y), 0, 'r');
			if (min.x != -1) { //Move to water
				return setPath(map, p, min, obs);
			}
		}
	}
	if (p->job != "chef" && p->state.hunger > cooked_food && p->sentient) {
		//Buy food
		for (Person* ape : map[p->y][p->x].people) {
			if (ape->sentient && ape != p) {
				for (int j = 0; j < INVENTORY_SIZE; j++) {
					if (ape->inventory[j].name != "null") {
						if (ape->inventory[j].sale && p->gold >= ape->inventory[j].value) {
							if (items[ape->inventory[j].i].type == FOOD || ((p->job == "chef" || ape->state.hunger > 90) && items[ape->inventory[j].i].type == RAW_FOOD)) {
								for (int k = 0; k < INVENTORY_SIZE; k++) {
									if (p->inventory[k].name == "null") {
										p->gold -= ape->inventory[j].value;
										ape->gold += ape->inventory[j].value;
										p->inventory[k] = ape->inventory[j];
										p->inventory[k].sale = false;
										if (items[ape->inventory[j].i].type == FOOD) {
											if (low(ape->inventory[j].name) == "fruit") {
												fruit_buys++;
											}
											else {
												num_buys++;
											}
											//std::cout << "Food Bought: " + to_str(num_buys) << std::endl;
											//std::cout << "Fruit Bought: " + to_str(fruit_buys) << std::endl;
										}
										ape->inventory[j].name = "null";
										return "trade complete";
									}
								}
							}
						}
					}
				}
			}
		}
		if (p->state.hunger >= 45 && (p->job != "fisher")) {
			Spot min = Spot(-1, -1);
			if (p->gold >= 15) {
				if (p->state.hunger < 95) {
					min = findSale(map, p, "", { FOOD });
				}
				else {
					min = findSale(map, p, "", { FOOD, RAW_FOOD });
				}
				if (min.x != -1) { //Move to someone selling food
					return setPath(map, p, min, obs);
				}
			}
			if (min.x == -1 && p->state.hunger >= 75) {
				Spot min = findItem(map, p, "", { RAW_FOOD, FOOD }, 24);
				if (min.x != -1) { //Look for food on the ground
					if (min.x == p->x && min.y == p->y) {
						for (int i = 0; i < map[p->y][p->x].items.size(); i++) {
							if (items[map[p->y][p->x].items[i].i].type == FOOD || items[map[p->y][p->x].items[i].i].type == RAW_FOOD) {
								return Get(p, map[p->y][p->x].items, i, obs);
							}
						}
					}
					return setPath(map, p, min, obs);
				}
			}
		}
	}
	return "";
}

//This function takes a reference to a 2d array of tiles - the map, a string saying what job the person has, an two Person pointers, the NPC and the observor.
std::string AIController(Person* p, std::vector<std::vector<Tile>>& map, Person* obs, int& cooked) {
	cooked = 0;
	int time = Time();
	bool returnHome = time > 12; //Make sure it's daytime
	std::string s = "";
	int freeIndex = -1;
	int freeSlots = 0;
	bool sale = false;
	bool observor = p != obs;
	std::string job = p->job;


	if (p->move != HERE) {
		s = moveDirection(map, p, obs);
	}
	if (p->path.size() > 0) {
		if (rand() % 100 == 1) {
			p->path = {};
		}
		else {
			s = setMovement(p, p->path.front(), obs);
			p->path.pop_front();
			return s;
		}
	}
	if (s != "") {
		return s;
	}
	if (p->job != "beggar") {
		s = General(p, map, obs);
		if (s != "") {
			return s;
		}
	}
	
	if (p->eats && p->sentient) {
		//Consider giving alms:
		if (15 + rand() % 75 < p->gold) {
			for (Person* ape : map[p->y][p->x].people) {
				if (ape->job == "beggar") {
					p->gold--;
					ape->gold++;
					break;
				}
			}
		}
		if (p->state.hunger >= 100 || (p->gold < 5 && p->state.hunger > 45)) {
			p->job = "gatherer";
			if (low(p->weapon.name) == "fishing pole") {
				p->job = "fisher";
			}
			else {
				for (int i = 0; i < INVENTORY_SIZE; i++) {
					if ((p->inventory[i].name) == "fishing pole") {
						Item temp = p->weapon;
						p->weapon = p->inventory[i];
						p->inventory[i] = temp;
					}
				}
			}
		}
	}
	int value = p->gold;
	if (p->sentient) {
		for (int k = 0; k < INVENTORY_SIZE; k++) { //Make sure ape has room in their inventory
			if (low(p->inventory[k].name) == "animal hide" || items[p->inventory[k].i].type == CLOTHING) {
				p->inventory[k].name = "null";
			}
			else if (p->inventory[k].name == "null") {
				freeSlots++;
				if (freeIndex == -1) {
					freeIndex = k;
				}
			}
			else { value += p->inventory[k].value; }
		}

		if (p->state.hunger > 10 && value < 15 && low(p->weapon.name) != "fishing pole") {
			p->job = "beggar";
		}
	}

	if (job == "fisher") {
		bool spare_pole = false;
		for (int i = 0; i < INVENTORY_SIZE; i++) {
			//mark goods for sale:
			if (p->inventory[i].name != "null") {
				if (items[p->inventory[i].i].type == RAW_FOOD) {
					p->inventory[i].sale = true;
					sale = true;
				}
				else if (low(p->inventory[i].name) == "stick") {
					p->inventory[i].sale = true;
				}
				if (low(p->inventory[i].name) == "fishing pole") {
					if (low(p->weapon.name) != "fishing pole") {
						Item holder = p->weapon;
						p->weapon = p->inventory[i];
						p->inventory[i] = holder;
					}
					else {
						spare_pole = true;
					}
				}
			}
		}
		if (freeSlots > 1 && low(p->weapon.name) == "fishing pole") { //Make sure NPC has a fishing pole
			Spot min = findSpot(map, p, Spot(p->x, p->y), 0, 'a');
			if (min.y == p->y && min.x == p->x) { //Fish sees if the ape catches a fish and returns a string saying what happened
				s = Fish(p, map[p->y][p->x], obs);
			}
			else if (min.x != -1) { //Move to water
				return setPath(map, p, min, obs);
			}
		}
		else {
			if (low(p->weapon.name) != "fishing pole" || !spare_pole && p->gold >= 35) { //acquire fishing pole
				for (Person* ape : map[p->y][p->x].people) {
					if (ape != p) {
						for (int j = 0; j < INVENTORY_SIZE; j++) {
							if (low(ape->inventory[j].name) == "fishing pole") {
								if (ape->inventory[j].sale && p->gold >= ape->inventory[j].value) {
									for (int k = 0; k < INVENTORY_SIZE; k++) {
										if (p->inventory[k].name == "null") {
											p->gold -= ape->inventory[j].value;
											ape->gold += ape->inventory[j].value;
											p->inventory[k] = ape->inventory[j];
											ape->inventory[j].name = "null";
											p->inventory[k].sale = false;
											return "trade complete";
										}
									}
								}
							}
						}
					}
				}
				if (p->weapon.name == "null" && p->gold >= 35) {
					Spot min = findSale(map, p, "fishing pole"); //move to nearby town
					if (min.x != -1) {
						return setPath(map, p, min, obs);
					}
				}
				else {
					p->job = "gatherer";
				}
			}
			if (freeSlots <= 4 && map[p->y][p->x].type == 't') { //at home, sell your stuff
				if (sale && rand() % 4 == 0) {
					s = relationName(p, obs) + ": 'Fish for sale!'";
				}
			}
			else if (freeSlots <= 2 && rand() % 4 == 0) {
				Spot min = findSpot(map, p, Spot(p->x, p->y), 't', 0, true); //move to nearby town
				if (min.x != -1) {
					return setPath(map, p, min, obs);
				}
			}
		}
	}
	else if (job == "hunter") {
		for (int i = 0; i < INVENTORY_SIZE; i++) { // Sell raw food
			//mark goods for sale:
			if (p->inventory[i].name != "null") {
				if (items[p->inventory[i].i].type == RAW_FOOD) {
					p->inventory[i].sale = true;
					sale = true;
				}
			}
		}
		for (int i = 0; i < map[p->y][p->x].items.size(); i++) { // Get raw meat off the ground
			if (items[map[p->y][p->x].items[i].i].type == RAW_FOOD) {
				Get(p, map[p->y][p->x].items, i, obs);
			}
		}
		if (s == "") {
			if (p->hp.val == p->hp.max) {
				for (Person* ape : map[p->y][p->x].people) {
					if (ape->job == "herbivore") {
						if (!ape->combat) {
							//Attack a deer
							startFight(p, ape, map[p->y][p->x], KILL);
							break;
						}
					}
				}
				if (!(p->combat)) {
					Spot min = findJob(map, p, { "herbivore" });
					int d = dist(Spot(p->x, p->y), min);
					if (min.x != -1 && d > 0) {
						return setPath(map, p, min, obs);
					}
				}
			}
		}
		if (sale) {
			Spot min = findJob(map, p, { "chef" });
			if (min.x != -1) {
				return setPath(map, p, min, obs);
			}
		}
	}
	else if (job == "herbivore") {
		for (int i = 0; i < map[p->y][p->x].items.size(); i++) { // Get Fruit
			if (items[map[p->y][p->x].items[i].i].type == FOOD) {
				return Get(p, map[p->y][p->x].items, i, obs);
			}
		}
		for (int a = -1; a < 2; a++) {
			for (int b = -1; b < 2; b++) {
				if (a * b == 0) {
					for (Person* ape : map[p->y + a][p->x + b].people) {
						if (a != b) {
							if (ape->job == "hunter") {
								//Flee in opposite direction of hunter.
								p->move = spotMovement(Spot(ape->x, ape->y), Spot(p->x, p->y));
								if (rand() % 5 == 0) {
									p->move = (Direction)(1 + rand() % 4);
								}
								return "";
							}
						}
						else if (ape->job == "herbivore") {
							if (rand() % 1000 == 0 && map[p->y + a][p->x + b].people.size() < 10) {
								map[p->y][p->x].people.push_back(new Person());
								map[p->y][p->x].people[map[p->y][p->x].people.size() - 1]->home = Spot(p->x, p->y);
								map[p->y][p->x].people[map[p->y][p->x].people.size() - 1]->x = p->x;
								map[p->y][p->x].people[map[p->y][p->x].people.size() - 1]->y = p->y;
								createDeer(map[p->y][p->x].people.back());

								return "Unto the world a child is born. ";
							}
						}
					}
				}
			}
		}
		Spot min = findItem(map, p, "", { FOOD });
		if (min.x != -1) {
			return setPath(map, p, min, obs);
		}
	}
	else if (job == "gatherer") {
		for (int i = 0; i < INVENTORY_SIZE; i++) {
			if (p->inventory[i].name != "null" && (items[p->inventory[i].i].type != FOOD && (items[p->inventory[i].i].hunger == 0 || p->state.hunger < 85))){
				p->inventory[i].sale = true;
				sale = true;
			}
			if (low(p->inventory[i].name) == "fishing pole") {
				if (low(p->weapon.name) != "fishing pole") {
					Item holder = p->weapon;
					p->weapon = p->inventory[i];
					p->inventory[i] = holder;
					p->job = "fisher";
				}
			}
		}
		if (freeSlots > 0) {
			for (int i = 0; i < map[p->y][p->x].items.size(); i++) {
				if (freeSlots >= 3 && map[p->y][p->x].items[i].value >= 3) {
					return Get(p, map[p->y][p->x].items, i, obs);
					freeSlots--;
				}
			}
		}
		if (p->gold > 50) {
			int ran = rand() % 4;
			switch (ran) {
			case 0: p->job = "artisan"; break;
			case 1: case 2: p->job = "fisher"; break;
			case 3: p->job = "chef"; break;
			}
		}
		else if (s == "") {
			if (p->state.hunger < 80 && freeSlots < 4) { // move to town to sell things
				Spot min = findJob(map, p, { "artisan"}, 9);
				int d = dist(Spot(p->x, p->y), min);
				if (min.x != -1 && d > 0) {
					return setPath(map, p, min, obs);
				}
			}
			else {//Move to forest
				Spot min;
				int ran = rand() % 2;
				if (ran == 0) {
					min = findItem(map, p, "", { FOOD, RAW_FOOD }, 12);
				}
				if (ran == 1 || min.x == -1) {
					min = findItem(map, p, "stick");
				}
				
				if (min.x != -1) {
					return setPath(map, p, min, obs);
				}
			}
		}
	}
	else if (job == "artisan") {
		//make fishing poles
		int stick_indexes[3] = { -1, -1, -1 };
		int index = 0;
		bool found_stick = false;
		for (int i = 0; i < INVENTORY_SIZE; i++) {
			if (p->inventory[i].name != "null" && low(p->inventory[i].name) == "stick") {
				stick_indexes[index++] = i;
				if (index == 3) {
					break;
				}
			}
		}
		if (index == 3) { //craft fishing pole
			static int fishing_poles = 0;
			//std::cout << "Fishing Pole Made: " + to_str(++fishing_poles) << std::endl;
			p->inventory[stick_indexes[0]] = Item(FISHING_POLE);
			p->inventory[stick_indexes[0]].sale = true;
			p->inventory[stick_indexes[1]].name = "null";
			p->inventory[stick_indexes[2]].name = "null";
			s = relationName(p, obs) + ": crafts a *YELLOW*fishing pole*YELLOW*.";
		}
		else if (p->gold > 15) {
			if (freeSlots > 0) {
				for (int i = 0; i < map[p->y][p->x].items.size(); i++) {
					if (freeSlots >= 3 && low(map[p->y][p->x].items[i].name) == "stick") {
						return Get(p, map[p->y][p->x].items, i, obs);
						freeSlots--;
					}
				}
			}
			for (Person* ape : map[p->y][p->x].people) {
				for (int j = 0; j < INVENTORY_SIZE; j++) {
					if (ape->inventory[j].name == "stick") {
						if (ape->inventory[j].sale && p->gold >= ape->inventory[j].value) {
							for (int k = 0; k < INVENTORY_SIZE-1; k++) {
								if (p->inventory[k].name == "null") {
									p->gold -= ape->inventory[j].value;
									ape->gold += ape->inventory[j].value;
									p->inventory[k] = ape->inventory[j];
									p->inventory[k].sale = false;
									ape->inventory[j].name = "null";
									found_stick = true;
									break;
								}
							}
						}
					}
				}
			}
			if (!found_stick && rand() % 8 == 0) {
				Spot min = findSale(map, p, "stick");
				if (min.x != -1) { //Move to stick
					return setPath(map, p, min, obs);
				}
			}
		}
		else {
			Spot min = findJob(map, p, { "fisher" }, 35);
			if (min.x != -1) { //Move to fishermen
				return setPath(map, p, min, obs);
			}
		}
	}
	else if (job == "chef") {
		bool fire_active = map[p->y][p->x].utilities.size() > 0;
		bool has_fuel = false;
		char food = 0;
		for (int i = 0; i < INVENTORY_SIZE; i++) {
			if (p->inventory[i].name != "null") {
				if (items[p->inventory[i].i].type == FOOD) {
					p->inventory[i].sale = true;
					sale = true;
				}
				else if (items[p->inventory[i].i].cook > -1) {
					if (fire_active && map[p->y][p->x].utilities[0].fuel > 0) {
						map[p->y][p->x].utilities[0].active = true;
						cooked = 1;
						static int num_cook = 0;
						std::cout << ++num_cook << std::endl;
						return Cook(p, i, map[p->y][p->x].utilities, obs);
					}
					else {
						++food;
					}
				}
				else if (items[p->inventory[i].i].fuel > 0) { //put stuff in fire
					if (fire_active) {
						map[p->y][p->x].utilities[0].fuel += items[p->inventory[i].i].fuel;
						p->inventory[i].name = "null";
					}
					else {
						has_fuel = true;
					}
				}
			}
		}
		if (!fire_active && (has_fuel && (food > p->gold/15 || food > 3))) {
			Spot min = findUtility(map, p); //move to nearby fire
			int d = dist(Spot(p->x, p->y), min);
			if (min.x != -1 && d > 0 && d < 9) {
				return setPath(map, p, min, obs);
			}
			else if (d > 9) {
				Utility campfire("campfire", 0, true);
				map[p->y][p->x].utilities.push_back(Utility("campfire", 0, false));
				return relationName(p, obs) + " builds a *FIRE*campfire*FIRE*";
			}
		}
		else if (fire_active && map[p->y][p->x].utilities[0].fuel <= 0 && !has_fuel && p->gold > 15) {
			Spot min = findSale(map, p, "stick");
			int d = dist(Spot(p->x, p->y), min);
			if (min.x != -1 && d <= 9) { //Move to someone selling sticks
				return setPath(map, p, min, obs);
			}
		}
		for (Person* ape : map[p->y][p->x].people) { //buy raw food or sticks
			if (ape != p) {
				for (int j = 0; j < INVENTORY_SIZE; j++) {
					if (ape->inventory[j].name != "null" && ape->inventory[j].sale && p->gold >= ape->inventory[j].value) {
						if ((p->gold > 15 && low(ape->inventory[j].name) == "stick") || items[ape->inventory[j].i].cook > -1) {
							for (int k = 0; k < INVENTORY_SIZE; k++) {
								if (p->inventory[k].name == "null") {
									p->gold -= ape->inventory[j].value;
									ape->gold += ape->inventory[j].value;
									p->inventory[k] = ape->inventory[j];
									p->inventory[k].sale = false;
									ape->inventory[j].name = "null";
									static int chef_buys = 0;
									std::cout << "Chef Buys: " + to_str(++chef_buys) << std::endl;
									s = "chef buys ingredients";
									break;
								}
							}
						}
					}
				}
			}
		}
		if (p->move == HERE && food < 4 && p->gold > 20 && s == "") {
			Spot min = findSale(map, p, "", { RAW_FOOD });
			if (min.x != -1) { //Move to someone selling food
				return setPath(map, p, min, obs);
			}
		}
	}
	else if (job == "beggar") {
		int ran = rand() % 8;
		if (p->gold >= 10 || p->state.hunger >= 100) {
			p->job = "gatherer";
		}
		else {
			if (ran == 0) {
				s = relationName(p, obs) + ": 'pls free items'";
			}
			else if (ran == 1) {
				s = relationName(p, obs) + ": 'gold please'";
			}
			else if (ran > 3) {
				Spot min = findSpot(map, p, Spot(p->x, p->y), 't', 0, true); //move to nearby town
				if (min.x != -1) { //Move to someone selling food
					return setPath(map, p, min, obs);
				}
			}
		}
	}
	if (returnHome) {
		//Return to ur home
		return setPath(map, p, p->home, obs);
	}
	return s;
}

std::string Battle(std::vector<Person*> allies, std::vector<Person*> enemies, int turn, CombatHandler& combat, Person* obs) {
	std::string s = "";
	std::string order = "";

	bool succ = false;

	int dmg = 0;
	int max_dmg = 0;

	int max_index = -1;

	int targets = 0;

	if (turn > allies.size() || turn < 0) {
		return "*RED*ERROR!*RED* Incorrect index";
	}

	allies[turn]->ap.val = min(allies[turn]->ap.val + 3, allies[turn]->ap.max);
	allies[turn]->sp.val = min(allies[turn]->sp.val + 1, allies[turn]->sp.max);

	//Control what the AI does in a battle.
	for (int i = 0; i < allies[turn]->attacks.size(); i++) {
		if (allies[turn]->ap.val >= energy_cost(attacks[allies[turn]->attacks[i]], allies[turn]->weapon)) {
			dmg = attacks[allies[turn]->attacks[i]].dmg * items[allies[turn]->weapon.i].dmg.dmg;
			if (dmg > max_dmg) {
				max_index = i;
				max_dmg = dmg;
			}
		}
	}

	if (max_index > -1) {
		order = attacks[allies[turn]->attacks[max_index]].name + " ";

		//Choose targets
		for (int i = 0; i < enemies.size(); i++) {
			if (!(enemies[i]->dead) && (enemies[i]->hp.val > 0 || combat.level == KILL)) {
				order += 'a' + i;
				targets++;
				if (targets < attacks[allies[turn]->attacks[max_index]].targets) { //Look for another target
					order += " ";
				}
				else { //End the loop
					break;
				}
			}
		}
		s = attack(allies[turn], order, enemies, obs, succ);
	}

	if (!succ) { //Rest
		allies[turn]->ap.val += 10; 
		s = allies[turn]->name + " rests. ";
	}
	return s;
}