#pragma once
#include <array>
#include <Gameplay/Ghost.h>
#include <Gameplay/Pacman.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <Utils/Constants.h>


std::array<std::string, MAP_HEIGHT> load_map(char* map_sketch_file);

void draw_map(const std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& i_map, sf::RenderWindow& i_window);

bool map_collisions(bool collect_pellets, bool use_door, short x, short y, std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH>& map, Pacman* player);

std::array<std::array<map_sprites, MAP_HEIGHT>, MAP_WIDTH> convert_sketch(const std::array<std::string, MAP_HEIGHT>& map_sketch, std::array<Ghost*, 4> &ghosts, Pacman* player);

void draw_pacman_lives(Pacman* player, sf::RenderWindow& window);