#include "draw.h"

void draw_boid_debug_info(const Boid& boid, sf::RenderWindow& window) {
  /** Cohesion distance */
  {
    const int kBoidCohesionRadius = boid.cohesion_distance();
    sf::CircleShape circle(kBoidCohesionRadius);
    circle.setOrigin(kBoidCohesionRadius, kBoidCohesionRadius);
    circle.move(boid.position());
    sf::Color color = boid.color();
    color.a = 32;
    circle.setFillColor(color);
    window.draw(circle);
  }

  /** Alignment distance */
  {
    const int kBoidAlignmentRadius = boid.alignment_distance();
    sf::CircleShape circle(kBoidAlignmentRadius);
    circle.setOrigin(kBoidAlignmentRadius, kBoidAlignmentRadius);
    circle.move(boid.position());
    sf::Color color = boid.color();
    color.a = 48;
    circle.setFillColor(color);
    window.draw(circle);
  }

  /** Separation distance */
  {
    const int kBoidSeparationRadius = boid.separation_distance();
    sf::CircleShape circle(kBoidSeparationRadius);
    circle.setOrigin(kBoidSeparationRadius, kBoidSeparationRadius);
    circle.move(boid.position());
    sf::Color color = boid.color();
    color.a = 48;
    circle.setFillColor(color);
    window.draw(circle);
  }
}

void draw_boids(const Boids& boids, sf::RenderWindow& window, bool debug_boid_drawing) {
  for (const auto& boid : boids) {
    if  (debug_boid_drawing) {
      draw_boid_debug_info(boid, window);
    }

    {
      const int kBoidCircleRadius = boid.size();
      /** Boid body */
      {
        sf::CircleShape circle(kBoidCircleRadius, 6);
        circle.setOrigin(kBoidCircleRadius, kBoidCircleRadius);
        circle.rotate(boid.rotation());
        circle.move(boid.position());
        circle.setFillColor(boid.color());
        window.draw(circle);
      }

      /** Boid direction indicator */
      {
        const int kLineWidth = kBoidCircleRadius / 4;
        sf::RectangleShape line(sf::Vector2f(kLineWidth, kBoidCircleRadius * 2));
        line.setOrigin(kLineWidth / 2, kBoidCircleRadius * 2);
        line.rotate(boid.rotation());
        line.move(boid.position());
        line.setFillColor(boid.color());
        window.draw(line);
      }
    }
  }
}

void draw_predators(const Predators& predators, sf::RenderWindow& window) {
  for (const auto& predator : predators) {
    const int kPredatorRadius = predator.size;
    sf::CircleShape circle(kPredatorRadius);
    circle.setOrigin(kPredatorRadius, kPredatorRadius);
    circle.move(predator.position);
    circle.setFillColor(sf::Color::Red);
    window.draw(circle);
  }
}
