#pragma once

#include <Gameplay/GameObject.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Utils/Constants.h>


class Pacman
{
public:

	bool Init();

	~Pacman() = default;

	void update(std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& map, char level);
	void render(sf::RenderWindow& window);

	void reset();
	void reset_position();
	void Pacman::next_level();

	sf::Vector2f get_position();
	void set_position(float x, float y);

	sf::Vector2f get_origin_position();
	void set_origin_position(float x, float y);

	bool get_animation_game_over();
	void set_animation_game_over(bool activated);

	bool get_dead();
	void set_dead(bool death);

	bool get_victory();
	void set_victory(bool wined);

	char get_direction();
	void set_direction(char direction_number);

	char get_lives();
	void set_lives(char lives_number);
	void loose_live();

	short get_current_animation_time();
	void set_current_animation_time(short current_time);

	float get_energized_timer();
	void set_energized_timer(float current_time);

	short get_score();
	void set_score(short score_bonus);
	void reset_score();

	void set_direction_input(std::array<bool, 4> walls);

private:

	sf::Sprite player_sprite;
	sf::Texture* pacman_texture{ nullptr };
	sf::Texture* pacman_death_texture{ nullptr };
	sf::Vector2f origin_position{ .0f, .0f };
	sf::Vector2f position{ .0f, .0f };

	bool animation_game_over_overed;
	bool dead;
	bool victory;

	char current_frame;
	char lives;
	char movement_direction;

	short current_animation_time;

	float current_energized_time;
	float score;

};