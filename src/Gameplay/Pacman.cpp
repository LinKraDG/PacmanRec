#include <Core/AssetManager.h>
#include <Core/Map.h>
#include <Gameplay/Pacman.h>
#include <SFML/Graphics/RenderWindow.hpp>


bool Pacman::Init()
{
	pacman_texture = AssetManager::getInstance()->loadTexture(PACMAN_SPRITE);
	pacman_death_texture = AssetManager::getInstance()->loadTexture(PACMAN_DEATH_SPRITE);

	set_dead(false);

	score = 0;

	set_lives(3);

	player_sprite.setTexture(*pacman_texture);
	player_sprite.setPosition(position);

	return true;
}

void Pacman::update(std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& map, char level)
{
	std::array<bool, 4> walls{};
	walls[0] = map_collisions(false, false, PACMAN_SPEED + get_position().x, get_position().y, map, this);
	walls[1] = map_collisions(false, false, get_position().x, get_position().y - PACMAN_SPEED, map, this);
	walls[2] = map_collisions(false, false, get_position().x - PACMAN_SPEED, get_position().y, map, this);
	walls[3] = map_collisions(false, false, get_position().x, PACMAN_SPEED + get_position().y, map, this);

	set_direction_input(walls);

	if (!walls[get_direction()])
	{
		switch (get_direction())
		{
			case 0:
			{
				position.x += PACMAN_SPEED;
				break;
			}
			case 1:
			{
				position.y -= PACMAN_SPEED;
				break;
			}
			case 2:
			{
				position.x -= PACMAN_SPEED;
				break;
			}
			case 3:
			{
				position.y += PACMAN_SPEED;
				break;
			}
		}
	}

	if (get_position().x <= -SPRITES_SIZE)
	{
		set_position(SPRITES_SIZE * MAP_WIDTH - PACMAN_SPEED, get_position().y);
	}
	else if (get_position().x >= SPRITES_SIZE * MAP_WIDTH)
	{
		set_position(PACMAN_SPEED - SPRITES_SIZE, get_position().y);
	}

	if (map_collisions(1, 0, get_position().x, get_position().y, map, this))
	{
		set_energized_timer(static_cast<float>(ENERGIZER_DURATION / pow(2, level)));
	}
	else
	{
		set_energized_timer(std::max(.0f, get_energized_timer() - 1));
	}
}

void Pacman::render(sf::RenderWindow& window)
{
	current_frame = floor(get_current_animation_time() / PACMAN_ANIMATION_SPEED);

	if (get_dead())
	{
		if (get_lives() >= 1) {
			if (get_current_animation_time() < PACMAN_DEATH_FRAMES * PACMAN_ANIMATION_SPEED)
			{
				set_current_animation_time(get_current_animation_time() + 1);

				player_sprite.setTextureRect(sf::IntRect(SPRITES_SIZE * current_frame, SPRITES_SIZE * get_direction(), SPRITES_SIZE, SPRITES_SIZE));

				window.draw(player_sprite);
			}
			else
			{
				set_animation_game_over(true);
				set_dead(false);
			}
		}
		else
		{
			if (get_current_animation_time() < PACMAN_DEATH_FRAMES * PACMAN_ANIMATION_SPEED)
			{
				set_current_animation_time(get_current_animation_time() + 1);

				player_sprite.setTextureRect(sf::IntRect(SPRITES_SIZE * current_frame, SPRITES_SIZE * get_direction(), SPRITES_SIZE, SPRITES_SIZE));

				window.draw(player_sprite);
			}
			else
			{
				set_animation_game_over(true);
			}
		}
		
	}
	else if (get_victory())
	{
		if (get_current_animation_time() < PACMAN_DEATH_FRAMES * PACMAN_ANIMATION_SPEED)
		{
			set_current_animation_time(get_current_animation_time() + 1);

			player_sprite.setTextureRect(sf::IntRect(SPRITES_SIZE * current_frame, SPRITES_SIZE * get_direction(), SPRITES_SIZE, SPRITES_SIZE));

			window.draw(player_sprite);
		}
		else
		{
			set_animation_game_over(true);
		}
	}
	else
	{
		player_sprite.setTextureRect(sf::IntRect(SPRITES_SIZE * current_frame, SPRITES_SIZE * get_direction(), SPRITES_SIZE, SPRITES_SIZE));
		player_sprite.setPosition(position);
		window.draw(player_sprite);
		set_current_animation_time((1 + get_current_animation_time()) % (PACMAN_ANIMATION_FRAMES * PACMAN_ANIMATION_SPEED));
	}

}

void Pacman::reset()
{
	next_level();
	reset_score();
	set_lives(3);
}

void Pacman::reset_position()
{
	set_position(get_origin_position().x, get_origin_position().y);
	player_sprite.setTexture(*pacman_texture);
}

void Pacman::next_level()
{
	set_animation_game_over(false);
	set_dead(false);
	set_victory(false);

	set_direction(0);
	reset_position();

	set_current_animation_time(0);
	set_energized_timer(0);
	
}

sf::Vector2f Pacman::get_position()
{
	return position;
}

void Pacman::set_position(float x, float y)
{
	position = { x, y };
}

sf::Vector2f Pacman::get_origin_position()
{
	return origin_position;
}

void Pacman::set_origin_position(float x, float y)
{
	origin_position = { x, y };
}

bool Pacman::get_animation_game_over()
{
	return animation_game_over_overed;
}

void Pacman::set_animation_game_over(bool activated)
{
	animation_game_over_overed = activated;
}

bool Pacman::get_dead()
{
	return dead;
}

void Pacman::set_dead(bool death)
{
	dead = death;
	if (death == true)
	{
		player_sprite.setTexture(*pacman_death_texture);
		set_direction(0);
		set_current_animation_time(0);
	}
}

bool Pacman::get_victory()
{
	return victory;
}

void Pacman::set_victory(bool wined)
{
	victory = wined;
	player_sprite.setTexture(*pacman_death_texture);
	set_direction(0);
	set_current_animation_time(0);
}

char Pacman::get_direction()
{
	return movement_direction;
}

void Pacman::set_direction(char direction_number) 
{
	movement_direction = direction_number;
}

char Pacman::get_lives()
{
	return lives;
}

void Pacman::set_lives(char lives_number)
{
	lives = lives_number;
}

void Pacman::loose_live()
{
	lives = get_lives() - 1;
}

short Pacman::get_current_animation_time()
{
	return current_animation_time;
}

void Pacman::set_current_animation_time(short current_time)
{
	current_animation_time = current_time;
}

float Pacman::get_energized_timer()
{
	return current_energized_time;
}

void Pacman::set_energized_timer(float current_time)
{
	current_energized_time = current_time;
}

short Pacman::get_score() 
{
	return score;
}

void Pacman::set_score(short score_bonus) 
{
	score += score_bonus;
}

void Pacman::reset_score() 
{
	score = .0f;
}

void Pacman::set_direction_input(std::array<bool, 4> walls)
{

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if (walls[0] == false)
		{
			set_direction(0);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (walls[1] == false)
		{
			set_direction(1);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (walls[2] == false)
		{
			set_direction(2);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (walls[3] == false)
		{
			set_direction(3);
		}
	}	
}




