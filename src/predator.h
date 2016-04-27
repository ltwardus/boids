#pragma once

#include <SFML/System.hpp>

struct Predator {
  sf::Vector2f position;
  int size = 20;
};

using Predators = std::vector<Predator>;
