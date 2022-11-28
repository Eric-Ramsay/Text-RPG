#pragma once
#include "classes.h"
#include "functions.h"

std::string Fish(Person* p, Tile t, Person* obs) {
	bool tp = obs != p;
	std::string s = "";
	char waterType = 'e';
	int ran;

	static int num_fish = 0;

	Item fish(RAW_FISH);
	Item turtle(RAW_TURTLE);
	if (p->job == "fisher") {
		fish.sale = true;
		turtle.sale = true;
	}
	int freeIndex = -1;

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (p->inventory[i].name == "null") {
			freeIndex = i;
			break;
		}
	}
	if (freeIndex == -1) {
		if (tp) {
			return "";
		}
		return "You *RED*don't*RED* have any space in your inventory. ";
	}

	static int caught = 0;
	if (p->weapon.name != "null" && low(p->weapon.name) == "fishing pole") {
		for (int i = HERE; i <= SOUTH; i++) {
			if (t.water[i] != waterType) {
				waterType = t.water[i];
				break;
			}
		}
		if (waterType == 'e') {
			if (!tp) {
				s = "There's no *BLUE*water*BLUE* here!";
			}
		}
		else {
			ran = rand() % 100;
			if (ran >= 20) {
				if (!tp) {
					s = "You *RED*don't*RED* catch anything. ";
				}
			}
			else {
				if (ran == 0) {
					if (!tp) {
						s = "Your *YELLOW*fishing rod*YELLOW* *RED*breaks*RED*!";
					}
					else {
						s = relationName(p, obs, "'s ") + "*YELLOW*fishing rod*YELLOW* *RED*breaks*RED*!";
					}
					p->weapon.name = "null";
				}
				else {
					ran = rand() % 5;
					//std::cout << "Fish caught: " + to_str(++num_fish) << std::endl;
					if (ran) {
						if (!tp) {
							s = "You catch a *YELLOW*fish*YELLOW*!";
						}
						else {
							s = relationName(p, obs) + " catches a *YELLOW*fish*YELLOW*!";
						}
						p->inventory[freeIndex] = fish;
						caught++;
						std::cout << "Caught: " << caught << std::endl;
					}
					else {
						if (!tp) {
							s = "You catch a *YELLOW*turtle*YELLOW*!";
						}
						else {
							s = relationName(p, obs) + " catches a *YELLOW*turtle*YELLOW*!";
						}
						p->inventory[freeIndex] = turtle;
						caught++;
						std::cout << "Caught: " << caught << std::endl;
					}
				}
			}
		}
	}
	else {
		s = "No fishing pole equipped!";
	}

	return s;
}

std::string attack(Person* p, std::string order, std::vector<Person*> enemies, Person* obs, bool& success) {
	//All attacks are no more than two words.
	std::string attack_name = "";
	std::string alt_attack_name = "";
	std::vector<std::string> words = {};
	std::string word = "";
	std::string s = "";
	int loop = 2;
	int target_index = 1;
	bool you = p == obs;
	success = false;

	for (int i = 0; i < order.length(); i++) {
		if (order[i] == ' ') {
			words.push_back(word);
			word = "";
		}
		else {
			word += std::tolower(order[i]);
		}
	}
	if (word.length() > 0) {
		words.push_back(word);
	}

	if (words.size() == 1) {
		return "attack not found. ";
	}

	attack_name = words[0];
	alt_attack_name = words[0] + " " + words[1];

	for (int a = 0; a < loop; a++) {
		s = "";
		if (a == 1) {
			attack_name = alt_attack_name;
			target_index = 2;
		}
		for (int i = 0; i < p->attacks.size(); i++) {
			if (attacks[p->attacks[i]].name == attack_name) { //Attack is found!
				loop = 1; //Don't loop anymore.
				if (words.size() - target_index > 0 && words.size() - target_index <= attacks[p->attacks[i]].targets) { //Correct number of words for targetting
					int energy = energy_cost(attacks[p->attacks[i]], p->weapon);
					int dmg = attacks[p->attacks[i]].dmg * items[p->weapon.i].dmg.dmg;
					int pen = attacks[p->attacks[i]].pen * items[p->weapon.i].dmg.pen;
					int hit = max(1, attacks[p->attacks[i]].hit + items[p->weapon.i].dmg.hit - p->skills[MELEE]);
					std::vector<int> enemy_index = {};

					if (p->ap.val >= energy) { //Can perform attack.
						for (int j = target_index; j < words.size(); j++) { //Find Targetted enemies
							if (words[j].size() == 1) { //Check letters a, b, c, d
								enemy_index.push_back((int)(words[j][0] - 'a'));
							}
							else {
								for (int k = 0; k < enemies.size(); k++) {
									if (low(enemies[k]->name) == words[j]) { //Target found
										enemy_index.push_back(k);
										break;
									}
								}
							}
						}
						if (enemy_index.size() == 0) {
							return "You must target an enemy!";
						}
						for (int j : enemy_index) { //Validate that targetted enemies can be hit
							if (j > enemies.size() || enemies[j]->dead) {
								return "You can't attack that enemy!";
							}
						}
						success = true;
						p->ap.val -= energy; //Good to go.
						for (int j : enemy_index) { //Validate that targetted enemies can be hit
							if (you) {
								s = "*GREEN*You*GREEN* hit ";
							}
							else {
								s = "*TEAL*" + p->name + "*TEAL* hits ";
							}
							if (!(enemies[j]->sentient)) {
								s += "the ";
							}
							s += "*RED*" + enemies[j]->name + "*RED* for *YELLOW*" + to_str(dmg) + "*YELLOW* damage! ";
							if (enemies[j]->hp.val > 0) {
								if (enemies[j]->sp.val >= dmg) {
									enemies[j]->sp.val -= (dmg);
								}
								else {
									enemies[j]->hp.val -= (dmg - enemies[j]->sp.val);
									enemies[j]->sp.val = 0;
									if (enemies[j]->hp.val < 0) {
										enemies[j]->hp.val = 0;
									}
								}
							}
							else if (dmg > 0) {
								enemies[j]->dead = true;
								if (you) {
									s = "*GREEN*You*GREEN* kill ";
								}
								else {
									s = "*TEAL*" + p->name + "*TEAL* kills ";
								}
								if (!(enemies[j]->sentient)) {
									s += "the ";
								}
								s += "*RED*" + enemies[j]->name + "*RED*";
							}
						}
					} // End of Attack Processing
				} 
			}
		}
		if (a > 0 && s == "") {
			s = "attack not found. ";
		}
	}

	return s;
}

std::string Eat(Person* p, char index, Person* obs) {
	bool you = p == obs;
	std::string s;
	std::string action = "";
	std::string article = "";
	if (index > INVENTORY_SIZE || index < 0) {
		return "*RED* Invalid inventory position *RED*";
	}

	if (p->inventory[index].name != "null") {
		if (items[p->inventory[index].i].hunger != 0) {
			p->state.hunger -= items[p->inventory[index].i].hunger;
			article = an(p->inventory[index].name);
			action = "eat";
		}
		if (items[p->inventory[index].i].thirst != 0) {
			p->state.thirst -= items[p->inventory[index].i].thirst;
			if (action == "") {
				action = "drink";
			}
		}
		if (action == "") {
			return "*RED* You can't eat that. *RED*";
		}
		else {
			p->state.misery -= items[p->inventory[index].i].flavor;
		}
		if (!you) {
			action += "s";
		}
		action = "*ORANGE*" + action + "*ORANGE*";
		if (you) {
			s = "*GREEN*You*GREEN* " + action + " " + article + "*YELLOW*" + p->inventory[index].name+"*YELLOW*.";
		}
		else {
			s = relationName(p, obs) + " " + action + " " + article + "*YELLOW*" + p->inventory[index].name+"*YELLOW*.";
		}
		p->inventory[index].name = "null";
	}
	p->state.hunger = max(0, p->state.hunger);
	p->state.thirst = max(0, p->state.thirst);
	p->state.misery = max(0, p->state.misery);

	return s;
}

std::string Cook(Person* p, int index, std::vector<Utility> fires, Person* obs) {
	bool you = p == obs;
	std::string s = "*RED*There's no fire here to cook on.*RED*";
	std::string action = "";
	std::string article = "";
	bool found = false;

	if (index > INVENTORY_SIZE || index < 0) {
		return "*RED* Invalid inventory position *RED*";
	}
	int ran = rand() % 5;
	int cookingSkill = 1;

	for (int i = 0; i < fires.size(); i++) {
		if (fires[i].active) {
			found = true;
			break;
		}
	}
	
	if (found) {
		if (p->inventory[index].name != "null") {
			if (items[p->inventory[index].i].cook > -1){
				p->inventory[index] = Item(items[p->inventory[index].i].cook);
				p->inventory[index].value += 2*ran * cookingSkill;
				p->inventory[index].color = colorize(100 - (ran * 10 * cookingSkill), true);
				if (you) {
					s = "*GREEN*You*GREEN* *TEAL*cook*TEAL* the *YELLOW*" + p->inventory[index].name + "*YELLOW*";
				}
				else {
					article = an(p->inventory[index].name);
					s = relationName(p, obs) + " *TEAL*cooks*TEAL* " + article + "*YELLOW*" + p->inventory[index].name + "*YELLOW*";
				}
			}
			else {
				return "*RED* You can't cook that. *RED*";
			}
		}
	}
	return s;
}

std::string Get(Person* p, std::vector<Item>& stuff, int index, Person* obs) {
	bool you = p == obs;
	std::string s = "";

	std::string name = stuff[index].name;
	std::string word = "";
	std::vector <std::string> words;
	for (int i = 0; i < name.length(); i++) {
		if (name[i] == ' ') {
			if (word.length() > 0 && word != "the")
				words.push_back(word);
			word = "";
		}
		else {
			word += std::tolower(name[i]);
		}
	}
	if (word.length() > 0) {
		words.push_back(word);
	}

	if (words.size() == 2 && low(words[1]) == "gold") {
		p->gold += stuff[index].value;
	}
	else {
		for (int i = 0; i < INVENTORY_SIZE; i++) {
			if (p->inventory[i].name == "null") {
				p->inventory[i] = stuff[index];
				if (!you) {
					s = relationName(p, obs) + " gets " + stuff[index].name;
				}
				else {
					s = "You get " + stuff[index].name;
				}
				break;
			}
		}
		if (s == "") {
			return "No space in inventory. ";
		}
	}
	
	stuff.erase(stuff.begin() + index);

	return s;
}

std::string moveDirection(std::vector<std::vector<Tile>>& map, Person* p, Person* obs) {
	std::string s = "";
	bool you = p == obs;
	int x = p->x;
	int y = p->y;

	//relationName is a function used to print someone's name as they'd be seen by the observor - with an opinion block beside their name, and with their name colored to what clothes they're wearing.
	if (p->move == NORTH) {
		if (y > 0 && map[y - 1][x].water[HERE] != 'w') {
			Move(p, map[y][x], map[y - 1][x]);
			if (!you) {
				s = relationName(p, obs) + " goes *TEAL*north*TEAL*. ";
			}
			else {
				s = "*GREEN*You*GREEN* go *TEAL*north*TEAL*";
			}
		}
	}
	else if (p->move == SOUTH) {
		if (y < (MAP_H * 3) - 1 && map[y + 1][x].water[HERE] != 'w') {
			Move(p, map[y][x], map[y + 1][x]);
			if (!you) {
				s = relationName(p, obs) + " goes *TEAL*south*TEAL*. ";
			}
			else {
				s = "*GREEN*You*GREEN* go *TEAL*south*TEAL*";
			}
		}
	}
	else if (p->move == EAST) {
		if (x < (MAP_W * 3) - 1 && map[y][x + 1].water[HERE] != 'w') {
			Move(p, map[y][x], map[y][x + 1]);
			if (!you) {
				s = relationName(p, obs) + " goes *TEAL*east*TEAL*. ";
			}
			else {
				s = "*GREEN*You*GREEN* go *TEAL*east*TEAL*";
			}
		}
	}
	else if (p->move == WEST) {
		if (x > 0 && map[y][x - 1].water[HERE] != 'w') {
			Move(p, map[y][x], map[y][x - 1]);
			if (!you) {
				s = relationName(p, obs) + " goes *TEAL*west*TEAL*. ";
			}
			else {
				s = "*GREEN*You*GREEN* go *TEAL*west*TEAL*";
			}
		}
	}

	if (you) {
		//s += "\n" + Look(p, map[p->y][p->x]);
	}

	p->move = HERE;

	return s;
}

std::string setMovement(Person* p, Spot to, Person* obs) {
	std::string s = "";
	std::string subject = "*GREEN*You*GREEN*";
	std::string verb = " turn ";
	Direction dir;

	if (p != obs) {
		verb = " turns ";
		subject = relationName(p, obs);
	}

	dir = spotMovement(Spot(p->x, p->y), to);
	p->move = dir;

	if (dir != HERE) {
		s = subject + verb + dirName(dir);
	}

	return s;
}

std::string setPath(std::vector<std::vector<Tile>>& map, Person* p, Spot to, Person* obs) {
	std::string s = "";
	p->path = pathfind(map, Spot(p->x, p->y), to);
	if (p->path.size() > 0) {
		s = setMovement(p, p->path.front(), obs);
		p->path.pop_front();
	}	
	return s;
}