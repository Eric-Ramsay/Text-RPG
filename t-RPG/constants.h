#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "olcPixelGameEngine.h"

//Colors
/*
WHITE, GREY, RED, 
ORANGE, YELLOW, GREEN, 
TEAL, BLUE, PURPLE, 
BROWN, PINK, TURQUOISE
*/

olc::Pixel GREY(133, 168, 178);
olc::Pixel WHITE(229, 223, 210);
olc::Pixel RED(185, 50, 50);
olc::Pixel ORANGE(199, 115, 45);
olc::Pixel YELLOW(246, 219, 83);
olc::Pixel GREEN(45, 255, 80);
olc::Pixel TEAL(0, 255, 255);
olc::Pixel BLUE(25, 167, 255);
olc::Pixel PURPLE(171, 164, 225);
olc::Pixel BROWN(139, 76, 45);
olc::Pixel PINK(237, 190, 190);
olc::Pixel FISH(83, 167, 142);
olc::Pixel GRASS(76, 182, 92);
olc::Pixel BLACK(15, 18, 22);
olc::Pixel TAN(193, 174, 124);
olc::Pixel DARK_GREEN(44, 104, 53);

//Dice
olc::Pixel D4(59, 76, 83);
olc::Pixel D6(71, 99, 111);
olc::Pixel D8(86, 121, 135);
olc::Pixel D10(119, 165, 185);
olc::Pixel D12(169, 198, 204);
olc::Pixel D14(255, 255, 255);

olc::Pixel bg_grey(171, 190, 207);

olc::Pixel FIRE_1(255, 123, 38);
olc::Pixel FIRE_2(255, 212, 38);
olc::Pixel FIRE_3(229, 161, 11);
olc::Pixel FIRE_4(255, 59, 38);
olc::Pixel FIRE_5(255, 84, 42);

const int INVENTORY_SIZE = 9;
const int TEXT_W = 3;
const int TEXT_H = 5;

const int MAP_W = 160;
const int MAP_H = 90;
const int SCALE = 3;

struct UItem;
struct Building;
struct Attack;

std::vector<UItem> items;
std::vector<Building> buildings;
std::vector<Attack> attacks;

#endif