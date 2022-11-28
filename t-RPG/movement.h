#pragma once


double tileCost(char t) {
	if (t == 'w') {
		return 1000.0;
	}
	else if (t == 's') {
		return 1;
	}
	return .5;
}

std::deque<Spot> pathfind(std::vector<std::vector<Tile>>& map, Spot start, Spot end) {
	std::deque<Spot> open;
	std::deque<Spot> closed;
	std::deque<Spot> path;

	short size = 0;

	if (start == end) {
		return {};
	}

	Spot current = start;
	Spot consider;

	current.g = 0;
	current.h = dist(start, end);
	open = { current };

	bool valid = true;
	int index = 0;

	while (open.size() > 0 && open.size() < 100) {
		index = 0;
		size = open.size();
		current = open[0];
		for (int i = 1; i < size; i++) {
			consider = open[i];
			if (consider.g + consider.h < current.g + current.h) {
				index = i;
				current = open[index];
			}
		}
		consider = open.back();
		open.back() = current;
		open[index] = consider;
		open.pop_back();

		closed.push_back(current);

		if (current == end) { //At location. Return path.
			index = closed.size() - 1;
			while (index > 0) {
				path.push_front(closed[index]);
				index = closed[index].parent;
			}
			return path;
		}

		for (int a = -1; a < 2; a++) {
			for (int b = -1; b < 2; b++) {
				if (a != 0 || b != 0) { //only check 4 adjacent tiles
					consider = Spot(current.x + a, current.y + b);

					consider.parent = closed.size() - 1; //Index of parent node, aka current's index in the closed list.
					consider.g = current.g; //Distance to start
					consider.g += tileCost(map[consider.y][consider.x].type);
					consider.h = dist(consider, end);
					valid = true;

					for (Spot s : open) {
						if (s.x == consider.x && s.y == consider.y) {
							valid = false;
							break;
						}
					}
					if (valid) {
						for (Spot s : closed) {
							if (s.x == consider.x && s.y == consider.y) {
								valid = false;
								break;
							}
						}
					}
					if (valid) { //Location has not been considered before. Add it to the open list.
						open.push_back(consider);
					}
				}
			}
		}
	}
	return {};
}


std::deque<Spot> greedy_pathfind(Spot start, Spot end) {
	std::deque<Spot> path;
	Spot current = start;
	
	while (current != end) {
		if (current.x > end.x) {
			current.x -= 1;
		}
		else if (current.x < end.x) {
			current.x += 1;
		}
		else if (current.y < end.y) {
			current.y += 1;
		}
		else if (current.y > end.y) {
			current.y -= 1;
		}
		path.push_back(current);
	}
	return path;
}

void Move(Person* p, Tile& from, Tile& to) {

	p->y = to.y;
	p->x = to.x;
	bool skip = false;

	for (Spot s : p->recent) {
		if (s.x == to.x && s.y == to.y) {
			skip = true;
			break;
		}
	}
	if (!skip) {
		p->recent.push_back(Spot(to.x, to.y));
		if (p->recent.size() > 8) {
			p->recent.pop_front();
		}
	}

	p->move = HERE;

	to.people.push_back(p);

	for (int i = 0; i < from.people.size(); i++) {
		if (from.people[i] == p) {
			from.people.erase(from.people.begin() + i);
			return;
		}
	}
}

Direction spotMovement(Spot from, Spot to) {
	if (from.y > to.y) {
		return NORTH;
	}
	else if (from.y < to.y) {
		return SOUTH;

	}
	else if (from.x < to.x) {
		return EAST;
	}
	else if (from.x > to.x) {
		return WEST;
	}
	return HERE;
}

