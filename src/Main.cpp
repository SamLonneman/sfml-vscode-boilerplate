#include "Board.h"
#include "TextureManager.h"
#include <SFML/Graphics.hpp>
using namespace std;

int main()
{
	// Seed the randomizer
	srand(time(0));

	//SFML START
	Board board;
	sf::RenderWindow window(sf::VideoMode(1080, 720), "Maze Runner");
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					board.leftClick(sf::Mouse::getPosition(window), window);
				}
			}
		}
		window.clear(sf::Color(173, 216, 230, 255));
		board.Draw(window);
		window.display();
	}
	TextureManager::Clear();
	return 0;
}
