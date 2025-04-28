#include <Core/Map.h>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Network/Packet.hpp>
#include <sstream>




std::array<std::string, MAP_HEIGHT> load_map(char* map_sketch_file)
{
	std::array<std::string, MAP_HEIGHT> map_sketch;
	std::string line;
	std::ifstream infile(map_sketch_file);
	
	int a = 0;
	for (std::string line; std::getline(infile, line);)
	{
		map_sketch[a] = line;
		a++;
	}

	infile.close();

	return map_sketch;
}

void draw_map(const std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& i_map, sf::RenderWindow& i_window)
{
	sf::Sprite sprite;

	sf::Texture texture;
	texture.loadFromFile(MAP);

	sprite.setTexture(texture);

	for (char a = 0; a < MAP_WIDTH; a++)
	{
		for (char b = 0; b < MAP_HEIGHT; b++)
		{
			sprite.setPosition(static_cast<float>(SPRITES_SIZE * a), static_cast<float>(SPRITES_SIZE * b));

			switch (i_map[a][b])
			{
			case map_sprites::Door:
			{
				sprite.setTextureRect(sf::IntRect(2 * SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE));

				i_window.draw(sprite);

				break;
			}
			case map_sprites::Energizer:
			{
				sprite.setTextureRect(sf::IntRect(SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE));

				i_window.draw(sprite);

				break;
			}
			case map_sprites::Pellet:
			{
				sprite.setTextureRect(sf::IntRect(0, SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE));

				i_window.draw(sprite);

				break;
			}
			case map_sprites::Wall:
			{
				bool down = 0;
				bool left = 0;
				bool right = 0;
				bool up = 0;

				if (b < MAP_HEIGHT - 1)
				{
					if (map_sprites::Wall == i_map[a][1 + b])
					{
						down = 1;
					}
				}

				if (0 < a)
				{
					if (map_sprites::Wall == i_map[a - 1][b])
					{
						left = 1;
					}
				}
				else
				{
					left = 1;
				}

				if (a < MAP_WIDTH - 1)
				{
					if (map_sprites::Wall == i_map[1 + a][b])
					{
						right = 1;
					}
				}
				else
				{
					right = 1;
				}

				if (0 < b)
				{
					if (map_sprites::Wall == i_map[a][b - 1])
					{
						up = 1;
					}
				}

				sprite.setTextureRect(sf::IntRect(SPRITES_SIZE * (down + 2 * (left + 2 * (right + 2 * up))), 0, SPRITES_SIZE, SPRITES_SIZE));

				i_window.draw(sprite);
			}
			}
		}
	}
}

bool map_collisions(bool collect_pellets, bool use_door, short map_x, short map_y, std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& map, Pacman* player)
{
	bool output = false;

	float cell_x = map_x / static_cast<float>(SPRITES_SIZE);
	float cell_y = map_y / static_cast<float>(SPRITES_SIZE);

	for (char a = 0; a < 4; a++)
	{
		float x = .0f;
		float y = .0f;

		switch (a)
		{
			case 0:
			{
				x = static_cast<float>(floor(cell_x));
				y = static_cast<float>(floor(cell_y));
				break;
			}
			case 1:
			{
				x = static_cast<float>(ceil(cell_x));
				y = static_cast<float>(floor(cell_y));
				break;
			}
			case 2:
			{
				x = static_cast<float>(floor(cell_x));
				y = static_cast<float>(ceil(cell_y));
				break;
			}
			case 3:
			{
				x = static_cast<float>(ceil(cell_x));
				y = static_cast<float>(ceil(cell_y));
				break;
			}
		}

		if (x >= 0 && y >= 0 && y < MAP_HEIGHT && x < MAP_WIDTH)
		{
			if (collect_pellets == false)
			{
				if (map_sprites::Wall == map[x][y])
				{
					output = true;
				}
				else if (use_door == false && map_sprites::Door == map[x][y])
				{
					output = true;
				}
			}
			else 
			{
				if (map_sprites::Energizer == map[x][y])
				{
					output = true;

					map[x][y] = map_sprites::Empty;
					player->set_score(50);
				}
				else if (map_sprites::Pellet == map[x][y])
				{
					map[x][y] = map_sprites::Empty;
					player->set_score(10);
				}
			}
		}
	}

	return output;
}

std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH> convert_sketch(const std::array<std::string, MAP_HEIGHT>& map_sketch, std::array<Ghost*, 4> &ghosts, Pacman * player)
{
	std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH> output_map{};
	for (char a = 0; a < MAP_HEIGHT; a++)
	{
		for (char b = 0; b < MAP_WIDTH; b++)
		{
			output_map[b][a] = map_sprites::Empty;

			switch (map_sketch[a][b])
			{
				case '#':
				{
					output_map[b][a] = map_sprites::Wall;

					break;
				}
				case '=':
				{
					output_map[b][a] = map_sprites::Door;

					break;
				}
				case '.':
				{
					output_map[b][a] = map_sprites::Pellet;

					break;
				}
				case '0':
				{
					ghosts[0]->set_origin_position(SPRITES_SIZE * b, SPRITES_SIZE * a);

					for (Ghost* ghost : ghosts)
					{
						ghost->set_home_door(SPRITES_SIZE * b, SPRITES_SIZE * a);
					}

					break;
				}
				case '1':
				{
					ghosts[1]->set_origin_position(SPRITES_SIZE * b, SPRITES_SIZE * a);
					break;
				}
				case '2':
				{
					ghosts[2]->set_origin_position(SPRITES_SIZE * b, SPRITES_SIZE * a);

					for (Ghost *ghost : ghosts)
					{
						ghost->set_home(SPRITES_SIZE * b, SPRITES_SIZE * a);
					}

					break;
				}
				case '3':
				{
					ghosts[3]->set_origin_position(SPRITES_SIZE * b, SPRITES_SIZE * a);
					break;
				}
				case 'P':
				{
					player->set_origin_position(SPRITES_SIZE * b, SPRITES_SIZE * a);
					player->set_position(SPRITES_SIZE * b, SPRITES_SIZE * a);
					break;
				}
				case 'o':
				{
					output_map[b][a] = map_sprites::Energizer;
				}
			}
		}
	}
	return output_map;
}

void draw_pacman_lives(Pacman* player, sf::RenderWindow& window)
{
	sf::Sprite sprite;
	sf::Texture texture;

	texture.loadFromFile(PACMAN_SPRITE);
	sprite.setTexture(texture);

	sprite.setTextureRect(sf::IntRect(SPRITES_SIZE * 3, SPRITES_SIZE * 2, SPRITES_SIZE, SPRITES_SIZE));

	for (char i = player->get_lives(); i >= 1; i--)
	{
		sprite.setPosition(SPRITES_SIZE * MAP_WIDTH - SPRITES_SIZE * (i + 1), SPRITES_SIZE * MAP_HEIGHT);
		window.draw(sprite);
	}
}