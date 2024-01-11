
#include"Client.h"

using namespace std;


sf::Packet& operator <<(sf::Packet& packet, const Player& player)
{
	return packet << player.positionX << player.positionY << player.size << player.clientID << player.numOfClients;
}

sf::Packet& operator >>(sf::Packet& packet, Player& player)
{
	return packet >> player.positionX >> player.positionY >> player.size >> player.clientID >> player.numOfClients;
}
int main()
{

	sf::RenderWindow window(sf::VideoMode(800, 600), "My SFML Window");
	window.setFramerateLimit(60);

	sf::TcpSocket socket;
	sf::SocketSelector selector;
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();

	food = new sf::RectangleShape();

	food->setFillColor(sf::Color::Magenta);
	food->setPosition(sf::Vector2f(500, 300));
	food->setSize(sf::Vector2f(10, 10));

	
	sf::Socket::Status status = socket.connect(ip, 3000);
	if (status != sf::Socket::Done)
	{

		std::cout << "Failed to connect" << endl;
	}

	std::cout << "Socket Connected to port" << endl;

	sf::Packet idPacket;
	socket.receive(idPacket);
	idPacket >> playerID;


	std::cout << "Players ID is: " << playerID << endl;

	
	localPlayer.clientID = playerID;

	selector.add(socket);
	socket.setBlocking(false);
	
	while (window.isOpen())
	{
		
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
		deltaTime = clocker.restart();
		elapsedTime += deltaTime;
		totalTime += deltaTime;

		handleInput(deltaTime, playerID);
		//handleNetwork(socket);
		//std::cout << "Input pased" << std::endl;
	
	

		handleSend(socket);

		if (selector.wait(sf::milliseconds(16)))
		{
			if (selector.isReady(socket))
			{
		handleReceive(socket);

			}
		}
		
		
		predictLocalPlayer();

		
		
		
		

		

			
		
		handleRender(window);


	}
		


	


}

void handleNetwork(sf::TcpSocket &socket)
{


	if (elapsedTime >= sendInterval)
	{

		sf::Packet sendPacket;
		sendPacket << localPlayer;
		if (socket.send(sendPacket) != sf::Socket::Done)
		{
			printf("send failed");
		}

		sendPacket.clear();

		if (playerMap.empty())
		{
			playerMap[playerID] = localPlayer;
		}

		for (const auto& pair : playerMap)
		{


			sf::Packet receivedPacket;
			socket.receive(receivedPacket);

			std::cout << "entereed map loop";
			Player receivedPlayer;
			receivedPacket >> receivedPlayer;
			playerMap[receivedPlayer.clientID] = receivedPlayer;

			if (receivedPlayer.clientID == playerID)
			{
				localPlayer = receivedPlayer;
			}
			else
			{
				playerMap[receivedPlayer.clientID] = receivedPlayer;
			}



			std::cout << "Received The following information for Player: " << receivedPlayer.clientID << std::endl
				<< "PlayerPosition X: " << receivedPlayer.positionX << std::endl
				<< "PlayerPosition Y: " << receivedPlayer.positionY << std::endl
				<< "Player size: " << receivedPlayer.size << std::endl
				<< "Number of Clients: " << receivedPlayer.numOfClients << std::endl;

			receivedPacket.clear();

		}


	}
}

void handleReceive(sf::TcpSocket& socket)
{

	if (playerMap.empty())
	{
		playerMap[playerID] = localPlayer;
	}
	int clientSize = playerID;

	for (const auto& pair : playerMap)
	{
		
		sf::Clock receiveClock;
		sf::Time receiveTime;

		sf::Packet receivedPacket;
		if (socket.receive(receivedPacket) == sf::Socket::Done)
		{


			//std::cout << "entereed map loop";
			Player receivedPlayer;
			receivedPacket >> receivedPlayer;
			clientSize = receivedPlayer.numOfClients;
			playerMap[receivedPlayer.clientID] = receivedPlayer;

			if (receivedPlayer.clientID == playerID)
			{
				receiveTime = receiveClock.getElapsedTime();
				receivedMessages.push_back(receivedPacket);
				localPlayer = receivedPlayer;
			}
			else
			{
				playerMap[receivedPlayer.clientID] = receivedPlayer;
			}



			std::cout << "Received The following information for Player: " << receivedPlayer.clientID << std::endl
				<< "PlayerPosition X: " << receivedPlayer.positionX << std::endl
				<< "PlayerPosition Y: " << receivedPlayer.positionY << std::endl
				<< "Player size: " << receivedPlayer.size << std::endl
				<< "Number of Clients: " << receivedPlayer.numOfClients << std::endl << std::endl << std::endl;

			receivedPacket.clear();
		}

	}

}

void handleSend(sf::TcpSocket& socket)
{
	if (elapsedTime >= sendInterval)
	{



		sf::Packet sendPacket;
		sendPacket << localPlayer;
		if (socket.send(sendPacket) != sf::Socket::Done)
		{
			printf("send failed");
		}

		sendPacket.clear();
		elapsedTime = sf::Time::Zero;
	}
}

void handleCollision(sf::CircleShape& playerCircle, sf::RectangleShape& foodRect, Player& player)
{
	if (playerCircle.getGlobalBounds().intersects(foodRect.getGlobalBounds()))
	{
		food->setPosition(sf::Vector2f(rand()% 800, rand()%400));
		food->setScale(2.0f, 2.0f);
		if (player.clientID == playerID)
		{


			localPlayer.size += 5;
			playerCircle.setRadius(localPlayer.size);
		}
	}
}

void predictLocalPlayer()
{

	if (elapsedTime >= sendInterval)
	{


		float predX, predY;

		const int messageSize = receivedMessages.size();
		if (messageSize < 3)
		{
			return;
		}

		Player msg0;
		receivedMessages[messageSize - 1] >> msg0;
		Player msg1;
		receivedMessages[messageSize - 2] >> msg1;

		sf::Vector2f recentMessage(msg0.positionX, msg0.positionY);
		sf::Vector2f previousMessage(msg1.positionX, msg1.positionY);


	

	

		float velocityX = (recentMessage.x - previousMessage.x);
		float velocityY = (recentMessage.y - previousMessage.y);




		predX = recentMessage.x + (velocityX*deltaTime.asSeconds());
		predY = recentMessage.y + (velocityY* deltaTime.asSeconds());




		localPlayer.positionX = predX;
		localPlayer.positionY = predY;
	}
}

void handleInput(sf::Time dt, int ID)
{
	if (playerID == 1)
	{


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			localPlayer.positionX -= 1000 * deltaTime.asSeconds();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			localPlayer.positionX += 1000 * deltaTime.asSeconds();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			localPlayer.positionY -= 1000 * deltaTime.asSeconds();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			localPlayer.positionY += 1000 * deltaTime.asSeconds();
		}
	}

	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			localPlayer.positionX -= 1000 * deltaTime.asSeconds();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			localPlayer.positionX += 1000 * deltaTime.asSeconds();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			localPlayer.positionY -= 1000 * deltaTime.asSeconds();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			localPlayer.positionY += 1000 * deltaTime.asSeconds();
		}

	}
}

void handleRender(sf::RenderWindow& window)
{

	window.clear();

		for (auto it = playerMap.begin(); it != playerMap.end(); it++)
		{
			const int playerID = it->first;
			 Player& player = it->second;
			sf::CircleShape circle;
			circle.setPosition(player.positionX, player.positionY);
			circle.setFillColor(colours[playerID]);
			circle.setRadius(player.size);
			handleCollision(circle, *food, player);
			window.draw(circle);
			window.draw(*food);

		}
		window.display();
	
}


