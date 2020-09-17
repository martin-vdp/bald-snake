#ifndef MENU_HPP
#define MENU_HP

#include <SFML/Graphics.hpp>

class Menu
{
public:
	Menu();
	~Menu();

	void draw(sf::RenderWindow &window);
	void MoveUp();
	void MoveDown();

private:
	sf::Font font;
	sf::Text text[3];
	enum selectedItem{ first = 0, second = 1, third = 2 };
	
};

#endif
