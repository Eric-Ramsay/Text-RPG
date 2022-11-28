#define OLC_PGE_APPLICATION
#include "constants.h"
#include "classes.h"
#include "functions.h"
#include "movement.h"
#include "resources.h"
#include "display.h"
#include "create.h"
#include "map.h"
#include "actions.h"
#include "ai.h"

class RPG : public olc::PixelGameEngine
{
public:
	RPG()
	{
		sAppName = "Text RPG";
	}

private:
	olc::Sprite* fontSprite; olc::Decal* font;
	olc::Sprite* background;
	int timer = 0;
	float fTargetFrameTime = 1.0f / 60.0f; // Virtual FPS of 60fps
	float fAccumulatedTime = 0.0f;

	//Stuff
	Person* p;
	TradeHandler trade;
	Conversation talk;
	CombatHandler *combat = nullptr;
	int backspace = 0;

	bool viewMap = false;

	std::vector<Item> clothes;

	int death_counter = 0;
	int turn_counter = 0;
	int ape_counter = 0;
	float avg_death_tick = 0;

	//UI stuff
	//Left Bar
	bool leftSelecting = false;
	short leftUI[5] = {1, 1, 1, 1, 1 };
	int leftSelected = 0;
	//Right Bar
	bool rightSelecting = false;
	short rightSelected = -1;
	//Combat
	short combat_topic = 0;
	short combat_selected = 0;

	int reticle = -1;
	bool allies_selected = true;
	std::vector<short> ally_targets = {};
	std::vector<short> enemy_targets = {};


	std::vector<std::vector<Tile>> map;

	uint8_t bgLayer;
	uint8_t textLayer;

	//UI variables
	int INPUT_LINE = 263;
	int STATUS_LINE = 2;
	std::vector<std::string> log = {""};
	std::string last_input = "";
	std::string input = "";
	int xView = 0;
	int yView = 0;
	std::string topic = "inventory";

	Box leftbar = Box(1, 1, 117, 268);
	Box midbar = Box(119, 1, 194, 260);
	Box input_line = Box(119, 262, 194, 7);
	Box mapbar = Box(314, 1, 165, 85);
	Box sidebar = Box(314, 95, 165, 171);
	Box screen = Box(0, 0, 480, 270);
	Box combat_screen = Box(99, 1, 282, 268);

bool OnUserCreate() override
{
	items = UItem_List();
	buildings = Building_List();
	attacks = Attack_List();
	Tile til;
	//std::cout << sizeof(til) << std::endl;
	//Seed Random # Generator:
	srand((unsigned int)time(NULL));

	//Load IMGs
	background = new olc::Sprite("./Sprites/bg.png");
	DrawSprite(olc::vf2d(0, 0), background);
	fontSprite = new olc::Sprite("./Sprites/font.png");
	font = new olc::Decal(fontSprite);
	//Performance
	std::vector <Person*> people;
	std::vector <Resource> res;
	std::vector<Utility> utilities;
	std::vector<Item> items;
	std::vector<CombatHandler> fights;
	std::vector<Building> buildings;

	std::cout << sizeof(people) << std::endl;

	//Map creation
	map = createMap();


	p = new Person();

	//createPerson(p, clothes);
	

	createPerson(p);
	p->hp.max = 450;
	p->hp.val = 450;
	p->ap.max = 900;
	p->ap.val = 9000;
	p->sp.max = 600;
	p->sp.val = 600;
	p->weapon = Item(BLADE_OF_CREATOR);
	p->trinket = Item(RUBY_RING);

	for (int i = 0; i < INVENTORY_SIZE-1; i++) {
		//p->inventory[i] = Item(rand() % MAUL);
	}

	p->job = "chef";
	p->gold = 400;
	JobEquip(p);
	Utility campfire("campfire", 50, true);
	campfire.color = "*FIRE*";


	for (int i = 0; i < map.size(); i++) {
		for (int j = 0; j < map[0].size(); j++) {
			if (map[i][j].type == 't') {
				p->y = i;
				p->x = j;
				map[i][j].people.push_back(p);
				map[i][j].utilities.push_back(campfire);
				yView = max(1, p->y / 3);
				xView = max(1, p->x / 3);
				yView = min(yView, map.size() - 1);
				xView = min(xView, map[0].size() - 1);
				p->home = Spot(p->x, p->y);
				return true;
			}
		}
	}

	return true;
}

bool OnUserUpdate(float fElapsedTime) override
{
	//Variables
	int x = 0;
	int y = 0;
	std::string crappy;

	//HANDLE TICK AT A CERTAIN FPS, Held keys ie backspace need to go here.
	fAccumulatedTime += fElapsedTime;
	if (fAccumulatedTime >= fTargetFrameTime) {
		fAccumulatedTime -= fTargetFrameTime;
		fElapsedTime = fTargetFrameTime;
		if (++timer >= 100) {
			timer = 0;
		}
		if (GetKey(olc::Key::BACK).bHeld && input != "") {
			if (++backspace >= 20 && timer % 3 == 1) {
				input = input.substr(0, input.length() - 1);
			}
		}
		else {
			backspace = 0;
		}
	}
	if (GetKey(olc::Key::M).bPressed) {
		//map = createMap();
		//map = fillMap(chunks, clothes);
	}
	//Check if player is typing/ check for player input
	if (input != "" && GetKey(olc::Key::BACK).bPressed) {
		input = input.substr(0, input.length() - 1);
	}

	if (input.length() < 80) {
		if (GetKey(olc::Key::ESCAPE).bHeld) {
			trade.active = false;
			viewMap = false;
			if (reticle != -1) {
				reticle = -1;
				ally_targets = {};
				enemy_targets = {};
				combat->message = "";
			}
		}
		for (int key = olc::Key::A; key != olc::Key::K0; key++) { //a-z
			if (GetKey((olc::Key)key).bPressed) {
				if (!trade.active) {
					input += (char)(96 + key);
				}
				else if (key <= 8){
					int counter = -1;
					for (int i = 0; i < INVENTORY_SIZE; i++) {
						if (!trade.giving && trade.p2->inventory[i].name != "null") {
							if ((char)('a' + ++counter) == (char)(96 + key)) {
								trade.buy[i] = !trade.buy[i];
							}
						}
					}
				}
			}
		}
		for (int key = olc::Key::K0; key != olc::Key::F1; key++) { //0-9
			if (GetKey((olc::Key)key).bPressed) {
				if (!trade.active) {
					input += (char)(21 + key);
				}
				else if (key <= 36) {
					int counter = -1;
					for (int i = 0; i < INVENTORY_SIZE; i++) {
						if (trade.p1->inventory[i].name != "null") {
							if ((char)('0' + ++counter) == (char)(key+20)) {
								trade.sell[i] = !trade.sell[i];
							}
						}
					}
				}
			}
		}
		for (int key = olc::Key::NP0; key != olc::Key::NP_MUL; key++) { //0-9 i think, but numpad
			if (GetKey((olc::Key)key).bPressed) {
				if (!trade.active) {
					input += (char)(key-21);
				}
				else if (key <= 78) {
					trade.buy[key-70] = !trade.buy[key-70];
				}
			}
		}
	}
	if (GetKey(olc::Key::SPACE).bPressed) {
		input += " ";
	}
	if (rightSelecting) {
		leftSelecting = false;
	}
	if (combat != nullptr) {
		if (GetKey(olc::Key::L).bPressed) {
			if ((checkCombat(*combat, &map[p->y][p->x]))) {
				for (int i = 0; i < map[p->y][p->x].fights.size(); i++) {
					for (Person* ape : map[p->y][p->x].fights[i].allies) {
						if (ape == p) {
							map[p->y][p->x].fights.erase(map[p->y][p->x].fights.begin() + i);
							combat = nullptr;
						}
					}
				}
			}
			else {
				combat->log.push_back("Can't leave while enemies are present!");
			}
		}
		if (GetKey(olc::Key::ENTER).bPressed) {
			short max_targets = 0;
			if (combat_topic == 1 || combat_topic == 0) {
				if (combat_topic == 1) { //spells
					//max_targets = spells[p->spells[combat_selected]].targets;
				}
				else { //attacks
					max_targets = attacks[p->attacks[combat_selected]].targets;
				}
				if (reticle == -1) { //target something.
					reticle = 0;
					allies_selected = false;
				}
				else {
					if (ally_targets.size() + enemy_targets.size() < max_targets) {
						if (allies_selected) {
							ally_targets.push_back(reticle);
						}
						else {
							enemy_targets.push_back(reticle);
						}
						if (ally_targets.size() + enemy_targets.size() == max_targets) {
							combat->message = "press enter to proceed.";
						}
					}
					else {
						//proceed with attack.
						std::string order = attacks[p->attacks[combat_selected]].name + " ";
						for (int i = 0; i < enemy_targets.size(); i++) {
							order += combat->enemies[enemy_targets[i]]->name;
							if (i < enemy_targets.size() - 1 || ally_targets.size() > 0) {
								order += " ";
							}
						}
						for (int i = 0; i < ally_targets.size(); i++) {
							order += combat->allies[ally_targets[i]]->name;
							if (i < ally_targets.size() - 1) {
								order += " ";
							}
						}
						bool success = false;
						combat->log.push_back(attack(p, order, combat->enemies, p, success));
						if (success) {
							combat->message = "";
							combat_selected = -1;
							ally_targets = {};
							enemy_targets = {};

							if ((checkCombat(*combat, &map[p->y][p->x], false))) {
								combat->message = "*GREEN*All enemies down! Press 'L' to leave.*GREEN*";
							}
						}
						else {
							combat->log.push_back("You can't perform that attack. Do you have enough AP?");
						}
					}
				}
			}
		}
	}
	if (combat == nullptr && GetKey(olc::Key::CTRL).bPressed) {
		rightSelecting = !rightSelecting;
		if (rightSelecting) {
			rightSelected = 0;
			if (p->inventory[rightSelected].name == "null") {
				int index = rightSelected;
				do {
					index = (index + 1) % 9;
					if (p->inventory[index].name != "null") {
						rightSelected = index;
					}
				} while (index != rightSelected);
			}
		}
	}
	else if (combat == nullptr && GetKey(olc::Key::SHIFT).bPressed) {
		leftSelecting = !leftSelecting;
		if (leftSelecting) {
			rightSelected = 0;
			if (p->inventory[rightSelected].name == "null") {
				int index = rightSelected;
				do {
					index = (index + 1) % 9;
					if (p->inventory[index].name != "null") {
						rightSelected = index;
					}
				} while (index != rightSelected);
			}
		}
	}
	if (GetKey(olc::Key::UP).bPressed) {
		if (combat != nullptr) {
			if (reticle == -1) {
				combat_selected--;
			}
			else {
				reticle++;
			}
		}
		else if (rightSelecting) {
			rightSelected--;
			if (rightSelected < 0) {
				rightSelected = 8;
			}
			if (p->inventory[rightSelected].name == "null") {
				int index = rightSelected;
				do {
					index--;
					if (index < 0) {
						index = 8;
					}
					if (p->inventory[index].name != "null") {
						rightSelected = index;
					}
				} while (index != rightSelected);
			}
		}
		else if (leftSelecting) {
			leftSelected = (leftSelected - 1);
			if (leftSelected < 0) {
				leftSelected = 4;
			}
		}
		else {
			Parse("go north");
		}
	}
	if (GetKey(olc::Key::DOWN).bPressed) {
		if (combat != nullptr) {
			if (reticle == -1) {
				combat_selected++;
			}
			else {
				reticle++;
			}
		}
		else if (rightSelecting) {
			rightSelected = (rightSelected + 1) % 9;
			if (p->inventory[rightSelected].name == "null") {
				int index = rightSelected;
				do {
					index = (index + 1) % 9;
					if (p->inventory[index].name != "null") {
						rightSelected = index;
					}
				} while (index != rightSelected);
			}
		}
		else if (leftSelecting) {
			leftSelected = (leftSelected + 1) % 5;
		}
		else {
			Parse("go south");
		}
	}
	if (GetKey(olc::Key::LEFT).bPressed) {
		if (combat != nullptr) {
			if (reticle == -1) {
				combat_topic--;
				if (combat_topic < 0) {
					combat_topic = 4;
				}
			}
			else {
				allies_selected = true;
				reticle = 0;
			}
		}
		else if (leftSelecting) {
			if (leftUI[leftSelected] > 0) {
				leftUI[leftSelected]--;
			}
		}
		else if (rightSelecting) {
			if (topic == "combat") {
				topic = "inventory";
			}
			else {
				topic = "combat";
			}
		}
		else {
			Parse("go west");
		}
	}
	if (GetKey(olc::Key::RIGHT).bPressed) {
		if (combat != nullptr) {
			if (reticle == -1) {
				combat_topic = (combat_topic + 1) % 5;
			}
			else {
				allies_selected = false;
				reticle = 0;
			}
		}
		else if (leftSelecting) {
			leftUI[leftSelected]++;
		}
		else if (rightSelecting) {
			if (topic == "combat") {
				topic = "inventory";
			}
			else {
				topic = "combat";
			}
		}
		else {
			Parse("go east");
		}
	}
	//Player pressed enter, need to parse command.
	
	bool FULL_MAP = true;
	bool NPC_TEST = true;

	if(combat == nullptr && ((!NPC_TEST && (p->move != HERE) || (NPC_TEST && GetKey(olc::Key::SHIFT).bHeld)) || ((GetKey(olc::Key::ENTER).bPressed && (p->hp.val > 0 || NPC_TEST))))){
		//Handle Player input:
		Person* ape;
		std::vector<Pair> jobs = {};
		int cooked = 0;
		int this_cook = 0;
		int hunger = 0;
		int food_held = 0;
		static int last_food_held = 0;
		if (!trade.active && !talk.active) {
			if (!NPC_TEST) {
				if (combat == nullptr) {
					if (input != "") {
						crappy = Parse(input);
						log.push_back("*GREEN*>*GREEN* " + input);
						p->acted = turn_counter + 1;
					}
				}
			}
			if (p->move == HERE) {
				last_input = input;
			}
			input = "";
			if (NPC_TEST || crappy != "" || p->move != HERE) {
				if (!NPC_TEST) {
					if (combat == nullptr) {
						log.push_back(crappy);
					}
					else {
						combat->log.push_back(crappy);
					}
				}
				this_cook = 0;
				ape_counter = 0;
				turn_counter++;
				int startX = -35;
				int startY = -25;
				int stopX = 35;
				int stopY = 25;
				if (FULL_MAP) {
					startX = 0;
					startY = 0;
					stopX = map[0].size();
					stopY = map.size();
				}
				for(int i = startY; i < stopY; i++){
					for(int j = startX; j < stopX; j++){
						if (FULL_MAP) {
							y = i;
							x = j;
						}
						else {
							y = i + p->y;
							x = j + p->x;
						}
						if (y >= 0 && y < map.size() && x >= 0 && x < map[0].size()) {
							for (int k = map[y][x].fights.size() - 1; k >= 0; k--) { //Handle fights on tile.
								bool player_found = false;
								for (int a = 0; a < map[y][x].fights[k].allies.size(); a++) {
									if (map[y][x].fights[k].allies[a] != p || NPC_TEST) {
										if (map[y][x].fights[k].turn % 2 == 0 && a == map[y][x].fights[k].turn/2) {
											map[y][x].fights[k].log.push_back(Battle(map[y][x].fights[k].allies, map[y][x].fights[k].enemies, a, map[y][x].fights[k], p));
											incrementTurn(map[y][x].fights[k]);
										}
									}
									else {
										player_found = true;
									}
								}
								if (!player_found) {
									for (int a = 0; a < map[y][x].fights[k].enemies.size(); a++) {
										if (map[y][x].fights[k].enemies[a] != p || NPC_TEST) {
											if (map[y][x].fights[k].turn % 2 != 0 && a == map[y][x].fights[k].turn/2) {
												map[y][x].fights[k].log.push_back(Battle(map[y][x].fights[k].enemies, map[y][x].fights[k].allies, a, map[y][x].fights[k], p));
												incrementTurn(map[y][x].fights[k]);
											}
										}
										else {
											player_found = true;
										}
									}
								}
								if (!player_found || NPC_TEST) { //Consider ending the fight.
									if (checkCombat(map[y][x].fights[k], &map[y][x])) {
										map[y][x].fights.erase(map[y][x].fights.begin() + k);
									}
								}
							}
							for (int a = map[y][x].res.size() - 1; a >= 0; a--) {
								for (int k = 0; k < map[y][x].res[a].drops.size(); k++) {
									if (rand() % map[y][x].res[a].drop_rates[k] == 0) {
										map[y][x].items.push_back(Item(map[y][x].res[a].drops[k]));
										if ((x == p->x && y == p->y)) {
											if (!NPC_TEST || log.size() < 2) {
												crappy = an(map[y][x].items.back().name) + map[y][x].items.back().name + " drops!";
												log.push_back(crappy);
											}
										}
									}
								}
							}
							for (int a = 0; a < map[y][x].utilities.size(); a++) {
								crappy = handleFire(map[y][x].utilities[a]);
								if (!NPC_TEST && (x == p->x && y == p->y)) {
									log.push_back(crappy);
								}
							}
							for (int a = 0; a < map[y][x].people.size(); a++){
								ape = map[y][x].people[a];
								if (ape->hp.val <= 0 && !(ape->combat)) {
									if (ape != p) {
										if (ape->sentient) {
											avg_death_tick = avg_death_tick * death_counter + turn_counter;
											death_counter++;
											avg_death_tick /= (float)death_counter;
										}
										Death(&map[y][x], map[y][x].people[a]);
									}
								}
								else if (ape->acted != turn_counter) {
									ape->acted = turn_counter;
									if (ape->sentient) { //Collect debug info
										bool found = false;
										for (Pair& job : jobs) {
											if (job.name == ape->job) {
												job.value++;
												found = true;
												break;
											}
										}
										if (!found) {
											jobs.push_back(Pair(ape->job, 1));
										}
										ape_counter++;
									}
									if (ape->eats) {
										if (turn_counter % 3) {
											ape->state.thirst++;
											if (turn_counter % 6) {
												ape->state.hunger++;
											}
										}
										if (ape->sentient) {
											hunger += ape->state.hunger;
										}
										if (!ape->combat) {
											if (ape->ap.val < ape->ap.max) {
												ape->ap.val = ape->ap.max;
											}
											if (!(ape->state.hunger >= 100) && !(ape->state.thirst >= 100)) {
												if (ape->hp.val < ape->hp.max) {
													ape->hp.val++;
												}
											}
											else {
												ape->hp.val--;
												ape->state.hunger = min(ape->state.hunger, 100);
												ape->state.thirst = min(ape->state.thirst, 100);
											}
										}
										if (!ape->dead && ape->hp.val > 0) {
											if (ape->sp.val < ape->sp.max) {
												ape->sp.val++;
											}
										}
									}
									crappy = "";
									if (ape->sentient) {
										//measure income inequality in terms of distance from average gold.
										for (Item it : ape->inventory) {
											if (it.name != "null") {
												food_held += items[it.i].hunger;
											}
										}
									}
									if (!ape->combat) {
										if (ape != p) {
											crappy = AIController(ape, map, p, this_cook);
											cooked += this_cook;
											if (NPC_TEST) {
												crappy = "";
											}
										}
										else if (ape->move != HERE || NPC_TEST) { //Control player during debug mode npc test player
											crappy = AIController(ape, map, p, this_cook);
											cooked += this_cook;
										}
										if ((x == p->x && y == p->y) || ape == p) {
											if (crappy != "") {
												if (!NPC_TEST) {
													log.push_back(crappy);
												}
												else if (log.size() < 2) {
													log.push_back(to_str(ape->acted) +" "+ crappy);
												}
												else {
													log[1] = to_str(ape->acted) +" "+ crappy;
												}
											}
										}
									}
								}
								if (a < map[y][x].people.size() && map[y][x].people[a] != ape) {
									a--;
								}
							}
						}
					}
				}
				if (NPC_TEST) {
					std::string s = "Tick: *GREEN*" + to_str(turn_counter) + "*GREEN*\nDeaths: *RED*" + to_str(death_counter) + "*RED*\nApes: *TEAL*"
						+ to_str(ape_counter) + "*TEAL*\nAvg Turns Survived: *ORANGE*" + to_str((int)avg_death_tick)+"\n*RED*Hunger *RED*: " + to_str(hunger) + "\n*GRASS*Food Held*GRASS*: " + to_str(food_held);
					if (food_held > last_food_held) {
						s += " *GREEN*+" + to_str(food_held - last_food_held) + "*GREEN*";
					}
					else if (food_held < last_food_held) {
						s += " *RED*-" + to_str(last_food_held - food_held) + "*RED*";
					}
					s += "\n";
					last_food_held = food_held;

					sort(jobs.begin(), jobs.end(), greater);
					for (int i = 0; i < min(5, jobs.size()); i++) {
						s += "*ORANGE*" + jobs[i].name + "*ORANGE* - *GREY*" + to_str(jobs[i].value) + "*GREY*\n";
					}
					s += "*BLUE*Food Cooked*BLUE* - ";
					if (cooked > 0) {
						s += "*GREEN*" + to_str(cooked) + "*GREEN*\n";
					}
					else {
						s += "*RED*" + to_str(cooked) + "*RED*\n";
					}

					log[0] = s;
						//"*ORANGE*\nArtisans: *GREY*" + to_str(artisan_deaths)
						//+"*GREY*\nFishermen: *GREY*" + to_str(fisher_deaths) +
						//"*GREY*\nGatherers: *GREY*"+to_str(gatherer_deaths)+"*GREY*"
						//+ "*GREY*\nChefs: *GREY*" + to_str(chef_deaths) + "*GREY*", midbar.w);
				}
			}
		}
		else {
			if (trade.active) {
				appraiseTrade(trade);
				if (trade.msg == "") {
					if (talk.active) {
						talk.gift_value = 0 - trade.gold;
						if (trade.giving) {
							talk.gift_value *= -1;
						}
						ParseTalk(talk, "", trade);
					}
					ExecuteTrade(trade);
					trade.active = false;
				}
			}
			else if (talk.active) {
				ParseTalk(talk, input, trade);
				input = "";
			}
		}
	}
	else if (p->hp.val <= 0){
		input = "*RED* You're dead. *RED*";
	}

	//Print log, status, input - -------------------------------------------------------------------------------------------------------------------------------------
	
	//Print(terraName(map[p->y][p->x].type), midbar.x + midbar.w/2, STATUS_LINE + 3, true);
	//Print player stats:
	std::string hp, sp, mp, ap, color;
	if (combat == nullptr && !viewMap) {
		Print("*ORANGE*HUNGER*ORANGE*:", midbar.x + midbar.w - 31, midbar.y + 5, true);
		Print("*BLUE*THIRST*BLUE*:", midbar.x + midbar.w - 31, midbar.y + 12, true);
		Print("*GREEN*MISERY*GREEN*:", midbar.x + midbar.w - 31, midbar.y + 19, true);
		Print(colorize(p->state.hunger), midbar.x + midbar.w - 10, midbar.y + 5, true);
		Print(colorize(p->state.thirst), midbar.x + midbar.w - 10, midbar.y + 12, true);
		Print(colorize(p->state.misery), midbar.x + midbar.w - 10, midbar.y + 19, true);
		StatStrings(p, hp, sp, ap);
		Print(hp, midbar.x + 2, midbar.y + 4);
		Print(ap, midbar.x + 2, midbar.y + 12);
		if (rightSelecting) {
			Print("*TEAL*"+topic+"*TEAL*" + " *GREY*-*GREY* " + p->job + " *GREY*-*GREY* " + "Gold: *YELLOW*" + to_str(p->gold) + "*YELLOW*", sidebar.x + sidebar.w / 2, sidebar.y - 4, true);
		}
		else {
			Print(topic + " *GREY*-*GREY* " + p->job + " *GREY*-*GREY* " + "Gold: *YELLOW*" + to_str(p->gold) + "*YELLOW*", sidebar.x + sidebar.w / 2, sidebar.y - 4, true);
		}
		SideBar(p);
		LeftBar(map[p->y][p->x], p);
	}
	else if (!viewMap) { //Draw Combat stuff
		DrawCombat();
	}

	if (!viewMap && !trade.active && (combat == nullptr) && !talk.active) { // Draw event log
		int logStart = midbar.y + 16;
		int dY = INPUT_LINE - 16;
		for (int i = log.size() - 1; i >= 0; i--) {
			dY -= Print(log[i]) * 5;
			if (dY > logStart) {
				Print(log[i], midbar.x + 2, dY, false, midbar.w - 1);
			}
			else {
				break;
			}
		}
		Print("*GREEN*>*GREEN* " + input, midbar.x + 1, INPUT_LINE);
	}
	else if (trade.active) { // Draw trade stuff
		std::string tradeText = drawTrade(trade);
		Print(tradeText, midbar.x + 2, midbar.y + 20);
	}
	else if (talk.active) {
		if (talk.page == 0) {
			std::string talkText = drawTalk(talk);
			Print(talkText, midbar.x + 2, midbar.y + 25, false, midbar.w);
		}
		Print("*GREEN*>*GREEN* " + input, midbar.x + 1, INPUT_LINE);
	}
	//viewMap = true;
	if (viewMap) {
		DrawMap(0, 0, map.size(), map[0].size(), false, false);
	}
	else if (!viewMap && !combat) {
		DrawMap(mapbar.x, mapbar.y, mapbar.w / 3, mapbar.h / 5);
	}

return true;
}

/*std::string CombatParse(std::string input) {
	std::string text = input;
	std::string s = "";
	std::string word = "";
	std::vector <std::string> words;
	int target = -1;
	int index = combat->turn / 2;
	bool turn = false;

	if (combat == nullptr) {
		return "wtf";
	}
	//Player is in combat
	if (index >= combat->allies.size() || combat->allies[index] != p) {
		return "*RED*It's not your turn.*RED*";
	}

	for (int i = 0; i < text.length(); i++) {
		if (text[i] == ' ') {
			if (word.length() > 0 && word != "the")
				words.push_back(word);
			word = "";
		}
		else {
			word += std::tolower(text[i]);
		}
	}

	if (word.length() > 0) {
		words.push_back(word);
	}
	if (words.size() == 0) {
		return s;
	}
	//Deal with input here.
	s = attack(p, text, combat->enemies, p);
	if (!turn) {
		if (words[0] == "rest") {
			p->ap.val = min(p->ap.max, p->ap.val + 10);
			turn = true;
		}
		else if (words[0] == "end") {
			if (!(checkCombat(*combat, &map[p->y][p->x]))) {
				s = "*RED*Unable to end combat while enemies have not been incapacitated. *RED*";
			}
			else {
				s = "*GREEN*Victory!*GREEN*";
				for (int i = 0; i < map[p->y][p->x].fights.size(); i++) {
					for (Person* ape : map[p->y][p->x].fights[i].allies) {
						if (ape == p) {
							map[p->y][p->x].fights.erase(map[p->y][p->x].fights.begin() + i);
							combat = nullptr;
							return s;
						}
					}
				}
				s = "*RED*Error fight not found oh no *RED*";
			}
		}
	}

	if (turn) {
		incrementTurn(*combat);
		if (checkSide(combat->enemies, combat->level)) {
			s += "\n*GREEN*Enemies defeated! Type *GREEN*'END'*GREEN* to end the battle. *GREEN*";
		}
	}

	return s;
}*/

std::string Parse(std::string input) {
	if (talk.active) {
		return "";
	}
	std::string text = input;
	std::string s = "";
	std::string word = "";
	bool in_index = false;
	std::string word1 = "";
	std::vector <std::string> words;
	for (int i = 0; i < text.length(); i++) {
		if (text[i] == ' ') {
			if (word.length() > 0 && word != "the" && word != "to")
				words.push_back(word);
			word = "";
		}
		else {
			word += std::tolower(text[i]);
		}
	}
	if (word.length() > 0) {
		words.push_back(word);
	}
	word = "";
	for (int i = 1; i < words.size(); i++) {
		if (!in_index) {
			word += words[i];
		}
		else {
			word1 += words[i];
		}
		if (i < words.size() - 1) {
			if (!in_index) {
				word += " ";
			}
			else {
				word1 += " ";
			}
		}
	}
	word = low(word);

	//Determine object of player input ie. Cook Fish -> Fish
	int obj_idx = -1;
	if (words.size() > 1) {
		if (words[1].size() == 1) { //A single number.
			int index = words[1][0] - 49;
			if (index >= 0 && index < 8) {
				int counter = -1;
				for (int j = 0; j < INVENTORY_SIZE; j++) {
					if (p->inventory[j].name != "null") {
						if (++counter == index) {
							obj_idx = j;
							break;
						}
					}
				}
			}
		}
		else { //otherwise, look at the rest of the command.
			for (int j = 0; j < INVENTORY_SIZE; j++) {
				if (low(p->inventory[j].name) == word && (words[0] != "cook" || items[p->inventory[j].i].type == RAW_FOOD)) {
					obj_idx = j;
					break;
				}
			}
		}
	}

	if (words.size() == 0) {
		return s;
	}
	if ((words[0] == "view" || words[0] == "look") && words.size() > 1) { //View an Item
		if (words[1].size() == 1) {
			int index = words[1][0] - 49;
			if (index >= 0 && index < 8) {
				if (p->inventory[index].name == "null") {
					s = "No item in this inventory slot. ";
				}
				else {
					s = p->inventory[index].name;
				}
			}
			else if (words[1] == "a") { // Outfit
				if (p->outfit.name == "null") {
					s = "You're not wearing any outfit. ";
				}
				else {
					s = p->outfit.name;
				}
			}
			else if (words[1] == "b") { // Trinket
				if (p->trinket.name == "null") {
					s = "You don't have a trinket equipped. ";
				}
				else {
					s = p->trinket.name;
				}
			}
			else if (words[1] == "c") { // Weapon
				if (p->weapon.name == "null") {
					s = "You don't have a weapon equipped. ";
				}
				else {
					s = p->weapon.name;
				}
			}
		}
	}
	else if (words[0] == "death") {
		s = "*RED*" + to_str(death_counter) + "*RED*";
	}
	else if (words[0] == "trade") {
		for (Person* ape : map[p->y][p->x].people) {
			if (ape != p) {
				if (low(ape->name) == word) {
					if (ape->sentient) {
						trade = TradeHandler();
						trade.active = true;
						trade.p1 = p;
						trade.p2 = ape;
					}
					else {
						s = "*RED*You can't trade with that. *RED*";
					}
					break;
				}
			}
		}
	}
	else if (words[0] == "combat") {
		topic = "combat";
	}
	else if (words[0] == "map") {
		viewMap = true;
	}
	else if (words[0] == "inventory") {
		topic = "inventory";
	}
	else if (words[0] == "attack") {
		for (Person* ape : map[p->y][p->x].people) {
			if (ape != p) {
				if (low(ape->name) == word) {
					if (ape->eats) {
						startFight(p, ape, map[p->y][p->x], IMPRISON);
						topic = "combat";
						combat = &map[p->y][p->x].fights.back();
					}
					else {
						s = "*RED*You can't attack that. *RED*";
					}
					break;
				}
			}
		}
	}
	else if (words[0] == "talk") {
		for (Person* ape : map[p->y][p->x].people) {
			if (ape != p) {
				if (low(ape->name) == word) {
					if (ape->sentient) {
						talk.active = true;
						talk.target = ape;
						talk.you = p;
						talk.page = 0;
					}
					else {
						s = "*RED*You can't talk to that. *RED*";
					}
					break;
				}
			}
		}
	}
	else if (words[0] == "look") {
		return Look(p, map[p->y][p->x]);
	}
	else if (words[0] == "go" || words[0] == "enter") {
		int index = -1;
		if (words[1].size() == 1) { //A single number.
			index = words[1][0] - 49;
			if (p->building == -1 && map[p->y][p->x].buildings.size() <= index) {
				index = -1;
			}
		}
		if (p->building == -1) {
			if (words[1] == "n" || words[1] == "north") {
				p->move = NORTH;
			}
			else if (words[1] == "s" || words[1] == "south") {
				p->move = SOUTH;
			}
			else if (words[1] == "e" || words[1] == "east") {
				p->move = EAST;
			}
			else if (words[1] == "w" || words[1] == "west") {
				p->move = WEST;
			}
			else { //check to enter a building
				Building b;
				for (int i = 0; i < map[p->y][p->x].buildings.size(); i++)  { //otherwise find the building names
					if (map[p->y][p->x].buildings[i].name == word) {
						index = i;
						break;
					}
				}
				if (index > -1) {
					b = map[p->y][p->x].buildings[index];
					p->building = index;
					p->room = -1;
					for (Exit e : b.exits) {
						for (int a = 0; a < 2; a++) {
							if (e.way[a] == -1) {
								p->room = e.way[(a + 1) % 2];
							}
						}
					}
					if (p->room == -1) {
						p->building = -1;
						std::cout << "No entry found!!!" << std::endl;
					}
				}
			}
		}
		else { //go to a different room
			char room = map[p->y][p->x].buildings[p->building].rooms[p->room].id;
			int counter = 0;
			for (Exit e : map[p->y][p->x].buildings[p->building].exits) {
				if (p->room > -1 && p->building > -1) {
					for (int i = 0; i < 2; i++) {
						if (e.way[i] == room) {
							if (e.way[(i + 1) % 2] > -1) {
								if (index == counter || map[p->y][p->x].buildings[p->building].rooms[e.way[(i + 1) % 2]].name == words[1]) {
									p->room = e.way[(i + 1) % 2];
								}
							}
							else {
								if (index == counter || word == "outside") {
									p->room = -1;
									p->building = -1;
									break;
								}
							}
							counter++;
						}
					}
				}
			}
		}
	}
	else if (words[0] == "fish") {
		s = Fish(p, map[p->y][p->x], p);
	}
	else if (words[0] == "eat") {
		if (obj_idx > -1) {
			Eat(p, obj_idx, p);
		}
	}
	else if (words[0] == "cook") {
		if (obj_idx > -1) {
			Cook(p, obj_idx, map[p->y][p->x].utilities, p);
		}
	}
	else if (words[0] == "get") {
		obj_idx = -1;
		if (words.size() > 1) {
			if (words[1].size() == 1) { //A single number.
				int index = words[1][0] - 49;
				if (index >= 0 && index < 8) {
					int counter = -1;
					for (int j = 0; j < map[p->y][p->x].items.size(); j++) {
						if (map[p->y][p->x].items[j].name != "null") {
							if (++counter == index) {
								obj_idx = j;
								break;
							}
						}
					}
				}
			}
			else { //otherwise, look at the rest of the command.
				for (int j = 0; j < map[p->y][p->x].items.size(); j++) {
					if (low(map[p->y][p->x].items[j].name) == word) {
						obj_idx = j;
						break;
					}
				}
			}
		}
		if (obj_idx > -1) {
			s = Get(p, map[p->y][p->x].items, obj_idx, p);
		}
	}
	else if (words[0] == "drink") {
		if (obj_idx > -1) {
			Eat(p, obj_idx, p);
		}
		else {
			for (int i = HERE; i <= SOUTH; i++) {
				if (map[p->y][p->x].water[i] == 'r') {
					s = "*GREEN*You*GREEN* drink *BLUE*river water*BLUE*.";
					p->state.thirst -= 15;
					p->state.misery += 4;
					break;
				}
			}
			if (s == "") {
				s = "*RED*There's no drinkable water here. *RED*";
			}
		}
	}
	else if (words[0] == "light" || words[0] == "extinguish") {
		for (int i = 0; i < map[p->y][p->x].utilities.size(); i++) {
			if (word == low(map[p->y][p->x].utilities[i].name)) {
				if (map[p->y][p->x].utilities[i].fuel > 0) {
					map[p->y][p->x].utilities[i].active = words[0] == "light";
				}
			}
		}
	}
	else if (words[0] == "put" && obj_idx > -1) {
		for (int i = 0; i < map[p->y][p->x].utilities.size(); i++) {
			if (word1 == low(map[p->y][p->x].utilities[i].name)) {
				map[p->y][p->x].utilities[i].fuel += items[p->inventory[obj_idx].i].fuel;
				p->inventory[obj_idx].name = "null";
			}
		}
	}
	else if (words[0] == "equip" && obj_idx > -1) {
		Item holder = p->weapon;
		p->weapon = p->inventory[obj_idx];
		p->inventory[obj_idx] = holder;
	}
	yView = max(1, p->y/3);
	xView = max(1, p->x/3);
	yView = min(yView, map.size()-1);
	xView = min(xView, map[0].size()-1);

	last_input = input;

	return s;
}

void DrawCombat() {
	//Variables
	std::string hp, sp, mp, ap, color;
	Box mid = Box(99, 0, 283, screen.h);
	int yC = midbar.y + 3;
	int xC = 2;
	int w = midbar.w / 2 - 7;
	int h = 18;

	if (allies_selected) {
		int ape = combat->enemies.size();
		if (reticle >= ape) {
			reticle = 0;
		}
	}
	else {
		int ape = combat->enemies.size();
		if (reticle >= ape) {
			reticle = 0;
		}
	}

	//Black out the entire screen
	FillRectDecal(olc::vf2d(1, 1), olc::vf2d(478, 268), BLACK);

	//Draw bars down the sides:
	DrawBox(98, 0, 0, screen.h, "BG_GREY");
	DrawBox(381, 0, 0, screen.h, "BG_GREY");

	//Draw List of allies
	DrawBox(xC, yC, midbar.w / 2 - 3, 4 + (h + 5) * combat->allies.size(), "green", "allies");
	for (int i = 0; i < combat->allies.size(); i++) {
		StatStrings(combat->allies[i], hp, sp, ap);
		color = "white";
		std::string name_color = "*TEAL*";
		if (2 * i == combat->turn && combat->allies[i] != p) {
			color = "blue";
			combat->log.push_back(Battle(combat->allies, combat->enemies, i, *combat, p));
			incrementTurn(*combat);
		}
		if (combat->allies[i]->hp.val <= 0) {
			color = "grey";
			if (combat->allies[i]->dead) {
				name_color = "*GREY*";
			}
		}
		std::string ally_name = name_color + combat->allies[i]->name + name_color;
		for (int ape : ally_targets) {
			if (ape == i) {
				ally_name = "*GREEN*[*GREEN*" + ally_name + "*GREEN*]*GREEN*";
			}
		}
		if (allies_selected && reticle == i) {
			ally_name = "*ORANGE*>*ORANGE*" + ally_name;
		}
		DrawBox(4, yC + 7 + i * (h + 5), w, h, color, ally_name, LEFT);
		Print(hp, 6, yC + 12 + i * (h + 5));
		Print(ap, 6, yC + 19 + i * (h + 5));
	}

	//Draw list of enemies:
	xC = 475 - w;
	DrawBox(xC - 2, yC, w + 4, 4 + (h + 5) * combat->enemies.size(), "red", "enemies");
	for (int i = 0; i < combat->enemies.size(); i++) {
		StatStrings(combat->enemies[i], hp, sp, ap);
		color = "white";
		std::string name_color = "*YELLOW*";
		if (2 * i + 1 == combat->turn) {
			color = "blue";
			combat->log.push_back(Battle(combat->enemies, combat->allies, i, *combat, p));
			incrementTurn(*combat);
		}
		if (combat->enemies[i]->hp.val <= 0) {
			color = "grey";
			if (combat->enemies[i]->dead) {
				name_color = "*GREY*";
			}
		}
		std::string name = name_color + combat->enemies[i]->name + name_color;
		for (int ape : enemy_targets) {
			if (ape == i) {
				name = "*ORANGE*[*ORANGE*" + name + "*ORANGE*]*ORANGE*";
			}
		}
		if (!allies_selected && reticle == i) {
			name = "*GREEN*>*GREEN*" + name;
		}
		DrawBox(xC, yC + 7 + i * (h + 5), midbar.w / 2 - 7, 18, color, name, LEFT);
		Print(hp, xC + 2, yC + 12 + i * (h + 5));
		Print(ap, xC + 2, yC + 19 + i * (h + 5));
	}

	//Draw menu stuff ------------------------------------------------------------------------
	std::vector<std::string> topics = { "attacks", "spells", "items", "runes", "actions" };
	std::string other = "actions";
	if (combat_topic > 0) {
		other = topics[combat_topic - 1];
	}
	Print("*GREY*" + other + "*GREY*", screen.w / 2 - 90, screen.y + 8, true);

	other = "items";
	if (combat_topic < topics.size() - 1) {
		other = topics[combat_topic + 1];
	}
	Print("*GREY*" + other + "*GREY*", screen.w / 2 + 90, screen.y + 8, true);

	Print(topics[combat_topic], screen.w / 2, screen.y + 6, true);

	int dY = 15;

	Print(combat->message, screen.w / 2, screen.y + 50, true);

	if (combat_topic == 0) {
		//List of attacks
		//Attacks have -> Attack names, # of hits, hit chance, Sharp DMG, Blunt DMG, AP cost
		short dmg, pen, hit;
		int oneCenter = combat_screen.x + 87;
		int twoCenter = combat_screen.x + 104;
		int threeCenter = combat_screen.x + 121;
		int fourCenter = combat_screen.x + 138;
		int fiveCenter = combat_screen.x + 155;
		std::string energy;
		if (combat_selected < 0) {
			combat_selected = p->attacks.size() - 1;
		}
		combat_selected = combat_selected % p->attacks.size();
		for (int i = 0; i < p->attacks.size(); i++) {
			//Attack AP cost
			dmg = attacks[p->attacks[i]].dmg * items[p->weapon.i].dmg.dmg;
			pen = attacks[p->attacks[i]].pen * items[p->weapon.i].dmg.pen;
			hit = max(1, attacks[p->attacks[i]].hit + items[p->weapon.i].dmg.hit - p->skills[MELEE]);
			energy = "*RED*" + to_str(energy_cost(attacks[p->attacks[i]], p->weapon)) + "*RED*";
			if (energy_cost(attacks[p->attacks[i]], p->weapon)) {
				energy = "*GRASS*" + to_str(energy_cost(attacks[p->attacks[i]], p->weapon)) + "*GRASS*";
			}
			std::string attack_name = "  " + attacks[p->attacks[i]].name;
			if (combat_selected == i) {
				if (reticle != -1) {
					attack_name = "*GREEN*>*GREEN* *TEAL*" + attacks[p->attacks[i]].name + "*TEAL*";
				}
				else {
					attack_name = "*GREEN*>*GREEN* " + attacks[p->attacks[i]].name;
				}
			}
			Print(attack_name, combat_screen.x + 5, dY + i * 7);
			Print(energy, twoCenter, dY + 3 + i * 7, true);
			Print(to_str(hit), threeCenter, dY + 3 + i * 7, true);
			Print(to_str(dmg), fourCenter, dY + 3 + i * 7, true);
			Print(to_str(pen), fiveCenter, dY + 3 + i * 7, true);
		}
	}
	else if (combat_topic == 1) {
		//Draw Spells
	}
	else if (combat_topic == 2) {
		//Draw Items
	}
	else if (combat_topic == 3) {
		//Draw Runes
	}
	else if (combat_topic == 4) {
		//List of actions
	}
	//END MID UI SEGMENT------------------------------------------------------------------

	//LOG STUFF---------------------------------------------------------------------------
	//Print combat log
	int logH = 135;
	int logW = mid.w - 4;
	int logY = mid.y + (mid.h - (2+logH));
	int logX = mid.x + 1;
	dY = logY + logH;
	DrawBox(logX, logY, logW, logH, "", "log");

	for (int i = combat->log.size() - 1; i >= 0; i--) {
		dY -= 7;
		if (dY > logY) {
			Print(combat->log[i], 103, dY, false, 278);
		}
		else {
			break;
		}
	}
	//END LOG STUFF-----------------------------------------------------------------------
}

void DrawMap(int x1, int y1, int w, int h, bool zoomed_in = true, bool center = true) {
	//Draw Map
	int sX = 0;
	int sY = 18;
	int dX = x1;
	int dY = y1;
	int x = 0;
	int y = 0;
	int yS = 0;
	int xS = 0;
	if (center) {
		yS = yView;
		xS = xView;
	}
	bool t; bool b; bool l; bool r;
	if (!zoomed_in) {
		for (int i = 1; i < w*3 - 1; i += 1) {
			for (int j = 1; j < h*3 - 1; j += 1) {
				if (i > 0 && i < map.size() - 1) {
					if (j > 0 && j < map[0].size() - 1) {
						sY = 20;
						switch (map[i][j].type) {
						case 'w': sX = 21; break;
						case 'g': sX = 11; break;
						case 'h': sX = 6; break;
						case 'f': sX = 1; break;
						case 't': sX = 16; break;
						case 'l': sX = 30; break;
						case 'm': sX = 36; break;
						case 's': sX = 21; sY = 21; break;
						case 'r': sX = 25; break;
						}
						//DrawPartialSprite(olc::vf2d(j, i), fontSprite, olc::vf2d(sX, sY), olc::vf2d(1, 1), 1);
						//DrawPartialDecal(olc::vf2d(j, i), olc::vf2d(sX, sY), font, olc::vf2d(1, 1), olc::vf2d(1, 1));
						DrawPartialDecal(olc::vf2d(j, i), olc::vf2d(1, 1), font, olc::vf2d(sX, sY), olc::vf2d(1, 1));
						//DrawPartialSprite(const olc::vi2d& pos, Sprite* sprite, const olc::vi2d& sourcepos, const olc::vi2d& size, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);
					}
				}
			}
		}
	}
	else {
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				dY = i * TEXT_H;
				dX = j * TEXT_W;
				if (center) {
					dY += y1;
					dX += x1;
					y = i + p->y - h / 2;
					x = j + p->x - w / 2;
				}
				else {
					y = i + y1;
					x = j + x1;
				}
				if (y > 0 && y < map.size() - 1) {
					if (x > 0 && x < map[0].size() - 1) {
						sY = 18;
						switch (map[y][x].type) {
						case 'w': sX = 20; break;
						case 'g': sX = 10; break;
						case 'h': sX = 5; break;
						case 'f': sX = 0; break;
						case 't': sX = 15; break;
						case 'l': sX = 30; break;
						case 'm': sX = 35; break;
						case 's': sX = 25;
							if (map[y - 1][x].type != 'w' && map[y - 1][x].type != 's') {
								sY += 6;
							}
							break;
						case 'r': sX = 40;
							t = (y > 0 && (map[y - 1][x].type == 'r' || map[y - 1][x].type == 'w' || map[y - 1][x].type == 's'));
							b = (y < map.size() && (map[y + 1][x].type == 'r' || map[y + 1][x].type == 'w' || map[y + 1][x].type == 's'));
							l = (x > 0 && (map[y][x - 1].type == 'r' || map[y][x - 1].type == 'w' || map[y][x - 1].type == 's'));
							r = (x < map[y].size() && (map[y][x + 1].type == 'r' || map[y][x + 1].type == 'w' || map[y][x + 1].type == 's'));
							break;
						case 'x': sX = 45; break;
						}
						int ran = randC(x, y) * 10;
						if (map[y][x].type == 'l') {
							if (ran < 2) {
								sY += 6;
							}
						}
						DrawPartialDecal(olc::vf2d(dX, dY), olc::vf2d(3, 5), font, olc::vf2d(sX, sY), olc::vf2d(3, 5));
						if (map[y][x].type == 'r') {
							int num = 0;
							if (t) {
								DrawPartialDecal(olc::vf2d(1 + dX, dY), olc::vf2d(1, 2), font, olc::vf2d(41, 23), olc::vf2d(1, 2));
							}
							if (b) {
								DrawPartialDecal(olc::vf2d(1 + dX, 3 + dY), olc::vf2d(1, 2), font, olc::vf2d(41, 23), olc::vf2d(1, 2));
							}
							if (r) {
								DrawPartialDecal(olc::vf2d(2 + dX, 2 + dY), olc::vf2d(1, 1), font, olc::vf2d(41, 23), olc::vf2d(1, 1));
							}
							if (l) {
								DrawPartialDecal(olc::vf2d(dX, 2 + dY), olc::vf2d(1, 1), font, olc::vf2d(41, 23), olc::vf2d(1, 1));
							}
						}
						//draw person indicator overlay
						olc::Pixel color = olc::WHITE;

						int num_people = 0;
						for (Person* ape : map[y][x].people) {
							if (ape->eats) {
								num_people++;
							}
						}
						if (x == p->x && y == p->y) {
							color = GREEN;
						}
						else if (num_people > 0){
							if (num_people == 1) {
								color = GREY;
							}
							else if (num_people < 3) {
								color = ORANGE;
							}
							else if (num_people < 6) {
								color = RED;
							}
							else {
								color = TEAL;
							}
						}
						if (color != olc::WHITE) {
							DrawPartialDecal(olc::vf2d(dX, dY), olc::vf2d(3, 5), font, olc::vf2d(16, 32), olc::vf2d(3, 5), color);
						}
					}
				}
				else {
					break;
				}
			}
		}
	}
}

int Print(std::string text, int x = -1, int y = -1, bool center = false, int cap = 1920, bool small_spaces = false) {
	olc::Pixel p = WHITE;
	if (y == -1) {
		cap = x;
	}
	std::vector<int> splitIndexes;
	std::string line = "";
	bool skipping = false;
	std::string color = "";
	std::vector<int> lineWidth = { 0 };
	int spaces = 0; int splits = 0;
	int drawX = x;
	int drawY = y;
	int offX = 0; int offY = 0;
	int sH = 5; int sW = 3;
	int sY = 1;
	int sX = 65;
	bool prnt = true;

	int wordlen = 0;
	for (char& c : text) {
		if (c == '*') {
			skipping = !skipping;
		}
		else if (!skipping) {
			wordlen += charWidth(c) + 1;
			lineWidth[lineWidth.size() - 1] += charWidth(c) + 1;
			line += c;
			if (c == ' ' || c == '\n') {
				if (c == '\n' || lineWidth[lineWidth.size() - 1] > cap) {
					line = "";
					if (lineWidth[lineWidth.size() - 1] > cap && spaces > 0) {
						splitIndexes.push_back(spaces - 1);
						lineWidth[lineWidth.size() - 1] -= wordlen;
						lineWidth.push_back(wordlen);
					}
					else {
						splitIndexes.push_back(spaces);
						lineWidth.push_back(0);
					}
				}
				wordlen = 0;
				spaces++;
			}
		}
	}
	if (lineWidth[lineWidth.size() - 1] > cap && spaces > 0) {
		splitIndexes.push_back(spaces - 1);
		lineWidth[lineWidth.size() - 1] -= wordlen;
		lineWidth.push_back(wordlen);
	}
	spaces = 0;
	if (y == -1) {
		return splitIndexes.size();
	}
	if (center) {
		drawX = 1 + x - lineWidth[splits] / 2;
		drawY = y - 3;
	}
	skipping = false;
	for (char& c : text) {
		if (c == '*') {
			skipping = !skipping;
			if (!skipping) {
				if (GetColor(color, timer) != p) {
					p = GetColor(color, timer);
				}
				else {
					p = WHITE;
				}
				color = "";
			}
		}
		else if (!skipping) {
			offX = 0; offY = 0;
			sH = 5; sW = charWidth(c, small_spaces);
			sY = 0; sX = 0;
			prnt = true;
			if (c >= 48 && c <= 57) { //Numbers
				sY = 0;
				sX = ((int)c - 48) * 5;
			}
			else if (c >= 65 && c <= 90) {
				sY = 6;
				sX = ((int)c - 65) * 5;
			}
			else if (c >= 97 && c <= 122) {
				sY = 6;
				sX = ((int)c - 97) * 5;
			}
			else {
				sY = 12;
				switch (c) {
				case 1: sX = 58; sY = 40; break; //Blunt DMG
				case 2: sX = 61; sY = 40; break; //Sharp DMG
				case 3: sX = 64; sY = 40; break; //Magic DMG
				case 4: sX = 67; sY = 40; break; //Shield/Armor
				case 5: sX = 70; sY = 40; break; //Gold Coin
				case '"': case '\'': sX = 2; sY = 15; sH = 2; break;
				case '.': sX = 2; sY = 15; sH = 1; offY = 4; break;
				case ',': sX = 2; sY = 15; sH = 2; offY = 4; break;
				case '>': sX = 5; break;
				case '?': sX = 16; break;
				case '!': sX = 11; break;
				case '+': sX = 20; break;
				case '-': sX = 25; break;
				case '$': sX = 30; break;
				case ':': sX = 41; sH = 4; break;
				case '<': sX = 73; break;
				case '[': sX = 44; break;
				case ']': sX = 51; break;
				case '#': sX = 55; break;
				case '(': sX = 60; break;
				case ')': sX = 64; break;
				case '=': sX = 68; break;
				case '/': sX = 91; break;
				case '%': sX = 95; break;
				case '|': sX = 87; break;
				case ';': sX = 80; sY = 13; sH = 4; offY = 1; break;
				case '\n': prnt = false;
				default: prnt = false;
				}
			}
			if (prnt) {
				//DrawPartialDecal(pos, size, decal, source_pos, source_size, tint);
				DrawPartialDecal(olc::vf2d(drawX + offX, drawY + offY), olc::vf2d(sW, sH), font, olc::vf2d(sX, sY), olc::vf2d(sW, sH), p);
			}
			if (c != '\n') {
				drawX += sW + offX + 1;
				if (c == '#') {
					drawX--;
				}
			}
			if (c == ' ' || c == '\n') {
				if (splitIndexes.size() > splits && splitIndexes[splits] == spaces++) {
					drawX = x;
					splits++;
					if (center) {
						drawX -= lineWidth[splits] / 2;
					}
					drawY += 7;
				}
			}
		}
		else {
			color += c;
		}
	}
	return splits;
}

void DrawBox(int x, int y, int w, int h, std::string col = "WHITE", std::string label = "", FORMAT format = CENTER) {
	olc::Pixel color = GetColor(col, timer);
	int labelWidth = measureText(label) - 1;

	if (labelWidth > w) {
		return;
	}

	FillRectDecal(olc::vf2d(x, y), olc::vf2d(1, h), color);
	FillRectDecal(olc::vf2d(x+w, y), olc::vf2d(1, h+1), color);
	FillRectDecal(olc::vf2d(x, y), olc::vf2d(w, 1), color);
	FillRectDecal(olc::vf2d(x, y+h), olc::vf2d(w, 1), color);

	if (labelWidth > 0) {
		if (format == LEFT) { //label is 3px from the left corner
			FillRectDecal(olc::vf2d(x + 3, y - TEXT_H / 2), olc::vf2d(2 + labelWidth, TEXT_H), BLACK);
			Print(label, x + 4, y - TEXT_H/2);
		}
		else if (format == RIGHT) { //label is 3px from the right corner
			FillRectDecal(olc::vf2d(x + w - (5 + labelWidth), y - TEXT_H / 2), olc::vf2d(2 + labelWidth, TEXT_H), BLACK);
			Print(label, x + w - (4 + labelWidth), y - TEXT_H / 2);
		}
		else if (format == CENTER) { //label is centered
			FillRectDecal(olc::vf2d(x + 1 + (w / 2 - (2 + labelWidth / 2)), y - TEXT_H / 2), olc::vf2d(2 + labelWidth, TEXT_H), BLACK);
			Print(label, x + (w / 2 - (labelWidth / 2)), y - TEXT_H / 2);
		}
	}
}
void LeftBar(Tile t, Person* p) {
	if (viewMap) {
		return;
	}
	int counter = 0;
	std::string s = "";
	std::string header = "";
	std::string color = "";

	Print("Sunny", leftbar.x + 2, 2);
	Print("6:00 PM", leftbar.w - 27, 2);

	//Print buildings on a tile-----------------------------------------------------------------------------
	s = "";
	color = "";
	if (leftSelected == 0) {
		color = "*TEAL*";
		if (!leftSelecting) {
			color = "*ORANGE*";
		}
	}
	if (p->building == -1) { //Outside of a building, draw list of buidlings
		counter = 0;
		while (t.buildings.size() / 4 < leftUI[0] - 1) {
			leftUI[0]--;
		}
		for (int i = (leftUI[0] - 1) * 4; i < min(t.buildings.size(), (leftUI[0]) * 4); i++) {
			Building b = t.buildings[i];
			s += to_str(++counter) + " " + b.color + b.name + b.color + "\n";
		}
		header = color + "Buildings: " + color + to_str(leftUI[1]) + "/" + to_str(1 + t.buildings.size() / 5);
	}
	else { //Inside of a building, draw list of rooms
		std::vector<Exit> exits = {};
		char room = t.buildings[p->building].rooms[p->room].id;
		for (Exit e : t.buildings[p->building].exits) {
			for (int i = 0; i < 2; i++) {
				if (e.way[i] == room) {
					exits.push_back(e);
				}
			}
		}
		counter = 0;
		while (exits.size() / 4 < leftUI[0] - 1) {
			leftUI[0]--;
		}
		for (int i = (leftUI[0] - 1) * 4; i < min(exits.size(), leftUI[0] * 4); i++) {
			Exit e = exits[i];
			for (int j = 0; j < 2; j++){
				if (e.way[(j + 1) % 2] > -1) {
					s += to_str(++counter) + " " + "*TEAL*" + t.buildings[p->building].rooms[e.way[(j + 1) % 2]].name + "*TEAL*\n";
				}
				else {
					s += to_str(++counter) + " *GREEN*outside*GREEN*\n";
				}
			}
		}
		header = color + "Rooms: " + color + to_str(leftUI[1]) + "/" + to_str(1 + t.buildings.size() / 5);
	}
	DrawBox(leftbar.x + 1, leftbar.y + 11, leftbar.w - 3, 33, "PURPLE", header);
	Print(s, leftbar.x + 3, leftbar.y + 16, false, 1920, true);

	//Print list of people on a tile-------------------------------------------------------------------------
	s = "";
	counter = 0;
	while (t.people.size() / 8 < leftUI[1] - 1) {
		leftUI[1]--;
	}
	for (int i = (leftUI[1] - 1) * 7; i < min(t.people.size(), (leftUI[1]) * 7); i++) {
		Person* ape = t.people[i];
		if (ape != p && ape->eats) {
			s += char('a' + counter++);
			s += " " + relationName(ape, p);
			if (ape->move != HERE) {
				s += " > " + dirName(ape->move);
			}
			s += "\n";
		}
	}
	color = "";
	if (leftSelected == 1) {
		color = "*TEAL*";
		if (!leftSelecting) {
			color = "*ORANGE*";
		}
	}
	header = color + "People: " + color + to_str(leftUI[1]) + "/" + to_str(1 + t.people.size() / 8);
	DrawBox(leftbar.x + 1, leftbar.y + 49, leftbar.w - 3, 54, "GREEN", header);
	Print(s, leftbar.x + 3, leftbar.y + 54, false, 1920, true);

	//Print list of items on a tile----------------------------------------------------------------------------
	s = "";
	counter = 0;
	while (t.items.size() / 8 < leftUI[2] - 1) {
		leftUI[2]--;
	}
	for (int i = (leftUI[2] - 1) * 7; i < min(t.items.size(), leftUI[2] * 7); i++) {
		Item it = t.items[i];
		s += to_str(++counter);
		s += " " + it.color + it.name + it.color;
		s += "\n";
	}
	color = "";
	if (leftSelected == 2) {
		color = "*TEAL*";
		if (!leftSelecting) {
			color = "*ORANGE*";
		}
	}
	header = color + "Items: " + color + to_str(leftUI[2]) + "/" + to_str(1 + t.items.size() / 8);
	DrawBox(leftbar.x + 1, leftbar.y + 108, leftbar.w - 3, 54, "GOLD", header);
	Print(s, leftbar.x + 3, leftbar.y + 113, false, 1920, true);

	//Print list of resources on a tile-------------------------------------------------------------------------
	s = "";
	counter = 0;
	while (t.res.size() / 8 < leftUI[3] - 1) {
		leftUI[3]--;
	}
	for (int i = (leftUI[3] - 1) * 7; i < min(t.res.size(), leftUI[3] * 7); i++) {
		Resource r = t.res[i];
		s += char('a' + counter++);
		s += " " + r.color + r.name + r.color + "\n";
	}
	color = "";
	if (leftSelected == 3) {
		color = "*TEAL*";
		if (!leftSelecting) {
			color = "*ORANGE*";
		}
	}
	header = color + "Resources: " + color + to_str(leftUI[3]) + "/" + to_str(1 + t.res.size() / 8);
	DrawBox(leftbar.x + 1, leftbar.y + 167, leftbar.w - 3, 54, "ORANGE", header);
	Print(s, leftbar.x + 3, leftbar.y + 172, false, 1920, true);

	//Print list of utilities on a tile--------------------------------------------------------------------------
	s = "";
	counter = 0;
	while (t.utilities.size() / 5 < leftUI[4] - 1) {
		leftUI[4]--;
	}
	for (int i = (leftUI[4] - 1) * 5; i < min(t.utilities.size(), leftUI[4] * 5); i++) {
		Utility u = t.utilities[i];
		s += char('a' + counter++);
		s += " " + u.color + u.name + u.color + "\n";
	}
	color = "";
	if (leftSelected == 4) {
		color = "*TEAL*";
		if (!leftSelecting) {
			color = "*ORANGE*";
		}
	}
	header = color + "Utilities: " + color + to_str(leftUI[4]) + "/" + to_str(1 + t.utilities.size() / 4);
	DrawBox(leftbar.x + 1, leftbar.y + 226, leftbar.w - 3, 40, "RED", header);
	Print(s, leftbar.x + 3, leftbar.y + 231, false, 1920, true);

}
void SideBar(Person* p) {
	if (viewMap) {
		return;
	}
	std::string s = "";
	std::string header = "";
	std::string color = "";
	int dX = sidebar.x;
	int dY = sidebar.y + 3;
	int oneCenter = sidebar.x + 87;
	int twoCenter = sidebar.x + 104;
	int threeCenter = sidebar.x + 121;
	int fourCenter = sidebar.x + 138;
	int fiveCenter = sidebar.x + 155;

	int counter = 0;
	if (topic == "inventory") {
		//Print list of items on a tile----------------------------------------------------------------------------
		s = "";
		counter = 0;
		for (int i = 0; i < INVENTORY_SIZE; i++) {
			if (p->inventory[i].name != "null") {
				if (rightSelecting && i == rightSelected) {
					s += " *GREEN*" + to_str(++counter) + ") *GREEN*";
				}
				else {
					s += to_str(++counter) + ") ";
				}
				s += typeColor("#", items[p->inventory[i].i].type) + " " + p->inventory[i].color + p->inventory[i].name + p->inventory[i].color + "\n";
			}
		}
		DrawBox(dX + 2, dY, sidebar.w - 5, 11 + (counter - 1) * 7, "GOLD", "Items");
		Print(s, dX + 4, dY + 4);

		if (p->outfit.name != "null") { // Draw Outfit
			dY = sidebar.y + 126;
			s = p->outfit.color + p->outfit.name + p->outfit.color;
			DrawBox(sidebar.x + 2, dY, 108, 12, "white", "*GRASS*Outfit*GRASS*", LEFT);
			DrawBox(sidebar.x + 114, dY, 14, 12, "white", "\5", CENTER);
			DrawBox(sidebar.x + 131, dY, 14, 12, "white", "\2\4", CENTER);
			DrawBox(sidebar.x + 148, dY, 14, 12, "white", "\1\4", CENTER);

			Print(s, dX + 5, dY+ 5);
			Print("*YELLOW*" + to_str(p->outfit.value) + "*YELLOW*", threeCenter, dY+ 8, true);
			////Print("*GREY*" + to_str(items[p->outfit.i].def.blunt) + "*GREY*", fourCenter, dY+ 8, true);
			//Print("*GREY*" + to_str(items[p->outfit.i].def.sharp) + "*GREY*", fiveCenter, dY+ 8, true);
		}
		else {
			s = "*RED*No outfit equipped!*RED*";
			Print(s, dX + sidebar.w / 2, dY + 10, true);
		}
		if (p->trinket.name != "null") {
			dY = sidebar.y + 143;
			s = p->trinket.color + p->trinket.name + p->trinket.color;
			DrawBox(sidebar.x + 2, dY, 108, 12, "white", "*GRASS*Trinket*GRASS*", LEFT);
			DrawBox(sidebar.x + 114, dY, 14, 12, "white", "\5", CENTER);
			DrawBox(sidebar.x + 131, dY, 14, 12, "white", "\2\4", CENTER);
			DrawBox(sidebar.x + 148, dY, 14, 12, "white", "\1\4", CENTER);

			Print(s, dX + 5, dY + 5);
			Print("*GOLD*" + to_str(p->trinket.value) + "*GOLD*", threeCenter, dY + 8, true);
			//Print("*GREY*" + to_str(items[p->trinket.i].def.blunt) + "*GREY*", fourCenter, dY + 8, true);
			//Print("*GREY*" + to_str(items[p->trinket.i].def.sharp) + "*GREY*", fiveCenter, dY + 8, true);
		}
		else {
			s = "*GREY*No trinket equipped!*GREY*";
			Print(s, dX + sidebar.w / 2, dY+ 21, true); //20%
		}
		if (p->weapon.name != "null") {
			dY = sidebar.y + 160;
			s = p->weapon.color + p->weapon.name + p->weapon.color;
			DrawBox(sidebar.x + 2, dY, 108, 12, "white", "*GRASS*Weapon/Tool*GRASS*", LEFT);
			DrawBox(sidebar.x + 114, dY, 14, 12, "white", "[\1]", CENTER);
			DrawBox(sidebar.x + 131, dY, 14, 12, "white", "\2", CENTER);
			DrawBox(sidebar.x + 148, dY, 14, 12, "white", "\1", CENTER);

			Print(s, dX + 5, dY + 5);
			Print("*GOLD*" + to_str(items[p->weapon.i].size) + "*GOLD*", threeCenter, dY + 8, true);
			//Print("*GREY*" + to_str(items[p->weapon.i].dmg.blunt) + "*GREY*", fourCenter, dY + 8, true);
			//Print("*GREY*" + to_str(items[p->weapon.i].dmg.sharp) + "*GREY*", fiveCenter, dY + 8, true);
		}
		else {
			s = "*RED*No weapon equipped!*RED*";
			Print(s, dX + sidebar.w / 2, sidebar.y + 126, true); //20%
		}
			

		//Print("Gold: *YELLOW*" + to_str(p->gold) + "*YELLOW*", sidebar.x + sidebar.w / 2, sidebar.y + sidebar.h - 10, true);
	}
	else if (topic == "combat") {
		int ap_cost = 15;
		int dmg = 1;
		int pen = 0;
		int hit = 0;
		std::string energy = "";

		//Attacks
		if (p->attacks.size() > 0) {
			int attackY = sidebar.y + 4;
			DrawBox(sidebar.x + 2, attackY, 92, 5 + 7 * p->attacks.size(), "white", "*ORANGE*attack*ORANGE*", LEFT);

			DrawBox(sidebar.x + 97, attackY, 14, 5 + 7 * p->attacks.size(), "white", "*GREEN*AP*GREEN*", CENTER);
			DrawBox(sidebar.x + 114, attackY, 14, 5 + 7 * p->attacks.size(), "white", "*YELLOW*%*YELLOW*", CENTER);
			DrawBox(sidebar.x + 131, attackY, 14, 5 + 7 * p->attacks.size(), "white", "\1", CENTER);
			DrawBox(sidebar.x + 148, attackY, 14, 5 + 7 * p->attacks.size(), "white", "\2", CENTER);

			int dY = attackY + 5;
			for (int i = 0; i < p->attacks.size(); i++) {
				//Attack AP cost
				dmg = attacks[p->attacks[i]].dmg * items[p->weapon.i].dmg.dmg;
				pen = attacks[p->attacks[i]].pen * items[p->weapon.i].dmg.pen;
				hit = max(1, attacks[p->attacks[i]].hit + items[p->weapon.i].dmg.hit - p->skills[MELEE]);
				energy = "*RED*" + to_str(energy_cost(attacks[p->attacks[i]], p->weapon)) + "*RED*";
				if (energy_cost(attacks[p->attacks[i]], p->weapon)) {
					energy = "*GRASS*" + to_str(energy_cost(attacks[p->attacks[i]], p->weapon)) + "*GRASS*";
				}
				Print(to_str(i + 1) + ") " + attacks[p->attacks[i]].name, sidebar.x + 5, dY + i * 7);
				Print(to_str(energy), twoCenter, dY + 3 + i * 7, true);
				Print(to_str(hit), threeCenter, dY + 3 + i * 7, true);
				Print(to_str(dmg), fourCenter, dY + 3 + i * 7, true);
				Print(to_str(pen), fiveCenter, dY + 3 + i * 7, true);
			}
		}

		//Magic
		if (p->spells.size() > 0) {
			int spellY = sidebar.y + 71;
			DrawBox(sidebar.x + 2, spellY, 126, 5 + 7 * p->spells.size(), "white", "*BLUE*spell*BLUE*", LEFT);

			DrawBox(sidebar.x + 131, spellY, 14, 5 + 7 * p->spells.size(), "white", "*TEAL*MP*TEAL*", CENTER);
			DrawBox(sidebar.x + 148, spellY, 14, 5 + 7 * p->spells.size(), "white", "*YELLOW*%*YELLOW*", CENTER);

			/*int dY = spellY + 5;
			for (int i = 0; i < p->spells.size(); i++) {
				energy = "*RED*" + to_str(abs(p->spells[i].energy)) + "*RED*";
				if (p->spells[i].energy < 0) {
					energy = "*GRASS*" + to_str(abs(p->spells[i].energy)) + "*GRASS*";
				}
				Print(to_str(i+1) + ") " + p->spells[i].name, sidebar.x + 5, dY + i * 7);
				Print(to_str(energy), fourCenter, dY + 3 + i * 7, true);
				Print(to_str(p->spells[i].chance), fiveCenter, dY + 3 + i * 7, true);
			}*/
		}

		//Other - Flee, Yield, Rest
		if (p->actions.size() > 0) {
			int actionY = sidebar.y + 138;
			DrawBox(sidebar.x + 2, actionY, 126, 5 + 7 * p->actions.size(), "white", "*GRASS*action*GRASS*", LEFT);

			DrawBox(sidebar.x + 131, actionY, 14, 5 + 7 * p->actions.size(), "white", "*GREEN*AP*GREEN*", CENTER);
			DrawBox(sidebar.x + 148, actionY, 14, 5 + 7 * p->actions.size(), "white", "*YELLOW*%*YELLOW*", CENTER);

			/*int dY = actionY + 5;
			for (int i = 0; i < p->actions.size(); i++) {
				energy = "*RED*" + to_str(abs(p->actions[i].energy)) + "*RED*";
				if (p->actions[i].energy < 0) {
					energy = "*GRASS*" + to_str(abs(p->actions[i].energy)) + "*GRASS*";
				}
				Print(to_str(i + 1) + ") " + p->actions[i].name, sidebar.x + 5, dY + i * 7);
				Print(to_str(energy), fourCenter, dY + 3 + i * 7, true);
				Print(to_str(p->actions[i].chance), fiveCenter, dY + 3 + i * 7, true);
			}*/
		}
		
	}
}

};

int main()
{
	int arr[3] = { 1, 2, 3 };
	std::cout << test_sum(arr, 3) << std::endl;

	RPG game;
	RPG text_game;
	if (text_game.Construct(1920/4, 1080/4, 1, 1, true))
		text_game.Start();
	return 0;
}
