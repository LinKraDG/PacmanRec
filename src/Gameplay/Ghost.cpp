#include <array>
#include <Core/AssetManager.h>
#include <Core/Map.h>
#include <Gameplay/Ghost.h>

Ghost::Ghost(char ghost_id) : id(ghost_id)
{
	ghostTexture = AssetManager::getInstance()->loadTexture(GHOSTS_SPRITE);
	body.setTexture(*ghostTexture);
	face.setTexture(*ghostTexture);
}

void Ghost::update(char level, std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& map, Ghost& ghost_ref, Pacman* pacman)
{
	bool move = false;

	char available_ways = 0;
	char speed = GHOST_SPEED;

	std::array<bool, 4> walls{};

	if (get_frightened_mode() == 0 && pacman->get_energized_timer() == ENERGIZER_DURATION / pow(2, level))
	{
		set_frightened_time(GHOST_FRIGHTENED_SPEED);

		set_frightened_mode(1);
	}
	else if (pacman->get_energized_timer() == 0 && get_frightened_mode() == 1)
	{
		set_frightened_mode(0);
	}

	if (get_frightened_mode() == 2 && fmod(get_position().x, GHOST_ESCAPE_SPEED) == 0 && fmod(get_position().y, GHOST_ESCAPE_SPEED) == 0)
	{
		speed = GHOST_ESCAPE_SPEED;
	}

	update_target(pacman->get_direction(), ghost_ref.get_position(), pacman->get_position());

	walls[0] = map_collisions(0, get_inside_home(), speed + get_position().x, get_position().y, map, pacman);
	walls[1] = map_collisions(0, get_inside_home(), get_position().x, get_position().y - speed, map, pacman);
	walls[2] = map_collisions(0, get_inside_home(), get_position().x - speed, get_position().y, map, pacman);
	walls[3] = map_collisions(0, get_inside_home(), get_position().x, speed + get_position().y, map, pacman);

	if (get_frightened_mode() != 1)
	{
		char optimal_direction = 4;

		move = true;

		for (char a = 0; a < 4; a++)
		{
			if (a == (2 + get_direction()) % 4)
			{
				continue;
			}
			else if (0 == walls[a])
			{
				if (4 == optimal_direction)
				{
					optimal_direction = a;
				}

				available_ways++;

				if (get_pacman_distance(a) < get_pacman_distance(optimal_direction))
				{
					optimal_direction = a;
				}
			}
		}

		if (1 < available_ways)
		{
			set_direction(optimal_direction);
		}
		else
		{
			if (4 == optimal_direction)
			{
				set_direction((2 + get_direction()) % 4);
			}
			else
			{
				set_direction(optimal_direction);
			}
		}
	}
	else
	{
		char random_direction = rand() % 4;

		if (get_frightened_time() == 0)
		{
			move = true;

			set_frightened_time(GHOST_FRIGHTENED_SPEED);

			for (char a = 0; a < 4; a++)
			{
				if (a == (2 + get_direction()) % 4)
				{
					continue;
				}
				else if (0 == walls[a])
				{
					available_ways++;
				}
			}

			if (0 < available_ways)
			{
				while (1 == walls[random_direction] || random_direction == (2 + get_direction()) % 4)
				{
					random_direction = rand() % 4;
				}

				set_direction(random_direction);
			}
			else
			{
				set_direction((2 + get_direction()) % 4);
			}
		}
		else
		{
			set_frightened_time(get_frightened_time() - 1);
		}
	}

	if (move)
	{
		switch (get_direction())
		{
			case 0:
			{
				set_position(get_position().x + speed, get_position().y);
				break;
			}
			case 1:
			{
				set_position(get_position().x, get_position().y - speed);
				break;
			}
			case 2:
			{
				set_position(get_position().x - speed, get_position().y);
				break;
			}
			case 3:
			{
				set_position(get_position().x, get_position().y + speed);
				break;
			}
		}

		if (get_position().x <= -SPRITES_SIZE)
		{
			set_position(SPRITES_SIZE* MAP_WIDTH - speed, get_position().y);
		}
		else if (get_position().x >= SPRITES_SIZE * MAP_WIDTH)
		{
			set_position(speed - SPRITES_SIZE, get_position().y);
		}
	}

	if (pacman_hit(pacman->get_position()))
	{
		if (get_frightened_mode() == 0)
		{
			if (pacman->get_lives() >= 1)
			{
				pacman->loose_live();
				pacman->set_dead(true);
			}
			else 
			{
				pacman->loose_live();
				pacman->set_dead(true);
			}
		}
		else if (get_frightened_mode() == 1) 
		{
			pacman->set_score(200);
			set_inside_home(true);
			set_frightened_mode(2);
			back_home();
		}
	}
}

void Ghost::reset()
{
	set_movement_mode(0);
	if (get_id() == 0) 
	{
		set_inside_home(false);
	}
	else
	{
		set_inside_home(true);
	}

	reset_position();

	set_direction(0);
	set_frightened_mode(0);
	set_frightened_time(0);

	set_animation_time(0);

	set_target(get_home_door().x, get_home_door().y);
}

void Ghost::reset_position()
{
	set_position(get_origin_position().x, get_origin_position().y);
	if (get_id() == 0)
	{
		set_inside_home(false);
	}
	else
	{
		set_inside_home(true);
	}
	set_target(get_home_door().x, get_home_door().y);

}



bool Ghost::pacman_hit(const sf::Vector2f& pacman_position)
{
	if (get_position().x > pacman_position.x - SPRITES_SIZE && get_position().x < SPRITES_SIZE + pacman_position.x)
	{
		if (get_position().y > pacman_position.y - SPRITES_SIZE && get_position().y < SPRITES_SIZE + pacman_position.y)
		{
			return true;
		}
	}

	return false;
}

float Ghost::get_pacman_distance(char target_direction)
{
	short x = get_position().x;
	short y = get_position().y;

	switch (target_direction)
	{
		case 0:
		{
			x += GHOST_SPEED;
			break;
		}
		case 1:
		{
			y -= GHOST_SPEED;
			break;
		}
		case 2:
		{
			x -= GHOST_SPEED;
			break;
		}
		case 3:
		{
			y += GHOST_SPEED;
			break;
		}
	}

	return static_cast<float>(sqrt(pow(x - get_target().x, 2) + pow(y - get_target().y, 2)));
}

void Ghost::draw(bool flash, sf::RenderWindow& window)
{
	char body_frame = static_cast<char>(floor(get_animation_time() / static_cast<float>(GHOST_ANIMATION_SPEED)));

	switch (get_id())
	{
		case 0:
		{
			set_body(sf::Color(255, 0, 0), body_frame, flash, window);
			set_face(body_frame, flash, window);
			break;
		}
		case 1:
		{
			set_body(sf::Color(255, 182, 255), body_frame, flash, window);
			set_face(body_frame, flash, window);
			break;
		}
		case 2:
		{
			set_body(sf::Color(0, 255, 255), body_frame, flash, window);
			set_face(body_frame, flash, window);
			break;
		}
		case 3:
		{
			set_body(sf::Color(255, 182, 85), body_frame, flash, window);
			set_face(body_frame, flash, window);
			break;
		}
	}

	

	set_animation_time((1 + get_animation_time()) % (GHOST_ANIMATION_FRAMES * GHOST_ANIMATION_SPEED));
}

void Ghost::update_target(char pacman_direction, const sf::Vector2f& ghost_ref_position, const sf::Vector2f& pacman_position)
{
	if (get_inside_home())
	{
		if (get_position() == get_target())
		{
			if (get_home_door() == get_target())
			{
				set_inside_home(false);
			}
			else if (get_home() == get_target())
			{
				set_frightened_mode(0);

				set_target(get_home_door().x, get_home_door().y);
			}
		}
	}
	else
	{
		if (get_movement_mode()==false) 
		{
			switch (get_id())
			{
				case 0:
				{
					set_target(SPRITES_SIZE * (MAP_WIDTH - 1), 0);
					break;
				}
				case 1:
				{
					set_target(0, 0);
					break;
				}
				case 2:
				{
					set_target(SPRITES_SIZE * (MAP_WIDTH - 1), SPRITES_SIZE * (MAP_HEIGHT - 1));
					break;
				}
				case 3:
				{
					set_target(0, SPRITES_SIZE * (MAP_HEIGHT - 1));
					break;
				}
			}
		}
		else
		{
			switch (get_id())
			{
				case 0:
				{
					set_target(pacman_position.x, pacman_position.y);
					break;
				}
				case 1:
				{
					set_target(pacman_position.x, pacman_position.y);

					switch (pacman_direction)
					{
						case 0:
						{
							set_target(get_target().x + SPRITES_SIZE * GHOST_1_CHASE, get_target().y);
							break;
						}
						case 1:
						{
							set_target(get_target().x, get_target().y - SPRITES_SIZE * GHOST_1_CHASE);
							break;
						}
						case 2:
						{
							set_target(get_target().x - SPRITES_SIZE * GHOST_1_CHASE, get_target().y);
							break;
						}
						case 3:
						{
							set_target(get_target().x, get_target().y + SPRITES_SIZE * GHOST_1_CHASE);
							break;
						}
					}
					break;
				}
				case 2:
				{
					set_target(pacman_position.x, pacman_position.y);

					switch (pacman_direction)
					{
						case 0:
						{
							set_target(get_target().x + SPRITES_SIZE * GHOST_2_CHASE, get_target().y);
							break;
						}
						case 1:
						{
							set_target(get_target().x, get_target().y - SPRITES_SIZE * GHOST_2_CHASE);
							break;
						}
						case 2:
						{
							set_target(get_target().x - SPRITES_SIZE * GHOST_2_CHASE, get_target().y);
							break;
						}
						case 3:
						{
							set_target(get_target().x, get_target().y + SPRITES_SIZE * GHOST_2_CHASE);
						}
					}

					set_target(get_target().x + (get_target().x - ghost_ref_position.x), get_target().y + (get_target().y - ghost_ref_position.y));
					break;
				}
				case 3:
				{
					if (SPRITES_SIZE * GHOST_3_CHASE <= sqrt(pow(get_position().x - pacman_position.x, 2) + pow(get_position().y - pacman_position.y, 2)))
					{
						set_target(pacman_position.x, pacman_position.y);
					}
					else
					{
						set_target(0, SPRITES_SIZE * (MAP_HEIGHT - 1));
					}
				}
			}
		}
	}
}

void Ghost::change_mode()
{
	set_movement_mode(!get_movement_mode());
}

void Ghost::back_home()
{
	target = home;
}

sf::Vector2f Ghost::get_home()
{
	return home;
}

void Ghost::set_home(float x, float y)
{
	home = { x, y };
}

sf::Vector2f Ghost::get_home_door()
{
	return door;
}

void Ghost::set_home_door(float x, float y)
{
	door = { x, y };
}

sf::Vector2f Ghost::get_origin_position()
{
	return origin_position;
}

void Ghost::set_origin_position(float x, float y)
{
	origin_position = { x, y };
}

sf::Vector2f Ghost::get_position()
{
	return position;
}

void Ghost::set_position(float x, float y)
{
	position = { x, y };
}

sf::Vector2f Ghost::get_target()
{
	return target;
}

void Ghost::set_target(float x, float y)
{
	target = { x, y };
}

void Ghost::set_body(sf::Color color, char frame, bool flash, sf::RenderWindow& window)
{
	
	body.setPosition(get_position().x, get_position().y);
	body.setTextureRect(sf::IntRect(SPRITES_SIZE * frame, 0, SPRITES_SIZE, SPRITES_SIZE));
	if (get_frightened_mode() == 0) 
	{
		body.setColor(color);
		window.draw(body);

	}
	else if (get_frightened_mode() == 1) 
	{
		body.setColor(sf::Color(36, 36, 255));
		window.draw(body);

		
		if (flash && frame % 2 == 0)
		{
			body.setColor(sf::Color(255, 255, 255));
			window.draw(body);

		}
		else
		{
			body.setColor(sf::Color(36, 36, 255));
			window.draw(body);

		}
	}
}

void Ghost::set_face(char frame, bool flash, sf::RenderWindow& window)
{
	face.setPosition(get_position().x, get_position().y);
	if (get_frightened_mode() == 0)
	{
		face.setTextureRect(sf::IntRect(SPRITES_SIZE * get_direction(), SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE));
		face.setColor(sf::Color(255, 255, 255));
	}
	else if (get_frightened_mode() == 1)
	{
		face.setTextureRect(sf::IntRect(4 * SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE));
		if (flash && frame % 2 == 0)
		{
			face.setColor(sf::Color(255, 0, 0));
		}
		else
		{
			face.setColor(sf::Color(255, 255, 255));
		}
	}
	else 
	{
		face.setTextureRect(sf::IntRect(SPRITES_SIZE * get_direction(), 2 * SPRITES_SIZE, SPRITES_SIZE, SPRITES_SIZE));
		face.setColor(sf::Color(255, 255, 255));
	}

	window.draw(face);
}

bool Ghost::get_movement_mode()
{
	return movement_mode;
}

void Ghost::set_movement_mode(bool mode)
{
	movement_mode = mode;
}

bool Ghost::get_inside_home()
{
	return inside_home;
}

void Ghost::set_inside_home(bool inside)
{
	inside_home = inside;
}

char Ghost::get_direction()
{
	return direction;
}

void Ghost::set_direction(char new_direction)
{
	direction = new_direction;
}

char Ghost::get_frightened_mode() 
{
	return frightened_mode;
}

void Ghost::set_frightened_mode(char mode) 
{
	frightened_mode = mode;
}

char Ghost::get_frightened_time()
{
	return frightened_timer;
}

void Ghost::set_frightened_time(char time)
{
	frightened_timer = time;
}

char Ghost::get_id() 
{
	return id;
}

short Ghost::get_animation_time()
{
	return animation_timer;
}

void Ghost::set_animation_time(short time)
{
	animation_timer = time;
}
