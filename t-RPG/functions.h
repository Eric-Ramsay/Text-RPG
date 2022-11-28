#pragma once
#include "string"

int dist(int x1, int x2) {
	return std::abs(x1 - x2);
}

int dist(int x1, int y1, int x2, int y2) {
	return std::abs(x1 - x2) + std::abs(y1 - y2);
}

int dist(Spot from, Spot to) {
	return dist(from.x, from.y, to.x, to.y);
}
float fDist(int x1, int y1, int x2, int y2) {
	int xDist = std::abs(x1 - x2);
	int yDist = std::abs(y1 - y2);
	return std::sqrt(xDist * xDist + yDist * yDist);
}
template < typename Type > std::string to_str(const Type& t)
{
	std::ostringstream os;
	os << t;
	return os.str();
}
std::string an(std::string a) {
	switch (std::tolower(a[0])) {
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u': return "an ";
	}
	return "a ";
}

std::string low(std::string a) {
	std::string word = "";
	bool skip = false;
	for (char c : a) {
		if (c == '*') {
			skip = !skip;
		}
		if (skip) {
			word += c;
		}
		else {
			word += std::tolower(c);
		}
	}
	return word;
}

std::string numToWord(int num) {
	std::string s = "";
	switch (num) {
	case 0: s = "Zero"; break;
	case 1: s = "One"; break;
	case 2: s = "Two"; break;
	case 3: s = "Three"; break;
	case 4: s = "Four"; break;
	case 5: s = "Five"; break;
	case 6: s = "Six"; break;
	case 7: s = "Seven"; break;
	case 8: s = "Eight"; break;
	case 9: s = "Nine"; break;
	}
	return s;
}

int charWidth(char c, bool small_space = false) {
	int sW = 3;
	switch (c) {
	case '\'':case '"': sW = 1; break;
	case '.': sW = 1; break;
	case ',': sW = 1; break;
	case ';': sW = 1; break;
	case '!': sW = 1; break;
	case ':': sW = 1; break;
	case '[': sW = 2; break;
	case '|': sW = 2; break;
	case ']': sW = 2; break;
	case '(': sW = 2; break;
	case ')': sW = 2; break;
	case ' ': if (small_space) { sW = 1; } break;
	case 5: sW = 5; break;
	}
	return sW;
}

int measureText(std::string text) {
	bool skipping = false;
	int size = 0;
	for (char& c : text) {
		if (c == '*') {
			skipping = !skipping;
		}
		else if (!skipping) {
			if (c != '\n') {
				size += charWidth(c) + 1;
			}
		}
	}
	return size;
}

int safeC(int a, int MAX) {
	if (a >= MAX) {
		return a % MAX;
	}
	else if (a < 0) {
		return safeC(a + MAX, MAX);
	}
	return a;
}

int Time(float x=0) {
	static float time = 0;
	if (time > 24) {
		time -= 24;
	}
	return time;
}

void Death(Tile* t, Person* p) {
	//Move person's belongings to tile's item list:
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (p->inventory[i].name != "null") {
			t->items.push_back(p->inventory[i]);
		}
	}
	if (p->gold > 0) {
		Item gold(GOLD);
		gold.color = "*YELLOW*";
		gold.name = to_str(p->gold) + " gold";
		gold.value = p->gold;
		t->items.push_back(gold);
	}

	if (p->outfit.name != "null") {
		t->items.push_back(p->outfit);
	}
	if (p->weapon.name != "null") {
		t->items.push_back(p->weapon);
	}
	if (p->trinket.name != "null") {
		t->items.push_back(p->trinket);
	}
	for (int i = 0; i < t->people.size(); i++) {
		if (t->people[i] == p) {
			delete p;
			t->people.erase(t->people.begin() + i);
			break;
		}
	}
}

int energy_cost(Attack a, Item weapon) {
	if (weapon.name != "null") {
		return a.base_cost + a.weight_cost * items[weapon.i].size;
	}
	return 5 + a.base_cost;
}


void startFight(Person* p, Person* target, Tile& t, Level combat_level) {
	//p is attacking target. Check the tile for anyone who will help ape. alter the combat handler and push it to the tile's list of combat handlers

	p->combat = true;
	target->combat = true;

	CombatHandler combat;

	combat.allies = { p };
	combat.enemies = { target };

	combat.level = combat_level;

	if (p->society != target->society) { //Society Clash
		for (Person* ape : t.people) {
			if (ape->eats && !(ape->combat)) {
				if (combat.allies.size() < 4 && ape->society == p->society) {
					ape->combat = true;
					combat.allies.push_back(ape);
				}
				else if (combat.enemies.size() < 4 && ape->society == target->society) {
					ape->combat = true;
					combat.enemies.push_back(ape);
				}
			}
		}
	}
	else { //Idk. Leave it alone for now. . .

	}

	t.fights.push_back(combat);
}

void incrementTurn(CombatHandler& combat) {
	bool condition = false;
	do {
		combat.turn = (combat.turn + 1) % 8;
		if (combat.turn % 2 == 0) {
			condition = combat.turn >= 2 * combat.allies.size() || combat.allies[combat.turn / 2]->dead || combat.allies[combat.turn / 2]->hp.val <= 0;
			if (!condition) {
				combat.allies[combat.turn / 2]->ap.val = min(combat.allies[combat.turn / 2]->ap.max, combat.allies[combat.turn / 2]->ap.val + 5);
			}
		}
		else {
			condition = combat.turn >= 2 * combat.enemies.size() || combat.enemies[combat.turn/2]->dead || combat.enemies[combat.turn / 2]->hp.val <= 0;
			if (!condition) {
				combat.enemies[combat.turn / 2]->ap.val = min(combat.enemies[combat.turn / 2]->ap.max, combat.enemies[combat.turn / 2]->ap.val + 5);
			}
		}
	} while (condition);
}


void imprison_losers(std::vector<Person*>& winners, std::vector<Person*>& losers) { 
	for (int i = 0; i < winners.size(); i++) {
		if (!winners[i]->dead && winners[i]->sentient) {
			for (int j = 0; j < losers.size(); j++) {
				if (!losers[j]->dead && losers[j]->sentient) {
					winners[i]->prisoners.push_back(losers[j]->id);
					losers[j]->masters.push_back(winners[i]->id);
				}
			}
		}
	}
}

void endCombat(std::vector<Person*>& team, Tile* t) {
	for (int i = team.size()-1; i >= 0; i--) {
		team[i]->combat = false;
		if (!team[i]->dead) {
			team[i]->hp.val += (team[i]->hp.max - team[i]->hp.val) / 5;
		}
		else {
			Death(t, team[i]);
			team.erase(team.begin() + i);
		}
	}

}

bool checkSide(std::vector<Person*> side, Level level) { //Check true if side is defeated.
	for (Person* ape : side) { 
		if (!(ape->dead)) {
			if (level == KILL || (ape->hp.val > 0 && !ape->yield)) {
				return false;
			}
		}
	}
	return true;
}

bool checkCombat(CombatHandler& combat, Tile* t, bool end = true) { //Check if combat needs to end. If it does, end it.
	bool ended = true;
	bool allies_win = false;

	ended = checkSide(combat.allies, combat.level);

	if (!ended) {
		ended = checkSide(combat.enemies, combat.level);
		allies_win = ended;
	}

	if (ended && end) {
		endCombat(combat.allies, t);
		endCombat(combat.enemies, t);
		if (allies_win) {
			imprison_losers(combat.allies, combat.enemies);
		}
		else {
			imprison_losers(combat.enemies, combat.allies);
		}
	}

	return ended;
}

template <typename T>
T test_sum(T arr[], int len) {
	T total;
	for (int i = 0; i < len; i++) {
		if (i == 0) {
			total = arr[i];
		}
		else {
			total += arr[i];
		}
	}
	return total;
}


bool isNull(std::string s) {
	if (s.size() < 4) {
		return false;
	}
	return s[0] == 'n' && s[1] == 'u' && s[2] == 'l' && s[3] == 'l';
}

/*std::vector<std::string> chopString(std::string text, int cap) {
	std::vector<std::string> lines;
	std::vector<std::string> words;
	int line_len = 0;
	int word_len = 0;
	std::string line = "";
	std::string word = "";
	bool skipping = false;

	for (char& c : text) {
		if (c == '*') {
			skipping = !skipping;
		}
		word += c;
		if (!skipping) {
			if (c == ' ') {
				word_len = measureText(word);
				if (line_len + word_len < cap) {
					line += word;
					line_len += word_len;
				}
				else {
					lines.push_back(line);
					line = "";
					line_len = 0;
				}
				word = ""; word_len = 0;
			}
			else if (c == '\n') {
				line += word;
				lines.push_back(line);
				line = ""; line_len = 0;
				word = ""; word_len = 0;
			}
		}
	}
	if (word != "") {
		line += word;
	}
	if (line != "") {
		lines.push_back(line);
	}
	return lines;
}*/

Number strToDmg(std::string s) {
	Number num;

	std::string number = "";
	std::string dIndex = "";
	int index = 0;
	bool die = false;

	for (char c : s) {
		if (c != ' ') {
			if (c != 'd') {
				if (!die) { //recording a base number, or the lead on a die ie the '1' in 1d4
					number += c;
				}
				else { //recording what type of die it is, ie the '4' in 1d4
					dIndex += c;
				}
			}
			else { //Switch from leading number to the die index
				die = true;
				dIndex = -1;
			}
		}
		else { //separate entity
			if (!die) { //base #
				num.base += std::stoi(number);
			}
			else { //die type
				index = std::stoi(dIndex);
				index -= 4;
				index /= 2;
				if (index >= 0 && index <= num.num) {
					num.die[index] += std::stoi(number);
				}
			}
		}
	}

	return num;
}

void ParseTalk(Conversation& t, std::string command, TradeHandler& trade) {
	Relation* opinion = nullptr;
	int ran = 0;
	int change = 0;
	for (int r = 0; r < t.target->relations.size(); r++) {
		if (t.target->relations[r].id == t.you->id) {
			opinion = &t.target->relations[r];
			break;
		}
	}
	if (opinion == nullptr) {
		std::cout << "relation not found somehow! " << std::endl;
		return;
	}
	int chat_desire = opinion->chat_desire + t.target->chat_desire;
	if (chat_desire <= 0) {
		t.active = false;
	}

	if (t.gift_value > 0) {
		ran = rand() % 3;
		if (opinion->value > 25) { //They already like you. 
			switch (ran) {
			case 0: t.statement = "Thank you, " + t.you->name + "!"; break;
			case 1: t.statement = "Thank you for that!"; break;
			case 2: t.statement = "Thanks, you're such a nice friend."; break;
			}
		}
		if (opinion->value < -25) { //They dislike you.
			switch (ran) {
			case 0: t.statement = "Hm."; break;
			case 1: t.statement = "Why'd you give me this?"; break;
			case 2: t.statement = "Don't think this changes anything between us."; break;
			}
			opinion->value -= 2;
		}
		else { //They're neutral to you. They take the compliment, but too much will freak them out.
			switch (ran) {
			case 0: t.statement = "Really? For me?"; break;
			case 1: t.statement = "Thank you for the gift."; break;
			case 2: t.statement = "Why thank you."; break;
			}
		}
		t.gift_value = 0;
	}
	else if (t.gift_value < 0) {

	}
	else if (t.page == 0) {
		if (command == "1") { //Compliment
			change = 2 + rand() % 10;
			ran = rand() % 4;
			if (opinion->value > 25) { //They already like you. They enjoy the compliment, but don't push it too far.
				switch (ran) {
				case 0: t.statement = "Thank you, " + t.you->name; break;
				case 1: t.statement = "What a lovely thing to say."; break;
				case 2: t.statement = "Oh, you're just being nice."; break;
				case 3: t.statement = "Aw, stop it."; break;
				}
				opinion->value += change;
				opinion->chat_desire -= change;
				ran = rand() % 3;
				if (opinion->chat_desire < 20 && opinion->chat_desire > 0) {
					switch (ran) {
					case 0: t.statement = "Thanks, but was there something you wanted to discuss?"; break;
					case 1: t.statement = "Thanks I guess."; break;
					case 2: t.statement = "Did you need something?"; break;
					}
				}
				else if ((t.target->chat_desire + opinion->chat_desire) <= 0) {
					switch (ran) {
					case 0: t.statement = "Thank you, but I have other things to do."; break;
					case 1: t.statement = "Sorry, I'm not in the mood for compliments."; break;
					case 2: t.statement = "Thanks, but let's continue this conversation later."; break;
					}
				}
			}
			if (opinion->value < -25) { //They dislike you. The compliment won't be very effective
				switch (ran) {
				case 0: t.statement = "You're wasting your breath."; break;
				case 1: t.statement = "Leave me alone."; break;
				case 2: t.statement = "What are you talking about?"; break;
				case 3: t.statement = "Get a life."; break;
				}
				opinion->value += change /2;
				opinion->chat_desire -= change * 2;
			}
			else { //They're neutral to you. They take the compliment, but too much will freak them out.
				switch (ran) {
				case 0: t.statement = "Why thank you."; break;
				case 1: t.statement = "Oh, don't flatter me."; break;
				case 2: t.statement = "That's too much, thank you."; break;
				case 3: t.statement = "That's nice."; break;
				}
				if (rand() % 20 == 0) {
					t.statement = "That's funny. I've heard the same about your mother.";
				}
				opinion->value += change;
				opinion->chat_desire -= change * 2;
				if ((t.target->chat_desire + opinion->chat_desire) < 20 && (t.target->chat_desire + opinion->chat_desire) > 0) {
					opinion->value -= change/2;
					switch (ran) {
					case 0: t.statement = "Uh, yeah thanks. . ."; break;
					case 1: t.statement = "What? I barely know you."; break;
					case 2: t.statement = "What a strange thing to say to a stranger."; break;
					}
				}
				else if ((t.target->chat_desire + opinion->chat_desire) <= 0) {
					opinion->value -= (5 + rand() % 10);
					switch (ran) {
					case 0: t.statement = "Leave me alone."; break;
					case 1: t.statement = "Creep."; break;
					case 2: t.statement = "Please stop talking to me."; break;
					}
				}
			}
		}
		else if (command == "2") { //Insult
			ran = 3 + rand() % 10;
			opinion->value = ran;
			opinion->chat_desire -= ran * 2;
			ran = rand() % 4;
			if (opinion->value > 25) {
				switch (ran) {
				case 0: t.statement = "You don't mean that!"; break;
				case 1: t.statement = "Why would you say that about me?"; break;
				case 2: t.statement = "Are you mad at me?"; break;
				case 3: t.statement = "What? What's the matter?"; break;
				}
			}
			if (opinion->value < -25) {
				switch (ran) {
				case 0: t.statement = "Right back at you, jerk."; break;
				case 1: t.statement = "That's funny. I've heard the same about your mother."; break;
				case 2: t.statement = "Do you have nothing better to do?"; break;
				case 3: t.statement = "Ouch."; break;
				}
			}
			else {
				switch (ran) {
				case 0: t.statement = "Really? That's what you think of me?"; break;
				case 1: t.statement = "Why are you being so rude?"; break;
				case 2: t.statement = "What's your problem?"; break;
				case 3: t.statement = "Jerk!"; break;
				}
				if (chat_desire <= 0) {
					switch (ran) {
					case 0: t.statement = "Leave me alone!"; break;
					case 1: t.statement = "I'm not going to put up with this."; break;
					case 2: t.statement = "Don't talk to me!"; break;
					}
				}
			}
		}
		else if (command == "3" || command == "4") { //Trade
			trade = TradeHandler();
			trade.active = true;
			trade.p1 = t.you;
			trade.p2 = t.target;
			if (command == "4") { //Gift
				trade.giving = true;
			}
		}
		else if (command == "5") { //End conversation
			t.active = false;
		}
	}
	opinion->value = min(100, opinion->value);
	opinion->value = max(-100, opinion->value);
}

void appraiseTrade(TradeHandler& t) {

	float modifier = 1.0f; //deal with charm advantage here blah.
	int value = 0;

	int p1_slots = 0;
	int p2_slots = 0;

	t.msg = "";

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (t.p2->inventory[i].name != "null") {
			if (t.buy[i]) {
				value -= t.p2->inventory[i].value;
				p1_slots--;
				p2_slots++;
			}
		}
		else {
			p2_slots++;
		}
		if (t.p1->inventory[i].name != "null") {
			if (t.sell[i]) {
				if (t.giving) {
					value += min(10, t.p1->inventory[i].value - 4);
				}
				else {
					value += t.p1->inventory[i].value;
				}
				p2_slots--;
				p1_slots++;
			}
		}
		else {
			p1_slots++;
		}
	}

	t.gold = value;
	if (!t.giving) {
		if (t.gold > 0) {
			if (t.p2->gold < t.gold) {
				t.msg = "Seller doesn't have enough *YELLOW*GOLD*YELLOW*!\n";
			}
		}
		else if (t.gold < 0) {
			if (t.p1->gold < abs(t.gold)) {
				t.msg = "You don't have enough *YELLOW*GOLD*YELLOW*!\n";
			}
		}
	}
	else if (t.gold < 0) {
		t.msg = "*TEAL*" + t.p2->name + "*TEAL* doesn't want your *FISH*gift*FISH*!";
	}

	if (p1_slots < 0) {
		t.msg += "You don't have enough space in your inventory. \n";
	}
	if (p2_slots < 0) {
		t.msg = "Seller doesn't have enough space in their inventory. ";
	}
}

void ExecuteTrade(TradeHandler& t) {

	Item p1[INVENTORY_SIZE];
	Item p2[INVENTORY_SIZE];

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (t.p2->inventory[i].name != "null") {
			for (int j = 0; j < INVENTORY_SIZE; j++) {
				if (t.buy[i]) {
					if (p1[j].name == "null") {
						p1[j] = t.p2->inventory[i];
						p1[j].sale = false;
						break;
					}
				}
				else {
					if (p2[j].name == "null") {
						p2[j] = t.p2->inventory[i];
						break;
					}
				}
			}
		}
		if (t.p1->inventory[i].name != "null") {
			for (int j = 0; j < INVENTORY_SIZE; j++) {
				if (t.sell[i]) {
					if (p2[j].name == "null") {
						p2[j] = t.p1->inventory[i];
						p2[j].sale = false;
						break;
					}
				}
				else {
					if (p1[j].name == "null") {
						p1[j] = t.p1->inventory[i];
						break;
					}
				}
			}
		}
	}
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		t.p1->inventory[i] = p1[i];
		t.p2->inventory[i] = p2[i];
	}
	if (t.giving) {
		for (int i = 0; i < t.p2->relations.size(); i++) {
			if (t.p2->relations[i].id == t.p1->id) {
				t.p2->chat_desire += t.gold;
				t.p2->relations[i].value += t.gold;
			}
		}
	}
	else {
		t.p1->gold += t.gold;
		t.p2->gold -= t.gold;
	}
	t = TradeHandler();
}

std::string handleFire(Utility& fire) {
	std::string s = "The *GREY*" + fire.name + "*GREY* isn't lit. ";
	s = "";
	int ran = rand() % 3;
	if (fire.active) {
		fire.fuel--;
		s = "The *FIRE*" + fire.name + "*FIRE* ";
		if (fire.fuel > 50) {
			s += "burns brightly. ";
		}
		else if (fire.fuel > 25) {
			s += "crackles quietly. ";
		}
		else if (fire.fuel > 10) {
			s += "glows. ";
		}
		else if (fire.fuel <= 0) {
			fire.active = false;
			s = "The *GREY*" + fire.name + "*GREY* is no longer lit. ";
		}
		else {
			if (ran == 0) {
				s += "is nearly extinguished. ";
			}
			if (ran == 1) {
				s += "burns very faintly. ";
			}
			if (ran == 2) {
				s += "glows, mere embers now. ";
			}
		}
	}
	return s;
}

unsigned long long int srandC(unsigned long long int seed) {
	unsigned long long int a = 1103515245;
	int c = 12345;
	unsigned long long int m = 2 ^ 32;
	return ((a * seed + c) % m);
}

float randC(int x1, int y1) {
	unsigned long long int x = (unsigned long long int)x1;
	unsigned long long int y = (unsigned long long int)y1;
	unsigned long long int a = 1103515245;
	int c = 12345;
	long m = 2 ^ 32;
	unsigned long long int seed = srandC(x + y + x * y + x * x + y * y);
	return fabsf((float)((a * seed + c) % m) / m);
}


bool greater(Pair p1, Pair p2) {
	return p1 > p2;
}

Spot findSpot(std::vector<std::vector<Tile>>& map, Person* p, Spot start, char lType = 0, char wType = 0, bool avoid_recent = false) {
	int minDist = 99;
	int step = -1;
	int d = 99;
	int x, y;
	Spot min = Spot(-1, -1);

	bool skip = false;

	//Find water
	while (minDist == 99 && ++step < 15) { // Iterative Deepening Search, will search 1 layer at a time, expanding out to 10 layers, stopping early if possible
		for (int i = 0 - step; i <= step; i++) {
			for (int j = 0 - step; j <= step; j++) {
				if (abs(i) == step || abs(j) == step) { //With each step, a box expands with dimensions step x step. This if makes sure the loop is on the outermost ring of the box.
					x = start.x + j;
					y = start.y + i;
					d = dist(i, j, 0, 0); //Determine the distance from the center, where the NPC is standing, to the tile being currently checked.
					if (d < minDist) {
						if (y >= 0 && y < map.size()) {
							if (x >= 0 && x < map[0].size()) {
								skip = false;
								if (avoid_recent) {
									for (Spot recent : p->recent) {
										skip = skip || (recent.x == x && recent.y == y);
									}
								}
								if (!skip) {
									if (wType != 0) {
										for (int k = NORTH; k <= SOUTH; k++) { //Check for water
											if ((map[y][x].water[k] != 'e' && wType == 'a') || (map[y][x].water[k] == wType)) {
												if (!avoid_recent)
													min = Spot(x, y);
												minDist = d;
											}
										}
									}
									else if (map[y][x].type == lType) {
										min = Spot(x, y);
										minDist = d;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return min;
}

Spot findSale(std::vector<std::vector<Tile>>& map, Person* p, std::string item_name = "", std::vector<TYPE> types = {}, int range = 12) {
	int minDist = 99;
	int step = -1;
	int d = 99;
	int x, y;
	Spot min = Spot(-1, -1);
	std::string name = low(item_name);
	bool found = false;

	bool skip = false;

	//Find water
	while (minDist == 99 && ++step < range) { // Iterative Deepening Search, will search 1 layer at a time, expanding out to 10 layers, stopping early if possible
		for (int i = 0 - step; i <= step; i++) {
			for (int j = 0 - step; j <= step; j++) {
				if (abs(i) == step || abs(j) == step) { //With each step, a box expands with dimensions step x step. This if makes sure the loop is on the outermost ring of the box.
					x = p->x + j;
					y = p->y + i;
					d = dist(i, j, 0, 0); //Determine the distance from the center, where the NPC is standing, to the tile being currently checked.
					if (d < minDist) {
						if (y >= 0 && y < map.size()) {
							if (x >= 0 && x < map[0].size()) {
								for (Person* ape : map[y][x].people) {
									if (ape != p && ape->sentient) {
										for (int a = 0; a < INVENTORY_SIZE; a++) {
											if (ape->inventory[a].sale && p->gold > ape->inventory[a].value && ape->inventory[a].name != "null") {
												for (TYPE t : types) {
													if (t == items[ape->inventory[a].i].type) {
														found = true;
														break;
													}
												}
												if (found || low(ape->inventory[a].name) == name) {
													min = Spot(x, y);
													minDist = d;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return min;
}
Spot findItem(std::vector<std::vector<Tile>>& map, Person* p, std::string item_name = "", std::vector<TYPE> types = {}, int range = 12) {
	int minDist = 99;
	int step = -1;
	int d = 99;
	int x, y;
	Spot min = Spot(-1, -1);
	bool found = false;
	std::string name = low(item_name);

	bool skip = false;

	//Find water
	while (minDist == 99 && ++step < range) { // Iterative Deepening Search, will search 1 layer at a time, expanding out to 10 layers, stopping early if possible
		for (int i = 0 - step; i <= step; i++) {
			for (int j = 0 - step; j <= step; j++) {
				if (abs(i) == step || abs(j) == step) { //With each step, a box expands with dimensions step x step. This if makes sure the loop is on the outermost ring of the box.
					x = p->x + j;
					y = p->y + i;
					d = dist(i, j, 0, 0); //Determine the distance from the center, where the NPC is standing, to the tile being currently checked.
					if (d < minDist) {
						if (y >= 0 && y < map.size()) {
							if (x >= 0 && x < map[0].size()) {
								for (Item it : map[y][x].items) {
									if (it.name != "null") {
										for (TYPE t : types) {
											if (t == items[it.i].type) {
												found = true;
												break;
											}
										}
										if (found || (low(it.name) == name)) {
											min = Spot(x, y);
											minDist = d;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return min;
}

Spot findJob(std::vector<std::vector<Tile>>& map, Person* p, std::vector<std::string> job1, int money_needed = 0, int range = 12) {
	int minDist = 99;
	int step = -1;
	int d = 99;
	int x, y;

	std::vector < std::string> jobs = job1;
	for (int i = 0; i < jobs.size(); i++) {
		jobs[i] = low(jobs[i]);
	}

	Spot min = Spot(-1, -1);
	//Find water
	while (minDist == 99 && ++step < range) { // Iterative Deepening Search, will search 1 layer at a time, expanding out to 10 layers, stopping early if possible
		for (int i = 0 - step; i <= step; i++) {
			for (int j = 0 - step; j <= step; j++) {
				if (abs(i) == step || abs(j) == step) { //With each step, a box expands with dimensions step x step. This if makes sure the loop is on the outermost ring of the box.
					x = p->x + j;
					y = p->y + i;
					d = dist(i, j, 0, 0); //Determine the distance from the center, where the NPC is standing, to the tile being currently checked.
					if (d < minDist) {
						if (y >= 0 && y < 3 * MAP_H) {
							if (x >= 0 && x < 3 * MAP_W) {
								for (Person* ape : map[y][x].people) {
									if (ape->eats && !ape->combat) {
										if(ape->gold >= money_needed){
											for (std::string job : jobs) {
												if (ape->job == job) {
													min = Spot(x, y);
													minDist = d;
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return min;
}

Spot findUtility(std::vector<std::vector<Tile>>& map, Person* p, int range = 12) {
	int minDist = 99;
	int step = -1;
	int d = 99;
	int x, y;
	Spot min = Spot(-1, -1);

	//Find water
	while (minDist == 99 && ++step < range) { // Iterative Deepening Search, will search 1 layer at a time, expanding out to 10 layers, stopping early if possible
		for (int i = 0 - step; i <= step; i++) {
			for (int j = 0 - step; j <= step; j++) {
				if (abs(i) == step || abs(j) == step) { //With each step, a box expands with dimensions step x step. This if makes sure the loop is on the outermost ring of the box.
					x = p->x + j;
					y = p->y + i;
					d = dist(i, j, 0, 0); //Determine the distance from the center, where the NPC is standing, to the tile being currently checked.
					if (d < minDist) {
						if (y >= 0 && y < map.size()) {
							if (x >= 0 && x < map[0].size()) {
								if (map[y][x].utilities.size() > 0) {
									min = Spot(x, y);
									minDist = d;
								}
							}
						}
					}
				}
			}
		}
	}
	return min;
}
