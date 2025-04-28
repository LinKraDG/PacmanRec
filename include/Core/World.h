#pragma once

#include <array>
#include <cstdint>
#include <Gameplay/Ghost.h>
#include <SFML/System/Vector2.hpp>
#include <Utils/Constants.h>

namespace sf
{
	class RenderWindow;
}

class World
{
	public:

		~World();

		bool load();

		void level_reset();

		void update(uint32_t deltaMilliseconds);
		void render(sf::RenderWindow& window);

		std::array<std::string, MAP_HEIGHT> get_blueprint();
		void set_blueprint(char *map);

		std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>* get_map();
		void set_map(std::array<std::string, MAP_HEIGHT> map);

		bool get_victory();
		void set_victory(bool woned);

		char get_wave();
		void set_wave(int current_wave);

		char get_level();
		void set_level(int level);

		short get_wave_time();
		void set_wave_time(short time);

		void prev_update_ghost(Ghost* ghost);

	private:

		std::array<std::string, MAP_HEIGHT> map_blueprint;
		std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH> created_map{};

		bool game_woned;

		char wave;

		char level;

		short wave_time;

		Pacman* player{ nullptr };
		Ghost* ghost_0{ nullptr };
		Ghost* ghost_1{ nullptr };
		Ghost* ghost_2{ nullptr };
		Ghost* ghost_3{ nullptr };
		std::array<Ghost*, 4> ghosts;
};