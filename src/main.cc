#include <array>
#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "arial_font.h"
#include "predator.h"
#include "boid.h"
#include "draw.h"

constexpr unsigned int kAddRemoveBoidsCount = 10;
constexpr unsigned int kStartupBoidCount = 80;

Boid randomize_boids(const sf::Window& window) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> random_rotation(-180, 179);
  static std::uniform_int_distribution<> random_color_channel_value(50, 255);
  const sf::Vector2u& window_size = window.getSize();
  std::uniform_int_distribution<> random_pos_x(0, window_size.x);
  std::uniform_int_distribution<> random_pos_y(0, window_size.y);

  return Boid(sf::Vector2f(random_pos_x(gen), random_pos_y(gen)),
              random_rotation(gen),
              sf::Color(random_color_channel_value(gen),
                        random_color_channel_value(gen),
                        random_color_channel_value(gen)));
}

Boids randomize_boids(Boids& boids, const sf::Window& window) {
  for (auto& boid : boids) {
    boid = randomize_boids(window);
  }

  return boids;
}

void add_boids(Boids& boids, unsigned int count, const sf::Window& window) {
  boids.reserve(boids.size() + count);
  for (unsigned int i = 0; i < count; ++i) {
    boids.push_back(randomize_boids(window));
  }
}

void remove_boids(Boids& boids, unsigned int count) {
  if (boids.size() > 1) {
    const Boids::size_type kNumberOfBoidsToRemove = std::min(boids.size(), static_cast<Boids::size_type>(count));

    Boids(boids.begin() + kNumberOfBoidsToRemove, boids.end()).swap(boids);
  }
}

void update_boids(Boids& boids, const Predators& predators, const sf::Time& dt, const sf::Window& window) {
  const sf::Vector2u& kWindowSize = window.getSize();
  const float kDeltaTimeSeconds = dt.asSeconds();
  for (auto& boid : boids) {
    boid.update(boids, predators, kDeltaTimeSeconds, kWindowSize);
  }
}

int main(int argc, char* argv[]) {
  sf::Font font;
  if (!font.loadFromMemory(kArialFont.data(), kArialFont.size())) {
    throw std::runtime_error("Cannot load font");
  }

  sf::RenderWindow window(sf::VideoMode(1024, 768), "Boids");
  window.setMouseCursorVisible(false);

  sf::Clock clock;
  Boids boids(kStartupBoidCount);
  randomize_boids(boids, window);
  Predators predators;

  sf::Text help_text(
      std::string("Help:\n") +
        "Move the mouse to scare the boids\n" +
        "r : randomize boids\n" +
        "+ : add " + std::to_string(kAddRemoveBoidsCount) + " boids\n" +
        "- : remove " + std::to_string(kAddRemoveBoidsCount) + " boids\n" +
        "d : on/off debug boid drawing\n",
      font);

  bool debug_boid_drawing = false;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::Resized) {
        window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
      }

      if (event.type == sf::Event::KeyPressed) {
        switch(event.key.code) {
          case sf::Keyboard::R: {
            randomize_boids(boids, window);
            break;
          }
          case sf::Keyboard::Add: {
            add_boids(boids, kAddRemoveBoidsCount, window);
            break;
          }
          case sf::Keyboard::Subtract: {
            remove_boids(boids, kAddRemoveBoidsCount);
            break;
          }
          case sf::Keyboard::D: {
            debug_boid_drawing = !debug_boid_drawing;
            break;
          }
          default: {
            break;
          }
        };
      }
    }

    window.clear(sf::Color::Black);

    const sf::Time& kDt = clock.getElapsedTime();
    clock.restart();

    Predators final_predators = predators;
    {
      Predator mouse_predator;
      const sf::Vector2i& mouse_position = sf::Mouse::getPosition(window);
      mouse_predator.position.x = mouse_position.x;
      mouse_predator.position.y = mouse_position.y;
      final_predators.push_back(mouse_predator);
    }

    update_boids(boids, final_predators, kDt, window);
    draw_boids(boids, window, debug_boid_drawing);
    draw_predators(final_predators, window);

    window.draw(help_text);

    window.display();

  }
};
