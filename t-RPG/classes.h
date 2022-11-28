#pragma once
#include <string>
#include <vector>
#include <deque>
#include "constants.h"

struct State {
	char thirst = 0;
	char hunger = 0;
	char misery = 0;
	State() {};
	State(int th, int hu, int fl) {
		thirst = th;
		hunger = hu;
		misery = fl;
	};
	State operator+=(const State& rhs) {
		this->thirst += rhs.thirst;
		this->hunger += rhs.hunger;
		this->misery += rhs.misery;
	}
};

struct Spot {
	short x;
	short y;

	short parent = -1; //Used only for pathfinding.
	short g = -1; //Distance to start. pathfinding.
	short h = -1; //Distance to end.   pathfinding.

	Spot(int x1=0, int y1=0) {
		x = x1;
		y = y1;
	}
	bool operator==(const Spot rhs) {
		return this->x == rhs.x && this->y == rhs.y;
	}
	bool operator!=(const Spot rhs) {
		return this->x != rhs.x || this->y != rhs.y;
	}
};

enum WAY {
	in, out
};

struct Exit {
	char way[2];
	Exit(char n, char o) {
		way[0] = n;
		way[1] = o;
	}
};

struct Room {
	std::string name = "";
	char id = 0;
	Room(std::string n = "") {
		name = n;
	}
};


struct Building {
	std::string name = "";
	std::string color = "";

	int type = 0;
	int id = 0;
	std::vector<Room> rooms;
	std::vector<Exit> exits;

	Building() {};
	Building(std::string n, std::string c = "") {
		name = n;
		color = c;
	}
};

struct Box {
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	Box() {};
	Box(int x1, int y1, int width, int height) {
		x = x1;
		y = y1;
		w = width;
		h = height;
	}
};

enum FORMAT {
	LEFT, RIGHT, CENTER
};

enum TYPE { //Item Types
	MISC, RAW_FOOD, FOOD, CLOTHING, WEAPON
};

enum ITEMS {
	STICK, LEAVES, GOLD, ANIMAL_HIDE,
	RAW_FISH, RAW_TURTLE, VENISON, ONION,
	FRUIT, COOKED_FISH, COOKED_TURTLE, COOKED_VENISON, COOKED_ONION,
	ELEGANT_HAT, FINE_HAT, CAP, ELEGANT_ROBES, FINE_ROBES, ROBES, PLAIN_ROBES, ELEGANT_SUIT, FINE_SUIT, SUIT, PLAIN_SUIT, ELEGANT_CLOTHES, FINE_CLOTHES, CLOTHES, PLAIN_CLOTHES,
	COPPER_RING, SILVER_RING, GOLD_RING, RUBY_RING, COPPER_NECKLACE, SILVER_NECKLACE, GOLD_NECKLACE, RUBY_NECKLACE,
	PLAIN_CAP, HEAVY_ARMOR, MEDIUM_ARMOR, LIGHT_ARMOR, SILK_ARMOR,
	BLADE_OF_CREATOR, FISHING_POLE, DAGGER, SHORT_SWORD, MACE, AXE, SHORT_SPEAR, WAR_HAMMER, MORNING_STAR, LONGSWORD, SPEAR, GREATSWORD, MAUL
};

struct DMG {
	//Number dmg;
	char pen = 0;
	char dmg = 0;
	char hit = 0;
	DMG(char d = 0, char p = 0, char h = 0) { 
		dmg = d;
		pen = p;
		hit = h;
	}
};

struct UItem {
	std::string name = "";
	std::string color = "";
	TYPE type = MISC;
	bool body_part = false;

	short value = 0;
	short fuel = 0;
	bool can_dye = false;

	//Clothes stats --------------------------------------------------
	bool major = true; //Whether this is a major or minor item
	DMG def;

	//Food stats ------------------------------------------------------
	short hunger = 0;
	short thirst = 0;
	short flavor = 0;
	short cook = -1;

	//Weapon stats -----------------------------------------------------
	DMG dmg;
	int size = 0;

	UItem(std::string n, int v, DMG d = DMG(), std::string col = "", bool m = true) {
		type = CLOTHING;
		name = n;
		value = v;

		color = col;
		if (col == "") {
			can_dye = true;
		}
		def = d;

		major = m;
	}

	UItem(std::string n, int v, int f, int h, int t, TYPE t1, std::string col = "", int cook_into = -1) {
		type = t1;
		name = n;
		value = v;
		flavor = f;
		hunger = h;
		thirst = t;
		color = col;
		cook = cook_into;
	}

	UItem(std::string n, int v, DMG d, int s) {
		type = WEAPON;
		name = n;
		value = v;
		dmg = d;
		size = s;
	}
	UItem(std::string n, int v, TYPE t, std::string col = "", int f = 0) {
		name = n;
		value = v;
		type = t;
		fuel = f;
		color = col;
	}
	UItem() {
	};
};

struct Item {
	std::string name = "";
	std::string color = "";

	short building = -1;
	short room = -1;

	bool sale = false; //mark an item for sale by NPCs
	short i = 0; //index of item
	short value = 0;

	Item(int ind = -1) {
		if (ind == -1) {
			i = 0;
			name = "null";
		}
		else {
			i = ind;
			color = items[i].color;
			name = items[i].name;
			value = items[i].value;
		}
	}
};


struct Pair {
	std::string name = "";
	int value = 0;
	Pair(std::string n = "", int v = 0) {
		name = n;
		value = v;
	}
	bool operator < (const Pair rhs) {
		return this->value < rhs.value;
	}
	bool operator <= (const Pair rhs) {
		return this->value <= rhs.value;
	}
	bool operator > (const Pair rhs) {
		return this->value > rhs.value;
	}
	bool operator >= (const Pair rhs) {
		return this->value >= rhs.value;
	}
};

struct Stat {
	int val = 0;
	int max = 0;
	Stat() {};
	Stat(int v, int m) {
		val = v;
		max = m;
	}
};

enum Gender {
	WOMAN, MAN
};

enum SKILL {
	STRENGTH, ENDURANCE, AGILITY, FAITH, MAGIC, AURA, CHARM, CREATION, MELEE
	/*<---------------[ Body ]---------------->
		Strength - Health, Damage on Attacks
	
		Agility - Increases chance to hit, chance to dodge
	
	<-----------------[ Mind ]---------------->
		Creation - Improves your ability to create things/cook etc
	
		Charm - Improves your prices and interactions with others

	<-----------------[ Soul ]---------------->
		Magic - Magical Attacks, Mana

		Resolve - Shield, Healing, Blessings
	*/
};


enum Direction {
	HERE, NORTH, WEST, EAST, SOUTH
};

struct Attack {
	std::string name = "";

	char base_cost = 100;
	char weight_cost = 10;

	char targets = 1;

	int hit = 0;
	short base = 0;
	float pen = 1.0;
	float dmg = 1.0;
	

	Attack(std::string n, short APcost, short WEIGHTcost, int hitChance, float DMG_multiplier, float PEN_multiplier = 0, short base_dmg = 0, char num_targets = 1) {
		name = n;
		base_cost = APcost;
		weight_cost = WEIGHTcost;
		hit = hitChance;
		dmg = DMG_multiplier;
		pen = PEN_multiplier;
		targets = num_targets;
		base = base_dmg;
	}
};

struct Number {
	char base = 0;
	//              4  6  8  10 12 14
	char die[6] = { 0, 0, 0, 0, 0, 0 };
	char num = 6; //How many die there are
	float mult = 1.0;
	Number operator+=(const Number& rhs) {
		this->base += rhs.base;
		for (int i = 0; i < this->num; i++) {
			this->die[i] += rhs.die[i];
		}
	}
};

struct Spell {
	std::string name = "blah";
	std::string description = "";
	int energy = 0;
	int chance = 10;

	char targets = 1;

	Spell(std::string n = "") {
		name = n;
	}
};

struct Action {
	std::string name = "";
	int energy = 0;
	int chance = 10;
	Action(std::string n = "", int e = 0) {
		name = n;
		energy = e;
	}
};

struct Relation {
	int id = 0;
	bool active = true;
	int value = 0;
	int chat_desire = 0;
	Relation(int id1, int val, int convo = 0) {
		id = id1;
		value = val;
		chat_desire = convo;
	}
};

struct Resource {
	std::string name = "";
	std::string color = "";

	std::vector<short> drop_rates;
	std::vector<short> drops;

	std::vector<short> body;
	std::vector<short> body_drops;

	bool drops_when_hit = true;
};

struct Person {
	int society;
	std::string name;
	std::string job;
	int id;
	int age = 20;

	std::deque<Spot> recent;
	std::deque<Spot> path;

	Direction move = HERE;

	std::vector<int> masters = {};
	std::vector<int> prisoners = {};
	std::vector<Relation> relations = {};
	

	short building = -1;
	short room = -1;

	bool sentient = true;
	bool eats = true;
	bool combat = false;
	bool dead = false;
	bool yield = false;
	int acted = -1;
	int chat_desire = 50;

	Spot home = Spot(0,0);

	int x = 0;
	int y = 0;

	Gender gender;

	std::vector<int> skills = { 0, 0, 0, 0, 0, 0, 0, 0, 0};

	short rune_slots = 0;
	short spell_slots = 0;

	std::vector<short> attacks = {}; //movest of ape
	std::vector<short> spells = {};
	std::vector<short> actions = {};

	Item inventory[INVENTORY_SIZE] = {};
	Item weapon;
	Item outfit;
	Item trinket;

	Stat hp = Stat(25, 50);
	Stat ap = Stat(25, 25);
	Stat sp = Stat(10, 10);

	int gold = 0;

	State state;
};

enum Level {
	BRAWL, IMPRISON, KILL
};

struct CombatHandler {
	std::vector<Person*> allies;
	std::vector<Person*> enemies;
	std::vector<std::string> log;

	std::string message = "";

	Level level;

	bool allies_defending = true;

	short turn = 0;
	short last_turn = -1;

	bool operator==(const CombatHandler& rhs) {
		short size = this->allies.size();
		bool condition = size == rhs.allies.size() && this->enemies.size() == rhs.enemies.size();
		if (condition) {
			for (int i = 0; i < size; i++) {
				if (this->allies[i]->id != rhs.allies[i]->id) {
					condition = false;
				}
			}
			if (condition) {
				size = this->enemies.size();
				for (int i = 0; i < size; i++) {
					if (this->enemies[i]->id != rhs.enemies[i]->id) {
						condition = false;
					}
				}
			}
		}
		return condition;
	}

	bool operator!=(const CombatHandler& rhs) {
		return !(*this == rhs);
	}
};

struct Utility {
	std::string name = "";
	std::string color = "";
	int fuel = 0;
	bool active = false;
	Utility(std::string n, int f, bool a = true) {
		name = n;
		fuel = f;
		active = a;
	}
};

//I need to remove strings from a lot of these structs in order to make the map data much smaller

struct Tile {
	char type = 'g';
	std::vector <Person*> people;
	std::vector <Resource> res;

	std::vector<Utility> utilities;

	std::vector<Item> items;
	std::vector<CombatHandler> fights;

	std::vector<Building> buildings;

	short town = 0;
	int x = 0;
	int y = 0;
	char water[5] = {'e', 'e', 'e', 'e', 'e' }; //Here, North, W, E, S
	Tile() {};
};

struct Effect {
	std::vector<int> skill_modifiers = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Stat hp = Stat(25, 50);
	Stat ap = Stat(25, 25);
	Stat sp = Stat(0, 0);
	short base_duration = 0;
	short duration = 0;
};

struct TextPair {
	std::string text = "";
	int lines = 0;
	TextPair(std::string t, int l = 1) {
		text = t;
		lines = l;
	}
};

struct TradeHandler {
	bool active = false;
	bool giving = false;
	Person* p1;
	Person* p2;
	int gold = 0;
	bool sell[INVENTORY_SIZE] = { false, false, false, false, false, false, false, false, false };
	bool buy[INVENTORY_SIZE] = { false, false, false, false, false, false, false, false, false };

	std::string msg = "";
};

struct Conversation {
	bool active = false;
	Person* you;
	Person* target;
	std::string statement = "Hello.";
	int page = 0;
	int gift_value = 0;
	Conversation() {
		active = false;
		you = nullptr;
		target = nullptr;
		statement = "Hello.";
		page = 0;
		gift_value = 0;
	}
};

