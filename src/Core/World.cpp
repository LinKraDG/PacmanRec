#include <Core/AssetManager.h>
#include <Core/Map.h>
#include <Core/World.h>
#include <Gameplay/Ghost.h>
#include <Gameplay/Pacman.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <UI/Text.h>
#include <Utils/Constants.h>

World::~World()
{
	delete pacman;

	for (Ghost* ghost : ghosts)
	{
		delete ghost;
	}
}

bool World::load()
{
	set_level(0);
	set_victory(false);

	set_wave(0);
	set_wave_time(LONG_SCATTER_DURATION);

	Pacman* c_pacman = new Pacman();
	const bool initOk = c_pacman->Init();
	
	ghosts = { ghost_0 = new Ghost(0), ghost_1 = new Ghost(1), ghost_2 = new Ghost(2), ghost_3 = new Ghost(3)};

	set_blueprint(MAP_SKETCH);

	pacman = c_pacman;

	set_map(get_blueprint());

	level_reset();

	return initOk;
}

void World::level_reset() 
{
	set_victory(false);

	set_wave(0);
	set_wave_time(static_cast<float>(LONG_SCATTER_DURATION / pow(2, get_level())));

	for (Ghost *ghost : ghosts)
	{
		ghost->reset();
	}

	pacman->reset_position();
	
}

void World::update(uint32_t deltaMilliseconds)
{
	if (get_victory() == false && pacman->get_dead() == false)
	{
		set_victory(true);

		pacman->update(*get_map(), get_level());

		for (Ghost* ghost : ghosts)
		{
			prev_update_ghost(ghost);
			ghost->update(get_level(), *get_map(), *ghosts[0], pacman);
		}

		for (const std::array<map_sprites, MAP_HEIGHT>& column : *get_map())
		{
			for (const map_sprites& cell : column)
			{
				if (map_sprites::Pellet == cell)
				{
					set_victory(false);
					break;
				}
			}

			if (!get_victory())
			{
				break;
			}
		}

		if (get_victory())
		{
			pacman->set_current_animation_time(0);
			pacman->set_victory(true);
		}
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		set_victory(false);

		if (pacman->get_dead())
		{
			set_level(0);
			pacman->reset();
		}
		else
		{
			set_level(get_level()+1);
			pacman->next_level();
		}

		set_map(get_blueprint());

		level_reset();
	}
}

void World::render(sf::RenderWindow& window)
{
	if (!get_victory() && !pacman->get_dead())
	{
		draw_map(*get_map(), window);
		for (Ghost* ghost : ghosts)
		{
			ghost->draw(GHOST_FLASH_START >= pacman->get_energized_timer(), window);
		}
		draw_text(0, 0, SPRITES_SIZE * MAP_HEIGHT, "Nivel: " + std::to_string(1 + level), window);
		draw_text(0, 0, SPRITES_SIZE * MAP_HEIGHT + SPRITES_SIZE, "Puntuacion: " + std::to_string(pacman->get_score()), window);
		draw_pacman_lives(pacman, window);
	}

	pacman->render(window);
	
	if (pacman->get_animation_game_over())
	{
		if (get_victory())
		{
			draw_text(1, 0, 0, "Siguiente nivel!", window);
			draw_text(1, SPRITES_SIZE, SPRITES_SIZE, "Enter para continuar", window);
		}
		else if (pacman->get_lives() >= 1)
		{
			pacman->reset_position();
			pacman->set_animation_game_over(false);
			for (Ghost* ghost : ghosts)
			{
				ghost->reset_position();
			}
		}
		else
		{
			draw_text(1, 0, 0, "Has perdido", window);
			draw_text(1, SPRITES_SIZE, SPRITES_SIZE, "Enter para continuar", window);
		}
	}
}

std::array<std::string, MAP_HEIGHT> World::get_blueprint()
{
	return map_blueprint;
}

void World::set_blueprint(char *map)
{
	map_blueprint = load_map(map);
}

std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>* World::get_map()
{
	return &created_map;
}

void World::set_map(std::array<std::string, MAP_HEIGHT> map)
{
	created_map = convert_sketch(map, ghosts, pacman);
}


/*sf::Vector2f World::get_home()
{
	return home_position;
}

void World::set_home(sf::Vector2f home)
{
	home_position = home;
}*/

bool World::get_victory()
{
	return game_woned;
}

void World::set_victory(bool victory)
{
	game_woned = victory;
}

char World::get_wave()
{
	return wave;
}

void World::set_wave(int current_wave)
{
	wave = current_wave;
}

char World::get_level()
{
	return level;
}

void World::set_level(int level_number)
{
	level = level_number;
}

short World::get_wave_time()
{
	return wave_time;
}

void World::set_wave_time(short time)
{
	wave_time = time;
}

void World::prev_update_ghost(Ghost* ghost)
{
	if (pacman->get_energized_timer() == 0)
	{
		if (get_wave_time() == 0)
		{
			if (get_wave() < 7)
			{
				set_wave(get_wave() + 1);
				ghost->change_mode();
			}

			if (get_wave() % 2 == 1)
			{
				set_wave_time(CHASE_DURATION);
			}
			else if (get_wave() == 2)
			{
				set_wave_time(static_cast<short>(LONG_SCATTER_DURATION / pow(2, level)));
			}
			else
			{
				set_wave_time(static_cast<short>(SHORT_SCATTER_DURATION / pow(2, level)));
			}
		}
		else
		{
			set_wave_time(get_wave_time() - 1);
		}
	}
}
