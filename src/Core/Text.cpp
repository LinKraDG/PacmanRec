#include <Core/AssetManager.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <UI/Text.h>
#include <Utils/Constants.h>

void draw_text(bool center, float x, float y, const std::string& text, sf::RenderWindow& window)
{
	short text_x;
	short text_y;

	sf::Sprite text_sprite;

	sf::Texture* font_texture = AssetManager::getInstance()->loadTexture(FONT_SPRITES);

	char text_width = font_texture->getSize().x / 96;

	text_sprite.setTexture(*font_texture);

	if (center)
	{
		text_x = static_cast<float>(round(0.5f * (SPRITES_SIZE * MAP_WIDTH - text_width * text.substr(0, text.find_first_of('\n')).size())));
		text_y = static_cast<float>(round(0.5f * (SPRITES_SIZE * MAP_HEIGHT - FONT_HEIGHT * (1 + std::count(text.begin(), text.end(), '\n')))));
		if (y != 0) 
		{
			text_y += y;
		}
	}
	else
	{
		text_x = x;
		text_y = y;
	}

	for (std::string::const_iterator i = text.begin(); i != text.end(); i++)
	{
		if ('\n' == *i)
		{
			if (center)
			{
				text_x = static_cast<float>(round(0.5f * (SPRITES_SIZE * MAP_WIDTH - text_width * text.substr(1 + i - text.begin(), text.find_first_of('\n', 1 + i - text.begin()) - (1 + i - text.begin())).size())));
			}
			else
			{
				text_x = x;
			}

			text_y += FONT_HEIGHT;

			continue;
		}

		text_sprite.setPosition(text_x, text_y);
		text_sprite.setTextureRect(sf::IntRect(text_width * (*i - 32), 0, text_width, FONT_HEIGHT));

		text_x += text_width;

		window.draw(text_sprite);
	}

}