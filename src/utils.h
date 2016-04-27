#pragma once

#include <SFML/System.hpp>

template<class T>
constexpr T kPi = T(3.1415926535897932385);

template<class T>
T distance_2d(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
  sf::Vector2<T> diff = a - b;
  return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

template<class T>
T rad2deg(T rad) {
  return (rad * 180) / kPi<T>;
}

