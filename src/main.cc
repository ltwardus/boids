#include <array>
#include <random>
#include <SFML/Graphics.hpp>

struct Boid {
  sf::Vector2f position;
  float angle = 0;
  sf::Color color = sf::Color::White;
  int size = 10;
  int speed = 200;
};

using Boids = std::array<Boid, 40>;

template<class T>
constexpr T kPi = T(3.1415926535897932385);


void draw_boids(const Boids& boids, sf::RenderWindow& window) {
  for (const auto& boid : boids) {
    const int kBoidCircleRadius = boid.size;

    {
      sf::CircleShape circle(kBoidCircleRadius, 6);
      circle.setOrigin(kBoidCircleRadius, kBoidCircleRadius);
      circle.rotate(boid.angle);
      circle.move(boid.position.x, boid.position.y);
      circle.setFillColor(boid.color);
      window.draw(circle);
    }

    {
      const int kLineWidth = kBoidCircleRadius / 4;
      sf::RectangleShape line(sf::Vector2f(kLineWidth, kBoidCircleRadius * 2));
      line.setOrigin(kLineWidth / 2, kBoidCircleRadius * 2);
      line.rotate(boid.angle);
      line.move(boid.position.x , boid.position.y);
      line.setFillColor(boid.color);
      window.draw(line);
    }
  }
}

Boids generate_random_boids(const sf::Window& window) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> random_angle(-180, 179);
  static std::uniform_int_distribution<> random_color_channel_value(50, 255);
  const sf::Vector2u& kWindowSize = window.getSize();
  std::uniform_int_distribution<> random_pos_x(0, kWindowSize.x);
  std::uniform_int_distribution<> random_pos_y(0, kWindowSize.y);
  Boids boids;

  for (auto& boid : boids) {
    boid.position = sf::Vector2f(random_pos_x(gen), random_pos_y(gen));
    boid.angle = random_angle(gen);
    boid.color =
      sf::Color(random_color_channel_value(gen), random_color_channel_value(gen), random_color_channel_value(gen));
  }

  return boids;
}

template<class T>
T distance_2d(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
  sf::Vector2<T> diff = a - b;
  return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

template<class T>
T rad2deg(T rad) {
  return (rad * 180) / kPi<T>;
}

void update_boids(Boids& boids, const sf::Time& dt, const sf::Window& window) {
  const sf::Vector2u& kWindowSize = window.getSize();
  const float kDeltaTimeSeconds = dt.asSeconds();
  for (auto& boid : boids) {
    /** Update position */
    {
      sf::Transform rotation;
      rotation.rotate(boid.angle);
      const float kDeltaSpeed = boid.speed * kDeltaTimeSeconds;
      boid.position += rotation.transformPoint(0, -kDeltaSpeed);
      if (boid.position.x < 0) {
        boid.position.x = kWindowSize.x;
      }

      if (boid.position.x > kWindowSize.x) {
        boid.position.x = 0;
      }

      if (boid.position.y < 0) {
        boid.position.y = kWindowSize.y;
      }

      if (boid.position.y > kWindowSize.y) {
        boid.position.y = 0;
      }
    }

    /** Cohesion */
    {
      /** Calculate average angle */
      int boids_in_area = 0;
      const float kAccumulatedAngle =
        std::accumulate(boids.begin(), boids.end(), 0.0f, [&](float result, const auto& other_boid) {
            if (distance_2d(boid.position, other_boid.position) < 3 * boid.size) {
              ++boids_in_area;
              return result + other_boid.angle;
            }

            return result;
        });
      boid.angle = kAccumulatedAngle / boids_in_area;
    }
  }
}

int main(int argc, char* argv[]) {
  sf::RenderWindow window(sf::VideoMode(800, 600), "Boids");

  sf::Clock clock;
  Boids boids = generate_random_boids(window);

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

    const sf::Time& kDt = clock.getElapsedTime();
    clock.restart();

    update_boids(boids, kDt, window);
    draw_boids(boids, window);

    window.display();

  }
};
