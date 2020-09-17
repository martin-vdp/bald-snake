#include <SFML/Graphics.hpp>
#include "menu.hpp"
constexpr unsigned int WIDTH=1000, HEIGHT=1000;

//a 1000px * 1000px window with 20 * 20 squares that are 50px * 50px each

int main()
{
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Bald Snake", sf::Style::Titlebar | sf::Style::Close);

	Menu menu;	

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{        
		if (event.type == sf::Event::Closed)
			window.close();
		}

		window.clear();
		
		// bug here
		menu.draw(window);

		window.display();
	}
	return 0;
}

