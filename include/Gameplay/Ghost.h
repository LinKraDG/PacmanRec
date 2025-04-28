#pragma once
#include <Gameplay/Pacman.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <Utils/Constants.h>


class Ghost
{
	public:
		Ghost(char ghost_id);

		~Ghost() = default;

		void update(char level, std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& map, Ghost& ghost_ref, Pacman* pacman);

		void reset();
		void reset_position();

		bool pacman_hit(const sf::Vector2f& i_pacman_position);

		float get_pacman_distance(char target_direction);

		void draw(bool flash, sf::RenderWindow& window);

		void update_target(char i_pacman_direction, const sf::Vector2f& ghost_ref_position, const sf::Vector2f& i_pacman_position);

		void change_mode();

		void back_home();

		sf::Vector2f get_home();
		void set_home(float x, float y);

		sf::Vector2f get_home_door();
		void set_home_door(float x, float y);

		sf::Vector2f get_origin_position();
		void set_origin_position(float x, float y);

		sf::Vector2f get_position();
		void set_position(float x, float y);

		sf::Vector2f get_target();
		void set_target(float x, float y);

		void set_body(sf::Color color, char frame, bool flash, sf::RenderWindow& window);
		void set_face(char frame, bool flash, sf::RenderWindow& window);

		bool get_movement_mode();
		void set_movement_mode(bool mode);

		bool get_inside_home();
		void set_inside_home(bool inside);

		char get_direction();
		void set_direction(char new_direction);

		char get_frightened_mode();
		void set_frightened_mode(char mode);

		char get_frightened_time();
		void set_frightened_time(char time);

		char get_id();

		short get_animation_time();
		void set_animation_time(short time);

	private:
		sf::Sprite body;
		sf::Sprite face;

		sf::Texture* ghostTexture{ nullptr };

		sf::Vector2f home{ .0f, .0f };
		sf::Vector2f door{ .0f, .0f };
		sf::Vector2f origin_position{ .0f, .0f };
		sf::Vector2f position{ .0f, .0f };
		sf::Vector2f target{ .0f, .0f };

		bool movement_mode;
		bool inside_home;

		char direction;

		char frightened_mode;
		char frightened_timer;

		char id;

		short animation_timer;

};