#pragma once
#include "classes.h"

std::string genFirstName(bool man) {
	std::vector<std::string> names;
	if (man) { //male name
		names = { "albert", "arthur", "andrew", "alfred", "alex", "alexander", "anthony", "alvin", "albin", "adam", "aaron", "anton", "bernard", "cornelius", "cyrus", "cyrin", "ceres", "cerin", "chris", "christopher", "claude", "claud", "charles",
		"curtis", "clarence", "cecil", "david", "dmitri", "derek", "dylan", "daniel", "edwin", "edward", "edmund", "emeret", "eric", "erin", "elrin", "erwin", "eli", "elijah", "everest", "edgar", "elmer", "edward", "frank", "franklin","francis",
		"george", "glenn", "gerald", "hubert", "hiram", "homer", "harry", "henry", "irving", "isaac", "irvin", "ian", "isaiah", "ivan", "ivar", "igor", "iam", "ismael", "julian", "jack", "jerin", "jeffrey", "jeff", "jebediah", "joseph", "jon",
		"john", "josh", "joshua", "james", "jaime", "jacob", "jeremiah", "jonah", "kane", "koen", "kain", "liam", "luke", "levi", "lucas", "lev", "levon", "levan", "lyonel", "luis", "leon", "leo", "leonardo", "luca", "matthew",
		"michael", "martin", "morgan", "matias", "marco", "mark", "mithras", "max", "maxwell", "miles", "nathaniel", "nikola", "nikolas", "naveed", "neil", "nathan", "ossip", "orion", "oscar", "octavian", "osiris", "odin", "orrin", "orlan", "orson",
		"omar", "otto", "paul", "pierre", "percy", "perseus", "pierce", "preston", "peter", "quentin", "quinton", "raul", "roman", "richard", "ronald",	"robert", "russel", "sam", "samuel", "saul", "steven", "sullivan", "sean", "solomon",
		"theodore", "theo", "troy", "trey", "trenton", "timothy", "terrin", "thaddeus", "terrance", "tristian", "thomas", "ulysses", "urist", "vernon", "vincent", "virgil", "victor", "von", "william", "will", "wilmer", "wilbur", "walter",
		"waylon", "warren", "wyatt", "xavier", "xander", "xerxes", "yuri", "yasir", "zach", "zachary", "zacharias", "zeke", "zebediah" };
	}
	else { //female name
		names = { "aiden", "alexis", "amanda", "adeline", "adelade", "anna", "anette", "alice", "alicia", "ann", "annie", "ada", "aris", "ares", "alma", "amelia", "amy", "august", "avelyn", "abbie", "beth", "beatrice", "belle",
		"barbara", "bella", "brook", "brianna", "beatrix", "brenda", "beatriz", "cora", "clementine", "carla", "chloe", "coraline", "caroline", "carol", "cecilia", "catherine", "catarina", "charlotte", "clara", "daisy", "diana", "darla", "dina",
		"dorothy", "dana", "delilah", "delia", "eleanor", "eileen", "eris", "erica", "emma", "elizabeth", "ella", "eve", "eva", "edna", "ellen", "emily", "emilia", "evelyn", "eliza", "estelle", "eunice", "florence", "faith", "fiona", "flora",
		"francisca", "frida", "grace", "gabriella", "gina", "georgina", "gracelyn", "greta", "gloria", "gwen", "geraldine", "harriet", "henrietta", "hannah", "helen", "helena", "heidi", "heather", "haley", "isabel", "isabella", "ivy", "ida", "iva",
		"ivana", "iris", "irene", "josephine", "julia", "josie", "jackie", "juliet", "juliana", "jenna", "jenette", "jennifer", "joy", "katherine", "katelyn", "katie", "kora", "kyra", "kira", "karina", "luna", "lilian", "liliana", "lucy", "lydia",
		"leia", "lela", "lola", "leonna", "lynn", "lena", "laura", "lauren", "mary", "maria", "margaret", "martha", "mabel", "moira", "maude", "maud", "may", "mathilda", "madelyn", "marie", "myra", "nora", "nina", "nancie", "natalie", "olivia",
		"odette", "octavia", "penelope", "phoebe", "paula", "paulina", "prudence", "rose", "rebecca", "ruth", "rhea", "ruby", "susan", "sue", "sylvia", "sylva", "sofia", "sophie", "scarlett", "selene", "sadie", "teresa", "tia", "ursula",
		"uma", "vera", "veronica", "valentina", "violet", "vivian", "viola", "virginia", "victoria", "verna", "velma", "welma", "wilfred", "wendy", "winona", "yvonne", "yesenia", "zoe" };
	}
	return names[rand() % names.size()];
}

std::string genLastName() {
	std::vector<std::string> names = { "abbott", "abram", "abrams", "adams", "adair", "anders", "apple", "baker", "barclay", "barnes", "barlow", "barlet", "brown", "blevins", "bowers", "browning", "combs", "cresswell", "corley", "chester", "crabtree",
	"chappell", "cowan", "coen", "corley", "dumas", "duvall", "dallas", "daniels", "driscoll", "dawkins", "delarosa", "diehl", "downey", "dawson", "dodd", "davids", "doan", "ellwood", "edmonds", "everett", "eldridge", "everton", "evergreen",
	"eckert", "eckhart", "ebern", "elridge", "eastman", "easton", "erickson", "elrod", "emery", "east", "engles", "fallon", "ford", "fuller", "finnegan", "fink", "fitch", "felton", "flint", "ferguson", "floyd", "feraz", "garner", "garnet", "greig",
	"galves", "granger", "glass", "geiger", "godfrey", "hodges", "haywood", "harman", "harmon", "hale", "houghes", "houston", "hess", "howe", "harding", "hardy", "harlow", "irwin", "iles", "ivers", "iverson", "irving", "isaacs", "joyce", "judd",
	"jones", "jefferson", "jackson", "joyner", "jones", "kurtz", "klein", "kirk", "kalvir", "keir", "kenney", "kinnings", "key", "lewis", "lacey", "lowe", "linley", "langley", "lancaster", "leigh", "leigh", "lovell", "lee" };
	return names[rand() % names.size()];
}

std::string jobs(int skill) {
	//STRENGTH, MAGIC, RESOLVE, CHARM, PRECISION, CREATION

	int ran = rand() % 9;
	std::vector<std::string> str = { "gatherer", "gatherer" };			//Strength
	std::vector<std::string> agi = { "fisher", "hunter" };				//Agility

	std::vector<std::string> cre = { "chef", "artisan" };				//Creation
	std::vector<std::string> cha = { "gatherer", "fisher" };			//Charm
	
	std::vector<std::string> mag = { "gatherer", "fisher" };			//Magic
	std::vector<std::string> res = { "gatherer", "fisher" };			//Resolve
	
	std::vector<std::vector<std::string>> jobs = { str, agi, cre, cha, mag, res};
	switch (ran) {
	case 0: case 1: return "gatherer";
	case 2: return "artisan";
	case 3: case 4: return "chef";
	case 5: return "hunter";
	case 6: case 7: case 8: return "fisher";
	}
	return "fisher";
	

	return jobs[skill][rand() % jobs[skill].size()];
}

void JobEquip(Person* p) { //There are a lot of jobs o boy.
	int ran;
	int ind = -1;

	//name, value, blunt dmg, sharp dmg, hit chance
	if (p->job == "fisher") {
		ind = FISHING_POLE;
	}
	else {
		ind = DAGGER + rand() % 10;
	}
	p->weapon = Item(ind);
}

Resource createTree() {
	int ran = 1 + rand() % 3; //Fl Hu  Th
	Item fruit(FRUIT);
	Item stick(STICK);
	Item leaves(LEAVES);

	Resource p;
	//p->gender = (Gender)(rand() % 2);
	p.name = "Tree";
	p.color = "*DARK GREEN*";
	if (rand() % 2 == 0) {
		p.color = "*BROWN*";
	}

	//p.job = "Resource";
	//p.society = "nature";
	//p.eats = false;
	//p.sentient = false;

	p.drops.push_back(STICK);
	p.drop_rates.push_back(500);

	if (rand() % 5 == 0) { // 20% chance to be a fruit tree
		p.name = "Fruit Tree";
		switch (ran) {
		case 0: fruit.color = "*PURPLE*"; break;
		case 1: fruit.color = "*YELLOW*"; break;
		case 2: fruit.color = "*RED*"; break;
		}
		p.drops.push_back(FRUIT);
		p.drop_rates.push_back(600);
	}

	

	/*for (int i = 0; i < INVENTORY_SIZE; i++) {
		if (p.inventory[i].name == "null") {
			p.inventory[i] = stick;
		}
	}*/

	p.body.push_back(STICK);
	p.body.push_back(LEAVES);
	p.body_drops.push_back(ran);
	p.body_drops.push_back(ran);

	//setID(p);
	return p;
}

void createPerson(Person *p) {
	static int num_people = 0;
	std::cout << ++num_people << std::endl;
	int skill;
	int max_index = 0;

	p->gender = (Gender)(rand() % 2);
	p->name = genFirstName(p->gender);
	
	//Skills:
	p->skills = { 1, 1, 1, 1, 1, 1, 1, 1, 1};
	for (int i = 0; i < 7; i++) {
		skill = rand() % 9;
		p->skills[skill]++;
	}

	p->attacks.push_back(STRIKE);

	p->gold = 15 + p->skills[CHARM] * 15;

	do {
		p->outfit = Item(ELEGANT_HAT + rand() % (RUBY_NECKLACE - ELEGANT_HAT));
	} while (!items[p->outfit.i].major);
	
	std::vector<std::string> colors = { "*TAN*", "*WHITE*", "*RED*", "*ORANGE*", "*YELLOW*", "*GREEN*", "*TEAL*", "*PURPLE*", "*BROWN*" };
	if (items[p->outfit.i].can_dye) {
		p->outfit.color = colors[rand() % colors.size()];
	}

	if (p->outfit.value < p->gold) {
		do {
			p->trinket = Item(ELEGANT_HAT + rand() % (RUBY_NECKLACE - ELEGANT_HAT));
		} while (items[p->trinket.i].major);
	}
	
	
	for (int i = 0; i < p->skills.size(); i++) {
		if (p->skills[i] > p->skills[max_index]) {
			max_index = i;
		}
	}
	p->job = jobs(max_index);
	if (p->job == "chef") {
		p->gold += 30;
	}
	JobEquip(p);

	int AP = 400 + p->skills[STRENGTH] * 20 + p->skills[ENDURANCE] * 40 + p->skills[AGILITY] * 20;
	int HP = 30 + p->skills[STRENGTH] + p->skills[ENDURANCE] * 3;
	int SP = p->skills[AURA] * 2 + p->skills[AURA] * 2;

	p->spell_slots = (p->skills[FAITH] + p->skills[AURA]) / 2 + p->skills[MAGIC];
	p->rune_slots = p->skills[AURA];

	p->hp = Stat(HP, HP);
	p->sp = Stat(SP, SP);
	p->ap = Stat(AP, AP);
	
}

void createDeer(Person* p) {
	
	Item hide(ANIMAL_HIDE);
	Item meat(VENISON);

	p->gender = (Gender)(rand() % 2);
	p->name = "Deer";

	p->job = "herbivore";
	p->society = -1;
	p->eats = true;
	p->sentient = false;


	for (int i = 0; i < INVENTORY_SIZE - 3; i++) {
		p->inventory[i] = meat;
	}

	p->skills[ENDURANCE] = rand() % 4;
	p->skills[STRENGTH] = 1 + rand() % 4;

	int AP = 400 + p->skills[STRENGTH] * 20 + p->skills[ENDURANCE] * 40 + p->skills[AGILITY] * 20;
	int HP = 5 + p->skills[STRENGTH] + p->skills[ENDURANCE] * 3;

	p->hp = Stat(HP, HP);
	p->ap = Stat(AP, AP);

	
	p->attacks.push_back(KICK);

	p->outfit = hide;
}