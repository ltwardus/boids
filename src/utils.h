#pragma once

#include <SFML/System.hpp>
#include <cmath>

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

template<class T>
T deg2rad(T deg) {
  return (deg * kPi<T>) / 180;
}

template<class T>
T constraint_angle_0_360(T angle) {
  angle = std::fmod(angle, 360);
  if (angle < 0) {
    angle += 360;
  }
  return angle;
}
