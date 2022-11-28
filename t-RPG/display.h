#pragma once
#include "classes.h"

olc::Pixel GetColor(std::string text, int timer) {
	static int time = 0;
	static int RGB[3] = { 0, 0, 0 };

	std::string t = low(text);

	if (t == "red") {
		return RED;
	}
	if (t == "orange") {
		return ORANGE;
	}
	if (t == "yellow" || t == "gold") {
		return YELLOW;
	}
	if (t == "green") {
		return GREEN;
	}
	if (t == "teal") {
		return TEAL;
	}
	if (t == "blue") {
		return BLUE;
	}
	if (t == "purple") {
		return PURPLE;
	}
	if (t == "grey") {
		return GREY;
	}
	if (t == "brown") {
		return BROWN;
	}
	if (t == "grass") {
		return GRASS;
	}
	if (t == "pink") {
		return PINK;
	}
	if (t == "fish") {
		return FISH;
	}
	if (t == "" || t == "white") {
		return WHITE;
	}
	if (t == "tan") {
		return TAN;
	}
	if (t == "dark green") {
		return DARK_GREEN;
	}
	if (t == "bg_grey") {
		return bg_grey;
	}
	if (t == "fire") {
		int R = 255;
		if (timer != time) {
			time = timer;
			for (int i = 0; i < 3; i++) {
				RGB[i] += rand() % 11 - 5;
				if (i == 1) {
					RGB[i] += rand() % 21 - 10;
				}
				RGB[0] = max(235, min(255, RGB[0])); // R
				RGB[1] = max(0, min(80, min(RGB[0]-30, RGB[1]))); //G
				RGB[2] = max(0, min(80, RGB[2])); // B
			}
		}
		olc::Pixel P(RGB[0], RGB[1], RGB[2]);
		return P;
	}
	std::cout << t << std::endl;
	return WHITE;
}

std::string Print_DMG(Number n) {
	std::string s = "";
	std::vector<std::string> colors = {"*D4*", "*D6*", "*D8*", "*D10*", "*D12*", "*D14*"};
	std::string color = "";
	bool prevTerm = false;
	if (n.mult != 1.0) {
		s += to_str(n.mult) + " * (";
	}
	if (n.base > 0) {
		s += to_str(n.base);
		prevTerm = true;
	}
	for (int i = 0; i < n.num; i++) {
		//4, 6, 8, 10, 12, 14
		color = colors[i];
		if (n.die[i] > 0) {
			if (prevTerm) {
				s += " + ";
			}
			s += color + "d" + to_str(4 + i * 2) + color;
		}
	}
	return s;
}



struct Item_Printable {
	std::string name = "";
	int value = 0;

	int max_hp = 0;
	int hp = 0;
};

std::string itemPrint(std::vector<Item> list, bool numbers = true, std::string indent = "") {
	std::string s = "";
	std::string line = "";
	std::string weight = "";
	for (Item t : list) {
		line = indent + t.color + t.name + t.color;
		if (numbers) {
			int len = measureText(line);
			for (int i = 0; i < (20 - len); i++) {
				line += " ";
			}
			line += "*YELLOW*" + to_str((int)(t.value)) + "*YELLOW*";
		}
		s += line + "\n";
	}
	return s;
}

void StatStrings(Person* p, std::string& hp, std::string& sp, std::string& ap) { //scale is how much a pip represents
	hp = "[*RED*HP*RED*]:*RED*";
	ap = "[*GREEN*AP*GREEN*]:*GREEN*";
	std::string color = "";
	int step = 0;
	bool colorChange = false;
	int stat = 0;
	std::string text = "";

	for (int a = 0; a < 3; a++) {
		if (a == 0) {
			text = "[*RED*HP*RED*]:";
			stat = p->hp.val;
			step = p->hp.max / 18;
			color = "*RED*";
		}
		if (a == 1) {
			text = "[*RED*SP*RED*]:";
			stat = p->sp.val;
			step = p->sp.max / 18;
			color = "*BLUE*";
		}
		if (a == 2) {
			text = "[*GREEN*AP*GREEN*]:";
			stat = p->ap.val;
			step = p->ap.max / 18;
			color = "*GREEN*";
		}
		text += color;
		for (int i = 0; i < 18; i++) {
			if (!colorChange) {
				if (stat < i * step) {
					text += color + "*GREY*";
				}
			}
			text += "#";
		}

		if (!colorChange) { text += color; }
		else { text += "*GREY*"; }

		text += " " + color + to_str(stat) + color;

		switch(a) {
		case 0: hp = text; break;
		case 1: sp = text; break;
		case 2: ap = text; break;
		}
	}
}

std::string dirName(Direction dir) {
	std::string s = "";
	switch (dir) {
	case HERE: s = "here"; break;
	case NORTH: s = "north"; break;
	case SOUTH: s = "south"; break;
	case WEST: s = "west"; break;
	case EAST: s = "east"; break;
	}
	return "*TEAL*" + s + "*TEAL*";
}

std::string opinionColor(int val) {
	if (val < -50) {
		return "*RED*";
	}
	if (val < -25) {
		return "*ORANGE*";
	}
	if (val > 25) {
		return "*TEAL*";
	}
	if (val > 50) {
		return "*GREEN*";
	}
	return "*GREY*";
}

std::string typeColor(std::string word, TYPE t) {
	//MISC, RAW_FOOD, FOOD, CLOTHING, WEAPON
	std::vector<std::string> colors = {"*GREY*", "*RED*", "*GRASS*", "*TEAL*", "*ORANGE*"};
	return colors[t] + word + colors[t];
}

std::string clothesName(Person* p, std::string add = "") {
	std::string clothes = "";
	clothes = "*TAN*";
	if (p->outfit.name != "null") {
		clothes = p->outfit.color;
	}
	return clothes + p->name + add + clothes;
}

void sizeComparison(Person* p) {
	int string_size = sizeof(p->society) + sizeof(p->name) + sizeof(p->job);
	int recent_size = sizeof(p->recent);

	int inventory_size = sizeof(p->inventory);
	int clothes_size = sizeof(p->outfit) + sizeof(p->trinket) + sizeof(p->weapon);

	int stat_size = sizeof(p->skills) + sizeof(p->hp) + sizeof(p->ap) + sizeof(p->sp) + sizeof(p->gold) + sizeof(p->state);
	int property_size = sizeof(p->gender) + sizeof(p->x) + sizeof(p->y) + sizeof(p->home) + sizeof(p->sentient) + sizeof(p->eats) + sizeof(p->acted);

	int attack_size = sizeof(p->attacks);

	int total_size = string_size + recent_size + inventory_size + clothes_size + stat_size + property_size + attack_size;

	std::cout << "Strings: " + to_str(recent_size) << std::endl;
	std::cout << "Recent tiles: " + to_str(recent_size) << std::endl;
	std::cout << "Inventory: " + to_str(inventory_size) << std::endl;
	std::cout << "Clothes: " + to_str(clothes_size) << std::endl;
	std::cout << "Stats: " + to_str(stat_size) << std::endl;
	std::cout << "Misc. Properties: " + to_str(property_size) << std::endl;
	std::cout << "Attacks: " + to_str(attack_size) << std::endl;
	std::cout << "Total: " + to_str(total_size) << std::endl;
}

std::string relationName(Person* p, Person* obs, std::string add = "") {
	std::string clothes = "";
	std::string col = "";
	std::string name = "";
	col = "*GREY*";
	//Look for person in player's relations:
	name = p->name;
	for (Relation r : p->relations) {
		if (r.id == obs->id) {
			col = opinionColor(r.value);
			break;
		}
	}
	clothes = "*TAN*";
	if (p->outfit.name != "null") {
		clothes = p->outfit.color;
	}
	return col + "# " + col + clothes + name + clothes + " - *GREY*" + p->job + add + "*GREY*";
}

std::string Look(Person* p, Tile t, int people_page = 1, int item_page = 1) {
	std::string s = "";
	
	std::string clothes = "";
	std::string col = "*GREY*";
	std::string last = "";
	std::string name = "";

	int counter = 0;

	//Describe water on tile.
	if (t.water[HERE] != 'e') {
		s += "There's water here. \n";
	}
	if (t.water[NORTH] != 'e') {
		s += "Theres water to the north. \n";
	}
	if (t.water[WEST] != 'e') {
		s += "There's water to the west. \n";
	}
	if (t.water[EAST] != 'e') {
		s += "Theres water to the east. \n";
	}
	if (t.water[SOUTH] != 'e') {
		s += "There's water to the south. \n";
	}
	//Describe items on the tile.
	counter = 0;
	s += "\n[Items]: Page " + to_str(item_page) + "/" + to_str(1 + t.items.size()/8) + "\n";
	for (Item it : t.items) {
		s += to_str(++counter) + " ";
		s += it.color + it.name + it.color + "\n";
	}

	//Describe people on the tile.
	counter = 0;
	s += "\n[People]: Page " + to_str(people_page) + "/" + to_str(1 + t.people.size() / 8) + "\n";
	for (Person* ape : t.people) {
		if (ape != p && ape->eats) {
			s += char('a' + counter++);
			s += " " + relationName(ape, p);
			if (ape->move != HERE) {
				s += " > " + dirName(ape->move);
			}
			s += "\n";
		}
	}

	if (s == "") {
		s = "There's nothing here. ";
	}

	return s;
}

std::string terraName(char c) {
	switch (c) {
	case 'w':
	case 's': return "the Ocean";
	case 'r': return "a River";
	case 'g': return "a Grassland";
	case 'l': return "a Meadow";
	case 'f': return "a Forest";
	case 'm': return "a Mountain";
	case 'h': return "a Hill";
	case 't': return "a Town";
	default: return "" + c;
	}
}

std::string drawTrade(TradeHandler trade) {
	std::string s = "[*GREEN*" + trade.p1->name + "*GREEN* - *YELLOW*" + to_str(trade.p1->gold) + " gold*YELLOW*]: \n\n";
	int counter = 1;
	std::string col = "";

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (trade.p1->inventory[i].name != "null") {
			col = "*GREY*";
			if (trade.sell[i]) {
				col = "*GREEN*";
			}
			s += to_str(counter++) + col + " # " + col;
			s += trade.p1->inventory[i].color + trade.p1->inventory[i].name + trade.p1->inventory[i].color + "\n";
		}
	}
	s += "\n";
	s += "[*TEAL*" + trade.p2->name + "*TEAL* - *YELLOW*" + to_str(trade.p2->gold) + " gold*YELLOW*]: \n\n";
	counter = 0;
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (trade.p2->inventory[i].name != "null") {
			col = "*GREY*";
			if (trade.buy[i]) {
				col = "*GREEN*";
			}
			if (!trade.giving) {
				s += char('a' + counter++);
			}
			s += col + " # " + col;
			s += trade.p2->inventory[i].color + trade.p2->inventory[i].name + trade.p2->inventory[i].color + "\n";
		}
	}
	s += "\n";

	appraiseTrade(trade);
	col = "*GREY*";
	if (trade.gold > 0) {
		col = "*GREEN*";
	}
	else if (trade.gold < 0) {
		col = "*RED*";
	}
	if (!trade.giving) {
		s += "*YELLOW*Gold*YELLOW*: " + col + to_str(trade.gold) + col + "\n";
	}
	else {
		s += "*TEAL*Opinion*TEAL*: " + col + to_str(trade.gold) + col + "\n";
	}
	s += trade.msg;

	return s;
}

std::string drawTalk(Conversation talk) {
	std::string s = "a";
	Relation opinion = Relation(talk.you->id, 0, 0);
	for (Relation r : talk.target->relations) {
		if (r.id == talk.you->id) {
			opinion = r;
			s = "";
			break;
		}
	}
	int chat_desire = opinion.chat_desire + talk.target->chat_desire;
	if (s == "a") {
		talk.target->relations.push_back(opinion);
		s = "";
	}
	if (talk.page == 0) {
		s += relationName(talk.target, talk.you) +": \"" + talk.statement + "\"\n";
		s += "Conversation Desire: " + opinionColor(chat_desire) + to_str(chat_desire) + opinionColor(chat_desire) + "\n\n";
		if (chat_desire > 0) {
			s += "*GREEN*<--*GREEN* *GOLD*STATEMENTS*GOLD* *GREEN*-->*GREEN*\n";
			s += "*GREY*1)*GREY* *GREEN*Compliment*GREEN*\n";
			s += "*GREY*2)*GREY* *RED*Insult*RED*\n";
			s += "*GREY*3)*GREY* *GOLD*Trade*GOLD*\n";
			s += "*GREY*4)*GREY* *FISH*Give*FISH* *GREEN*Gift*GREEN*\n";
			s += "*GREY*5)*GREY* End Conversation\n\n";

			s += "*GREEN*<--*GREEN* *GOLD*QUESTIONS*GOLD* *GREEN*-->*GREEN*\n";
			s += "*GREY*A)*GREY* *FISH*Ask*FISH* about a *PURPLE*place*PURPLE*. . .\n";
			s += "*GREY*B)*GREY* *FISH*Ask*FISH* about a *TEAL*person*TEAL*. . .\n";
			s += "*GREY*C)*GREY* *FISH*Ask*FISH* them to *ORANGE*lead*ORANGE* *GREEN*you*GREEN* *PURPLE*somewhere*PURPLE*. . .\n";
			s += "*GREY*D)*GREY* *FISH*Ask*FISH* them to *ORANGE*join*ORANGE* *GREEN*you*GREEN*.\n";
		}
		else {
			s += "*GREY*1)*GREY* *ORANGE*End Conversation*ORANGE*\n\n";
		}
	}
	return s;
}

std::string colorize(int num, bool color = false) { //Takes a num 0 to 100 and applies a color scale
	std::string col = "";
	if (num <= 20) {
		col = "*GREEN*";
	}
	else if (num <= 40) {
		col = "*GREY*";
	}
	else if (num <= 60) {
		col = "*ORANGE*";
	}
	else if (num <= 80) {
		col = "*YELLOW*";
	}
	else {
		col = "*RED*";
	}
	if (color)
		return col;
	return col + to_str(num) + col;
}