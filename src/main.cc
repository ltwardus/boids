#include <array>
#include <random>
#include <SFML/Graphics.hpp>

class Boid;
using Boids = std::array<Boid, 40>;

class Boid {
 public:
  Boid() = default;
  Boid(const sf::Vector2f& pos, float rot, const sf::Color& col) : pos_(pos), rot_(rot), col_(col) {}
  Boid(const Boid&) = default;
  Boid(Boid&&) = default;
  Boid& operator=(const Boid&) = default;
  Boid& operator=(Boid&&) = default;

  /**
   * Update boid.
   *
   * /param dt Delta time in seconds.
   * /param boids All boids.
   * /param window_size Window size.
   */
  void update(float dt, const Boids& boids, const sf::Vector2u& window_size) {
    /** Update position */
    {
      sf::Transform rotation;
      rotation.rotate(rot_);
      const float kDeltaSpeed = move_speed_ * dt;
      pos_ += rotation.transformPoint(0, -kDeltaSpeed);
      if (pos_.x < 0) {
        pos_.x = window_size.x;
      }

      if (pos_.x > window_size.x) {
        pos_.x = 0;
      }

      if (pos_.y < 0) {
        pos_.y = window_size.y;
      }

      if (pos_.y > window_size.y) {
        pos_.y = 0;
      }
    }
  }

  sf::Vector2f position() const {
    return pos_;
  }

  float rotation() const {
    return rot_;
  }

  sf::Color color() const {
    return col_;
  }

  int size() const {
    return size_;
  }

  int view_distance() const {
    return size_ * kViewDistanceFactor;
  }
 private:
  sf::Vector2f pos_;
  float rot_ = 0;
  sf::Color col_ = sf::Color::White;
  int size_ = 10;
  int move_speed_ = 200;
  int kViewDistanceFactor = 3;
};

template<class T>
constexpr T kPi = T(3.1415926535897932385);


void draw_boids(const Boids& boids, sf::RenderWindow& window) {
  for (const auto& boid : boids) {
    const int kBoidCircleRadius = boid.size();
    const int kBoidViewRadius = boid.view_distance();

    /** View distance */
    {
      sf::CircleShape circle(kBoidViewRadius);
      circle.setOrigin(kBoidViewRadius, kBoidViewRadius);
      circle.move(boid.position());
      sf::Color color = boid.color();
      color.a = 32;
      circle.setFillColor(color);
      window.draw(circle);
    }

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

Boids generate_random_boids(const sf::Window& window) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> random_rotation(0, 359);
  static std::uniform_int_distribution<> random_color_channel_value(50, 255);
  const sf::Vector2u& window_size = window.getSize();
  std::uniform_int_distribution<> random_pos_x(0, window_size.x);
  std::uniform_int_distribution<> random_pos_y(0, window_size.y);
  Boids boids;

  for (auto& boid : boids) {
    boid = Boid(sf::Vector2f(random_pos_x(gen), random_pos_y(gen)), random_rotation(gen),
                sf::Color(random_color_channel_value(gen),
                          random_color_channel_value(gen),
                          random_color_channel_value(gen)));
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
    boid.update(kDeltaTimeSeconds, boids, kWindowSize);
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
