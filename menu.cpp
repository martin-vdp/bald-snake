#include "menu.hpp"
#include <iostream>

Menu::Menu()
{
	sf::Font font;

	if(!font.loadFromFile("Roboto-Black.ttf"))
		std::cerr << "Couldn't load the font file" << std::endl;

	for(unsigned int i=0;i<3;i++)
		text[i].setFont(font);
	
	text[0].setFillColor(sf::Color::Red);

	for(unsigned int i=0;i<2;i++)
	{
		text[i+1].setFillColor(sf::Color::White);
	}
	text[0].setString("Play");
	text[1].setString("Credit");
	text[2].setString("Exit");

	text[0].setPosition(sf::Vector2f(1000/2, 1000 / (3+1) * 1));
	text[1].setPosition(sf::Vector2f(1000/2, 1000 / (3+1) * 2));
	text[2].setPosition(sf::Vector2f(1000/2, 1000 / (3+1) * 3));

}

//bug here
void Menu::draw(sf::RenderWindow &window)
{
	for(unsigned int i=0;i<3;i++)
	{
		window.draw(text[i]);
	}
}

void Menu::MoveUp()
{}

void Menu::MoveDown()
{}

Menu::~Menu()
{

}
