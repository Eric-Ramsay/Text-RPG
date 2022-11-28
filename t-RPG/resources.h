#pragma once

#include "classes.h";

//enum TYPE { //Item Types
//	OTHER, RAW_FOOD, FOOD, CLOTHES, WEAPON
//};

//Item list can be found on classes.h

std::vector<UItem> UItem_List() {
	std::vector<UItem> items;
	//misc stuff
	items.push_back(UItem("stick", 4, MISC, "*brown*", 20));
	items.push_back(UItem("leaves", 3, MISC, "*green*", 10));
	items.push_back(UItem("gold", 3, MISC, "*gold*"));
	items.push_back(UItem("animal hide", 15, MISC, "*brown*"));
	//raw 
	items.push_back(UItem("raw fish", 6, -2, 7, 0, RAW_FOOD, "*fish*", COOKED_FISH));
	items.push_back(UItem("raw turtle", 5, -3, 6, 0, RAW_FOOD, "*grass*", COOKED_TURTLE));
	items.push_back(UItem("raw venison", 12, -2, 8, -1, RAW_FOOD, "*red*", COOKED_VENISON));
	items.push_back(UItem("wild onion", 9, 2, 5, 0, RAW_FOOD, "*purple*", COOKED_ONION));
	//FOOD
	items.push_back(UItem("fruit", 10, 7, 22, 7, FOOD, "*red*"));
	items.push_back(UItem("fish", 13, -2, 28, 0, FOOD, "*fish*"));
	items.push_back(UItem("turtle", 10, -3, 24, 0, FOOD, "*grass*"));
	items.push_back(UItem("venison", 16, 10, 36, -1, FOOD, "*red*"));
	items.push_back(UItem("onion", 9, 6, 7, 0, FOOD, "*purple*"));

	//clothes
	items.push_back(UItem("elegant hat", 30, DMG(), "", false));
	items.push_back(UItem("fine hat", 15, DMG(), "", false));
	items.push_back(UItem("cap", 10, DMG(), "", false));

	items.push_back(UItem("elegant robes", 60));
	items.push_back(UItem("fine robes", 45));
	items.push_back(UItem("robes", 30));
	items.push_back(UItem("plain robes", 15));

	items.push_back(UItem("elegant suit", 75));
	items.push_back(UItem("fine suit", 60));
	items.push_back(UItem("suit", 45));
	items.push_back(UItem("plain suit", 30));

	items.push_back(UItem("elegant clothes", 45));
	items.push_back(UItem("fine clothes", 30));
	items.push_back(UItem("clothes", 15));
	items.push_back(UItem("plain clothes", 10));

	items.push_back(UItem("copper ring", 5, DMG(), "*orange*", false));
	items.push_back(UItem("silver ring", 20, DMG(), "*grey*", false));
	items.push_back(UItem("gold ring", 80, DMG(), "*yellow*", false));
	items.push_back(UItem("ruby ring", 100, DMG(), "*red*", false));

	items.push_back(UItem("copper necklace", 5, DMG(), "*orange*", false));
	items.push_back(UItem("silver necklace", 20, DMG(), "*grey*", false));
	items.push_back(UItem("gold necklace", 80, DMG(), "*yellow*", false));
	items.push_back(UItem("ruby necklace", 100, DMG(), "*red*", false));

	items.push_back(UItem("plain cap", 5, DMG(), "*brown*", false));

	items.push_back(UItem("heavy armor", 80, DMG(1, 3), "*grey*"));
	items.push_back(UItem("medium armor", 40, DMG(1, 2), "*orange*"));
	items.push_back(UItem("light armor", 20, DMG(1, 1), "*brown*"));
	items.push_back(UItem("silk armor", 60, DMG(0, 3), "*purple*"));

	//weapons/tools
	items.push_back(UItem("*teal*blade*teal* *grey*of the*grey* *fire*creator*fire*", 45, DMG(0, 25), -5));
	items.push_back(UItem("fishing pole", 40, DMG(1, 1), 4));

										//  Cost        D  P  H     W		
	items.push_back(UItem("dagger",			15,		DMG(4, 0, 5),	1));
	items.push_back(UItem("short sword",	15,		DMG(6, 1, 5),	2));
	items.push_back(UItem("mace",			15,		DMG(6, 6, 4),	3));
	items.push_back(UItem("axe",			15,		DMG(7, 3, 5),	3));
	items.push_back(UItem("short spear",	15,		DMG(8, 2, 4),	3));
	items.push_back(UItem("war hammer",		15,		DMG(7, 7, 4),	3));
	items.push_back(UItem("morning star",	15,		DMG(8, 4, 4),	4));
	items.push_back(UItem("longsword",		15,		DMG(10, 2, 6),	4));
	items.push_back(UItem("spear",			15,		DMG(10, 3, 4),	5));
	items.push_back(UItem("greatsword",		15,		DMG(10, 2, 6),	6));
	items.push_back(UItem("maul",			15,		DMG(10, 7, 5),	7));

	return items;
}

void addRoom(Building& place, int size = 1, char connection_point = 0) {

}

Building createHouse(int size = 1, std::string name = "house", std::string color = "*BROWN*") {
	int counter = 1;
	Building house(name, color);

	Room bedroom("bedroom");
	Room hallway("hallway");
	Room kitchen("kitchen");
	Room pantry("closet");
	Room parlor("parlor");
	Room upstairs("upstairs");

	house.rooms.push_back(hallway); //The Parlor, Kitchen, and Bedrooms will connect to the hall. A room can only have 4 exits.
	house.rooms.push_back(bedroom);
	house.rooms.push_back(bedroom);
	house.rooms.push_back(kitchen);
	house.rooms.push_back(pantry);
	house.rooms.push_back(parlor);

	for (int i = 0; i < house.rooms.size(); i++) {
		house.rooms[i].id = i;
	}

	house.exits.push_back(Exit(1, 0));
	house.exits.push_back(Exit(2, 0));
	house.exits.push_back(Exit(3, 0));
	house.exits.push_back(Exit(5, 0));
	house.exits.push_back(Exit(5, -1));
	house.exits.push_back(Exit(5, 4));
	house.exits.push_back(Exit(3, 4));

	/*while (counter <= size) {
		house.rooms.push_back(Room("bedroom" + to_str(counter++)));
	}*/
	return house;
}

std::vector<Building> Building_List() {
	//example small wooden house
	/*Building house("small house", "*brown*");

	Room bedroom("bedroom");
	Room hallway("hallway");
	Room kitchen("kitchen");
	Room pantry("closet");
	Room parlor("parlor");
	Room bathroom("bathroom");

	house.rooms = { bedroom, hallway, kitchen, pantry, parlor, bathroom };

	house.exits.push_back(Exit());

	house.exits.push_back(Exit("kitchen", "closet"));
	house.exits.push_back(Exit("kitchen", "hallway"));

	house.exits.push_back(Exit("bedroom", "hallway"));*/

	/*Building medium_house = small_house;
	small_house.exits.push_back(Exit("kitchen", ""));
	buildings.push_back(small_house);

	Building small_house_two = small_house;
	small_house_two.rooms.push_back(Room("master bedroom"));
	small_house_two.exits.push_back(Exit("master bedroom", "hallway"));
	buildings.push_back(small_house_two);

	medium_house.name = "house";
	medium_house.rooms.push_back(parlor);
	medium_house.exits.push_back(Exit("parlor", ""));
	medium_house.exits.push_back(Exit("parlor", "closet"));
	medium_house.exits.push_back(Exit("parlor", "hallway"));
	medium_house.rooms.push_back(Room("master bedroom"));
	medium_house.exits.push_back(Exit("master bedroom", "hallway"));
	buildings.push_back(medium_house);

	Building large_house = medium_house;
	large_house.name = "large house";
	large_house.rooms.push_back(Room("second bedroom"));
	large_house.exits.push_back(Exit("second bedroom", "hallway"));
	buildings.push_back(large_house);*/

	return buildings;
}

enum ATTACKS {
	KICK, STRIKE
};

std::vector<Attack> Attack_List() {
	std::vector<Attack> attacks;

	//(NAME, AP COST, WEIGHT COST, HIT CHANCE, DMG MULT, PEN MULT, base_dmg, NUM TARGETS)

	attacks.push_back(Attack("kick", 50, 0, 5, 0, 0, 5, 1));

	attacks.push_back(Attack("strike", 50, 10, 5, 1, 1, 0, 1));

	return attacks;
}

std::vector<Spell> Spell_List() {
	std::vector<Spell> spells;

	return spells;
}

