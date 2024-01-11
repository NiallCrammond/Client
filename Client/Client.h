#pragma once
#include "SFML/Network.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include <iostream>
#include<Windows.h>
#include "Player.h"
#include <thread>
#include <map>

enum MessageType
{
	PlayerInfo,
	FoodSpawm
};

struct foodInfo
{
	int positionX;
	int positionY;

};


void handleNetwork(sf::TcpSocket &socket);
void handleInput(sf::Time dt, int ID);
void handleRender(sf::RenderWindow& window);
void handleReceive(sf::TcpSocket& socket);
void handleSend(sf::TcpSocket& socket);
void handleCollision(sf::CircleShape& playerCircle, sf::RectangleShape& foodRect, Player& player);
void predictLocalPlayer();

sf::Color colours[] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::White};
sf::Clock clocker;
const sf::Time sendInterval = sf::seconds(1.0/60.0f);
const sf::Time predictInterval = sf::seconds(1.0 / 10.0f);

sf::Time elapsedTime;
sf::Time deltaTime;
sf::Time totalTime;

int playerID;
bool isFood = true;

sf::RectangleShape* food;
Player localPlayer;
Player receivedLocalPlayer;
std::map<int, Player> playerMap;
std::map<int, sf::CircleShape> circleMap;
std::vector<sf::Packet> receivedMessages;
float updateInterval = 0.0167;