#include <SFML/Graphics.hpp>

struct Boid {
  sf::Vector2f position;
  float angle = 0;
  sf::Color color = sf::Color::White;
  float size = 10;
};

void draw_boid(const Boid& boid, sf::RenderWindow& window) {
  const float kBoidCircleRadius = boid.size;

  {
    sf::CircleShape circle(kBoidCircleRadius, 6);
    circle.setOrigin(kBoidCircleRadius, kBoidCircleRadius);
    circle.rotate(boid.angle);
    circle.move(boid.position.x, boid.position.y);
    circle.setFillColor(boid.color);
    window.draw(circle);
  }

  {
    const float kLineWidth = kBoidCircleRadius / 4;
    sf::RectangleShape line(sf::Vector2f(kLineWidth, kBoidCircleRadius * 2));
    line.setOrigin(kLineWidth / 2, kBoidCircleRadius * 2);
    line.rotate(boid.angle);
    line.move(boid.position.x , boid.position.y);
    line.setFillColor(boid.color);
    window.draw(line);
  }
}

int main(int argc, char* argv[]) {
  sf::RenderWindow window(sf::VideoMode(800, 600), "Boids");

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::Resized) {
        window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
      }
    }

    window.clear(sf::Color::Black);

    draw_boid({{20, 20}, -30, sf::Color::Red}, window);
    draw_boid({{50, 50}, 40, sf::Color::Green, 20}, window);

    window.display();

  }
};
