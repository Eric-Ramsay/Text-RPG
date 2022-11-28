#pragma once
#include "classes.h"
#include "functions.h"

std::vector<std::vector<Tile>> fillMap(std::vector<std::vector<char>>& chunks, bool fill = true);
std::vector<std::vector<char>> Chunkify(std::vector<std::vector<Tile>>& chunks);

void genOceans(int oceanSize, int x, int y, std::vector< std::vector<char>>& map) {
	int oS = oceanSize;
	int dir = 0;
	int yCoord = y;
	int xCoord = x;
	int lastDir = dir;
	while (oS-- > 0)
	{
		yCoord = safeC(yCoord, MAP_H);
		xCoord = safeC(xCoord, MAP_W);
		
		map[yCoord][xCoord] = 'w';
		
		dir = rand() % 4;
		if (dir == 0) { yCoord++; }
		else if (dir == 1) { xCoord++; }
		else if (dir == 2) { yCoord--; }
		else { xCoord--; }
		if (lastDir < 2) { lastDir = dir + 2; }
		else { lastDir = dir - 2; } //Asign Last Dir
	}
}

void slapIslands(std::vector<std::vector<char>>& map) {
	bool slap = false;
	for (int i = 1; i < MAP_H-1; i++) {
		for (int j = 1; j < MAP_W-1; j++) {
			slap = true;
			for (int a = -1; a < 2; a++) {
				for (int b = -1; b < 2; b++) {
					if (a * b == 0 && a != b) {
						if (map[i + a][j + b] == 'g') {
							slap = false;
							break;
						}
					}
				}
			}
			if (slap) {
				map[i][j] = 'w';
			}
		}
	}
}

std::vector<std::vector<Tile>> createMap() {
	std::vector<std::vector<char>> chunks;
	int dir = 0;
	int bannedDir = 0;
	int lastDir = -1;
	int bd = -1;
	bool foundWater = false;
	std::vector<Spot> points;
	bool found = false;
	int x, y, ran, TOWNS;
	chunks.resize(MAP_H);
	for (int i = 0; i < MAP_H; i++) {
		chunks[i].resize(MAP_W);
	}

	Spot spot(MAP_W / 2, MAP_H / 2);
	spot.h = MAP_H / 2;
	spot.g = MAP_W / 4;
	points.push_back(spot);

	spot.x = MAP_W / 8;
	spot.y = MAP_H / 4 + (rand() % 3) * MAP_H/4;
	spot.h = MAP_H / 4;
	spot.g = MAP_W / 8;
	points.push_back(spot);

	spot.x = 7*MAP_W / 8;
	spot.y = MAP_H / 4 + (rand() % 3) * MAP_H / 4;
	spot.h = MAP_H / 4;
	spot.g = MAP_W / 8;
	points.push_back(spot);


	bool found_point = false;
	for (int i = 0; i < MAP_H; i++) {
		for (int j = 0; j < MAP_W; j++) {
			chunks[i][j] = 'g';
			found_point = false;
			for (int a = 0; a < points.size(); a++) {
				if (fDist(j, i, points[a].x, points[a].y) < points[a].h/1.5 + rand() % points[a].h / 4) {
					if (abs(j - points[a].x) < points[a].g / 2 + rand() % points[a].g / 2.5) {
						if (abs(i - points[a].y) < points[a].h / 2 + rand() % points[a].h / 2.5) {
							found_point = true;
						}
					}
				}
			}
			if (!found_point) {
				chunks[i][j] = 'w';
			}
		}
	}

	points = {};

	//Gen Oceans
	genOceans(MAP_H*MAP_H, MAP_W/2, MAP_H-1, chunks);
	genOceans(MAP_H*MAP_H, MAP_W/2, 0, chunks);
	genOceans(MAP_H, MAP_W / 2, MAP_H/2, chunks);

	slapIslands(chunks);
	//Gen Rivers
	for (int i = 0; i < MAP_H; i++) {
		for (int j = 0; j < MAP_W; j++) {
			if (i < 6 || j < 6 || i > MAP_H - 6 || j > MAP_W - 6) {
				chunks[i][j] = 'w';
			}
			bannedDir = 0;
			if (i < MAP_H / 2) {
				bannedDir = 2;
			}
			bd = -1;
			points.clear();
			foundWater = false;
			if (chunks[i][j] == 'g' && rand() % 40 == 0 && bd == -1) {
				bd = 1;
				x = 0;
				y = 0;
				do {
					found = false;
					for (int a = -1; a < 2; a += 1) {
						for (int b = -1; b < 2; b += 1) {
							if ((a != 0 || b == 0) && (a == 0 || b == 0)) {
								if (chunks[i + y + a][j + x + b] == 'w') {
									found = true;
									foundWater = true;
								}
							}
							if (chunks[i + y + a][j + x + b] == 'r') {
								found = true;
							}
						}
					}
					points.push_back(Spot(i + y, j + x));
					do {
						dir = rand() % 4;
					} while (dir == lastDir || dir == bannedDir || dir == bd);
					switch (dir) {
					case 0: y--; break;
					case 1: x++; break;
					case 2: y++; break;
					case 3: x--; break;
					}
					bd = lastDir;
					if (dir < 2) {
						lastDir = dir + 2;
					}
					else {
						lastDir = dir - 2;
					}
				} while (!found && chunks[i + y][j + x] != 'r');
				if (foundWater) {
					for (int a = 0; a < points.size(); a++) {
						chunks[points[a].x][points[a].y] = 'r';
					}
				}
			}
		}
	}

	TOWNS = 0;
	//Gen Misc. Things
	bool river = false;
	bool forest = false;
	bool no_towns = true;
	for (int i = 0; i < MAP_H; i++) {
		for (int j = 0; j < MAP_W; j++) {
			if (chunks[i][j] != 'r') {
				ran = rand() % 31;
				if (chunks[i][j] == 'g') {
					if (ran == 30) { //gen mountains
						chunks[i][j] = 'h';
					}
					else if (ran > 20) {
						chunks[i][j] = 'f';
					}
					else if (ran == 1) {
						chunks[i][j] = 'l';
					}
					else if (ran < 20) {
						river = false;
						forest = false;
						no_towns = true;
						for (int a = -5; a <= 5; a++) {
							for (int b = -5; b <= 5; b++) {
								if (a != 0 || b != 0) {
									if ((abs(a) < 2 && abs(b) < 2)) {
										switch (chunks[i + a][j + b]) {
										case 'r': river = true; break;
										case 'f': forest = true; break;
										}
									}
									if (chunks[i + a][j + b] == 't') {
										no_towns = false;
										a = 5; b = 5;
									}
								}
							}
						}
						if (river && forest && no_towns) {
							chunks[i][j] = 't';
							TOWNS++;
						}
					}
				}
				else if (chunks[i][j] == 'w') {
					for (int a = -3; a <= 3; a++) {
						for (int b = -3; b <= 3; b++) {
							if (a + i > 0 && a + i < MAP_H) {
								if (j + b > 0 && j + b < MAP_W) {
									if (chunks[i + a][j + b] != 'w' && chunks[i + a][j + b] != 's') {
										if (fDist(i, j, i + a, j + b) <= 2) {
											chunks[i][j] = 's';
										}
									}
								}
							}
						}
					}
				}
			}
			else {
				if (chunks[i - 1][j] == 'r' && chunks[i][j - 1] == 'r' && chunks[i - 1][j - 1] == 'r') {
					chunks[i][j] = 'g';
				}
			}
		}
	}

	if (TOWNS < .003 * MAP_W*MAP_H) {
		//return createMap();
	}
	

	std::vector<std::vector<Tile>> map;
	map = fillMap(chunks, true);
	//chunks = Chunkify(map);
	//map = fillMap(chunks);

	return map;
}

int waterIndex(int i, int j) {
	if (i == 0) {
		if (j == 0) {
			return HERE;
		}
		if (j == -1) {
			return WEST;
		}
		if (j == 1) { //east
			return EAST;
		}
	}
	else if (i == -1){
		return NORTH;
	}
	return SOUTH;
}

int createTown(std::vector<std::vector<Tile>>& map, int x, int y, int startID = 0) { // Create a 3x3 town
	short townID = map[y][x].town;
	std::vector<Spot> town = { Spot(x, y) };
	std::vector<Spot> open = { Spot(x, y) };
	Spot current;
	Spot consider;
	int id = startID;
	bool valid = false;
	while (open.size() > 0) {
		current = open.back();
		open.pop_back();
		for (int a = -1; a < 2; a++) {
			for (int b = -1; b < 2; b++) {
				if (a * b == 0 && a != b) { // Only check Von Neumann neighborhood
					consider = Spot(current.x + a, current.y + b);
					valid = true;
					for (Spot s : town) { //Check if spot has been visited before
						if (s.x == consider.x && s.y == consider.y) {
							valid = false;
							break;
						}
					}
					if (valid) { //Push spot to town
						if (map[consider.y][consider.x].town == townID) {
							open.push_back(consider);
							town.push_back(consider);
							map[consider.y][consider.x].type = 'z'; //Temporary type so that in map gen, createTown wont be called multiple times per town
						}
					}
				}
			}
		}
	}
	//Town found, and is stored in the town list. Now it's time to make the people and buildings that will fill it up.
	std::vector<Person*> people;
	int homes = town.size() * 1.5;
	int index = 0;
	for (int i = 0; i < homes; i++) {
		people.push_back(new Person());
		createPerson(people.back());
		people[i]->id = id++;
		people[i]->society = townID;
	}
	for (int i = 0; i < people.size(); i++) {
		for (int j = i + 1; j < people.size(); j++) {
			people[i]->relations.push_back(Relation(people[j]->id, rand() % 20, 0));
			people[j]->relations.push_back(Relation(people[i]->id, rand() % 20, 0));
		}
	}

	while (homes > 0) {
		do {
			index = rand() % town.size();
		} while (map[town[index].y][town[index].x].buildings.size() >= 4);
		map[town[index].y][town[index].x].buildings.push_back(createHouse());
		map[town[index].y][town[index].x].buildings.back().id = people.back()->id;
		people.back()->x = town[index].x;
		people.back()->y = town[index].y;
		people.back()->home = town[index];
		map[town[index].y][town[index].x].people.push_back(people.back());
		people.pop_back();
		homes--;
	}
	return id;
}

std::vector<std::vector<char>> Chunkify(std::vector<std::vector<Tile>>& chunks) {
	std::vector<std::vector<char>> map;
	map.resize(chunks.size());
	for (int i = 0; i < chunks.size(); i++) {
		map[i].resize(chunks[0].size());
	}
	for (int i = 0; i < map.size(); i++) {
		for (int j = 0; j < map[i].size(); j++) {
			map[i][j] = chunks[i][j].type;
			if (map[i][j] == 'r') {
				if (chunks[i][j].water[HERE] != 'r') {
					map[i][j] = 'g';
				}
			}
		}
	}
	return map;
}

std::vector<std::vector<Tile>> fillMap(std::vector<std::vector<char>>& chunks, bool fill) {
	std::vector<std::vector<Tile>> map;

	std::vector<Person*> deer;
	std::vector<Resource> tree;

	Item onion(ONION);

	short town_ID = 0;
	int person_id = 1;

	for (int i = 0; i < 32; i++) {
		deer.push_back(new Person());
		tree.push_back(createTree());
		createDeer(deer[i]);
	}

	map.resize(chunks.size() * 3);
	for (int i = 0; i < chunks.size() * 3; i++) {
		map[i].resize(chunks[0].size() * 3);
	}
	bool n, s, w, e;
	char ran = 0;
	bool can_place = false;

	for (int i = 0; i < map.size(); i++) { //FIRST PASS, INITIAL TYPES
		for (int j = 0; j < map[i].size(); j++) {
			map[i][j].y = i;
			map[i][j].x = j;
			map[i][j].type = chunks[i / 3][j / 3];
			if (map[i][j].type == 'g') {
				for (int a = -1; a < 2; a++) {
					for (int b = -1; b < 2; b++) {
						if (a * b == 0 && a != b) {
							if (map[i + a][j + b].type == 't') {
								if (rand() % 10 == 0) {
									map[i][j].type = 't';
									map[i][j].town = map[i + a][j + b].town;
									break;
								}
							}
						}
					}
				}
				ran = rand() % 20;
				switch (ran) {
				case 0: map[i][j].type = 'l'; break;
				case 1: map[i][j].type = 'f'; break;
				case 2:
					can_place = true;
					for (int a = -3; a <= 3; a++) {
						for (int b = -3; b <= 3; b++) {
							if (map[i + a][j + b].type == 't') {
								can_place = false;
								a = 4; b = 4;
							}
						}
					}
					if (can_place) {
						map[i][j].type = 't';
					}
				break;
				}
			}
			if (map[i][j].type == 'h') { //turn hills into mountains
				if (i % 3 == 1 && j % 3 == 1) {
					if (rand() % 3 == 0) {
						map[i][j].type = 'm';
					}
				}
			}
			if (map[i][j].type == 't') {
				map[i][j].town = (i / 3 * MAP_W) + j / 3;
			}
		}
	} 
	for (int i = 0; i < map.size(); i++) { //SECOND PASS, TILE TRANSFORMATIONS, MORE TOWNS
		for (int j = 0; j < map[i].size(); j++) {
			if (map[i][j].type == 'g') {
				for (int a = -1; a < 2; a++) {
					for (int b = -1; b < 2; b++) {
						if (a * b == 0 && a != b) {
							if (map[i + a][j + b].type == 't') {
								if (rand() % 10 == 0) {
									map[i][j].type = 't';
									map[i][j].town = map[i + a][j + b].town;
									break;
								}
							}
						}
						if (map[i + a][j + b].type == 'f') {
							if (rand() % 10 == 0) {
								map[i][j].type = 'f';
								break;
							}
						}
					}
				}
			}
			if (map[i][j].type == 'g' && fill) {
				if (rand() % 7 == 0) {
					do {
						map[i][j].items.push_back(onion);
					} while (rand() % 3 == 0);
				}
			}
			if (map[i][j].type == 'f' && fill) { //Forest
				for (int k = 0; k < 6; k++) { //Push a tree to the forest
					map[i][j].res.push_back(tree[rand() % tree.size()]);
				}
				if (rand() % 12 == 0) { //Push a deer to the forest
					map[i][j].people.push_back(new Person());
					*map[i][j].people.back() = *deer[rand() % deer.size()];
					map[i][j].people.back()->y = i;
					map[i][j].people.back()->x = j;
					map[i][j].people.back()->home = Spot(j, i);
					map[i][j].people.back()->id = person_id++;
				}
			}
			if (i % 3 + j % 3 == 0 && chunks[i / 3][j / 3] == 'r') { // River
				n = chunks[i / 3 - 1][j / 3] == 'r' || chunks[i / 3 - 1][j / 3] == 's';
				s = chunks[i / 3 + 1][j / 3] == 'r' || chunks[i / 3 + 1][j / 3] == 's';
				w = chunks[i / 3][j / 3 - 1] == 'r' || chunks[i / 3][j / 3 - 1] == 'r';
				e = chunks[i / 3][j / 3 + 1] == 'r' || chunks[i / 3][j / 3 + 1] == 'r';
				map[i + 1][j + 1].water[HERE] = 'r';
				if (n) {
					map[i][j + 1].water[HERE] = 'r';
				}
				if (s) {
					map[i + 2][j + 1].water[HERE] = 'r';
				}
				if (w) {
					map[i + 1][j].water[HERE] = 'r';
				}
				if (e) {
					map[i + 1][j + 2].water[HERE] = 'r';
				}
			}
			if (chunks[i / 3][j / 3] == 'w' || chunks[i / 3][j / 3] == 's') {
				map[i][j].water[HERE] = chunks[i / 3][j / 3];
			}
		}
	}
			
	char north, south, east, west;
	if (fill) {
		for (int i = 1; i < map.size() - 1; i++) {
			for (int j = 1; j < map[i].size() - 1; j++) {
				north = map[i - 1][j].water[HERE];
				south = map[i + 1][j].water[HERE];
				west = map[i][j - 1].water[HERE];
				east = map[i][j + 1].water[HERE];

				map[i][j].water[NORTH] = north;
				map[i][j].water[SOUTH] = south;
				map[i][j].water[WEST] = west;
				map[i][j].water[EAST] = east;

				if (map[i][j].type == 'r' && map[i][j].water[HERE] == 'e') {
					map[i][j].type = 'g';
				}
				if (map[i][j].type == 't') { //Town
					person_id = createTown(map, j, i, person_id);
					if (rand() % 2 == 0) {
						map[i][j].res.push_back(tree[rand() % tree.size()]);
						Utility campfire("campfire", 50, true);
						campfire.color = "*FIRE*";
						map[i][j].utilities.push_back(campfire);
					}
				}
				if (map[i][j].type == 'z') {
					map[i][j].type = 't';
				}

			}
		}
	}// FINAL PASS -> RIVERS, TOWNS

	for (int i = 0; i < 32; i++) {
		delete deer[i];
	}
	deer.clear();

	return map;
}