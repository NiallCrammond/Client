#pragma once
#include "SFML/Graphics.hpp"
struct Player
{
	float positionX =100;
	float positionY = 100;
	float size = 50;
	int clientID;
	int numOfClients;
	float serverTime;
};