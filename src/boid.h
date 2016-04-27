#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "predator.h"
#include "utils.h"

class Boid;
using Boids = std::vector<Boid>;

class Boid {
 public:
  Boid() = default;
  Boid(const sf::Vector2f& pos, float rot, const sf::Color& col)
    : pos_(pos),
      rot_(rot),
      target_rot_(rot),
      col_(col) {}
  Boid(const Boid&) = default;
  Boid(Boid&&) = default;
  Boid& operator=(const Boid&) = default;
  Boid& operator=(Boid&&) = default;

  /**
   * Update boid.
   *
   * /param boids All boids.
   * /param predators Predators.
   * /param dt Delta time in seconds.
   * /param window_size Window size.
   */
  void update(const Boids& boids, const Predators& predators, float dt, const sf::Vector2u& window_size);

  sf::Vector2f position() const;
  float rotation() const;
  sf::Color color() const;
  int size() const;
  int cohesion_distance() const;
  int alignment_distance() const;
  int separation_distance() const;
 private:
  /** Boid config options */
  struct Config {
    const int kSize = 10;
    const float kDefaultMoveSpeed = 200;
    const float kPredatorEscapeMoveSpeed = 4 * kDefaultMoveSpeed;
    const float kDefaultRotationSpeed = 360;
    const float kPredatorEscapeRotationSpeed = 4 * kDefaultRotationSpeed;
    const int kSeparationDistanceFactor = 2;
    const int kAlignmentDistanceFactor = 7;
    const int kCohesionDistanceFactor = 20;
  };

  Predators get_local_predators(const Predators& predators, int distance) const;

  template<class T>
  Boids get_flockmates(const T& boids, int distance) const {
      std::vector<Boid> result;
      std::copy_if(boids.begin(), boids.end(), std::back_inserter(result), [&](const auto& local_flockmate) {
        return distance_2d(pos_, local_flockmate.pos_) < distance;
      });
      return result;
  }

  template<class T>
  sf::Vector2f center_of_mass(const T& boids) const {
    return std::accumulate(
      boids.begin(),
      boids.end(),
      sf::Vector2f(),
      [&](auto result, const auto& local_flockmate) {
        result.x += local_flockmate.pos_.x / boids.size();
        result.y += local_flockmate.pos_.y / boids.size();
        return result;
      }
    );
  }

  sf::Vector2f center_of_mass(const Predators& predators) const;

  /**
   * Handle predators.
   *
   * \param preadators Predators
   * \param dt Delta time in seconds.
   * \return True if some predators were detected and some actions performed, false otherwise.
   */
  bool handle_predators(const Predators& predators, float dt);

  static const Config kConfig_;

  sf::Vector2f pos_;
  float rot_ = 0;
  float target_rot_ = 0;
  sf::Color col_ = sf::Color::White;
  float move_speed_ = kConfig_.kDefaultMoveSpeed;
  float rotation_speed_ = kConfig_.kDefaultRotationSpeed;
};

