#include <array>
#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

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

struct Predator {
  sf::Vector2f position;
  int size = 20;
};

using Predators = std::vector<Predator>;

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
   * /param boids All boids.
   * /param predators Predators.
   * /param dt Delta time in seconds.
   * /param window_size Window size.
   */
  void update(const Boids& boids, const Predators& predators, float dt, const sf::Vector2u& window_size) {
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

    /** Predators */
    {
      const Predators& kLocalPredators = get_local_predators(predators, cohesion_distance());
      if (!kLocalPredators.empty()) {
        const sf::Vector2f& kPreadtorsCenterOfMass = center_of_mass(kLocalPredators);
        const float kBoidToCenterOfMassRotation =
          rad2deg(std::atan2(kPreadtorsCenterOfMass.y - pos_.y,
                             kPreadtorsCenterOfMass.x - pos_.x));

        rot_ = kBoidToCenterOfMassRotation - 90;
        /** Run away from the predator */
        move_speed_ = predator_escape_move_speed_;
        return;
      } else {
        /** No predator, decelerate if needed */
        if (move_speed_ > default_move_speed_) {
          move_speed_ -= predator_escape_move_speed_ * dt;
        }

        move_speed_= std::max(move_speed_, default_move_speed_);
      }
    }

    /** Cohesion */
    const std::vector<Boid> kCohesionFlockmates = get_flockmates(boids, cohesion_distance());
    /** If at this point there is only one flockmate (this boid) then there is nothing to do */
    if (kCohesionFlockmates.size() == 1) {
      return;
    }
    const sf::Vector2f& kCohesionFlockmateCenterOfMass = center_of_mass(kCohesionFlockmates);

    /** Alignment */
    const std::vector<Boid> kAlignmentFlockmates = get_flockmates(kCohesionFlockmates, alignment_distance());
    const sf::Vector2f& kAlignmentFlockmateCenterOfMass = center_of_mass(kAlignmentFlockmates);

    /** Separation */
    const std::vector<Boid> kSeparationFlockmates = get_flockmates(kAlignmentFlockmates, separation_distance());
    const sf::Vector2f& kSeparationFlockmateCenterOfMass = center_of_mass(kSeparationFlockmates);

    if (kSeparationFlockmates.size() > 1) {
      const float kBoidToCenterOfMassRotation =
        rad2deg(std::atan2(kSeparationFlockmateCenterOfMass.y - pos_.y,
                           kSeparationFlockmateCenterOfMass.x - pos_.x));

      rot_ = kBoidToCenterOfMassRotation - 90;
    } else if (kAlignmentFlockmates.size() > 1) {
      const float kAverageRotation =
        std::accumulate(
          kAlignmentFlockmates.begin(),
          kAlignmentFlockmates.end(),
          0.0f,
          [&](float result, const auto& boid) {
            return result + boid.rot_ / kAlignmentFlockmates.size();
          }
        );

        rot_ = kAverageRotation;
    } else if (kCohesionFlockmates.size() > 1) {
      const float kBoidToCenterOfMassRotation =
        rad2deg(std::atan2(kCohesionFlockmateCenterOfMass.y - pos_.y, kCohesionFlockmateCenterOfMass.x - pos_.x));

      rot_ = kBoidToCenterOfMassRotation + 90;
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

  int cohesion_distance() const {
    return size_ * kCohesionDistanceFactor;
  }

  int alignment_distance() const {
    return size_ * kAlignmentDistanceFactor;
  }

  int separation_distance() const {
    return size_ * kSeparationDistanceFactor;
  }
 private:
  Predators get_local_predators(const Predators& predators, int distance) const {
      Predators result;
      std::copy_if(predators.begin(), predators.end(), std::back_inserter(result), [&](const auto& predator) {
        return distance_2d(pos_, predator.position) < distance;
      });
      return result;
  }

  template<class T>
  std::vector<Boid> get_flockmates(const T& boids, int distance) const {
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

  sf::Vector2f center_of_mass(const Predators& predators) const {
    return std::accumulate(
      predators.begin(),
      predators.end(),
      sf::Vector2f(),
      [&](auto result, const auto& predator) {
        result.x += predator.position.x / predators.size();
        result.y += predator.position.y / predators.size();
        return result;
      }
    );
  }

  sf::Vector2f pos_;
  float rot_ = 0;
  sf::Color col_ = sf::Color::White;
  int size_ = 10;
  float default_move_speed_ = 200;
  float move_speed_ = 200;
  float predator_escape_move_speed_ = 4 * default_move_speed_;
  int kSeparationDistanceFactor = 3;
  int kAlignmentDistanceFactor = 9;
  int kCohesionDistanceFactor = 14;
};

void draw_boids(const Boids& boids, sf::RenderWindow& window) {
  for (const auto& boid : boids) {
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

Boids generate_random_boids(const sf::Window& window) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> random_rotation(-180, 179);
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

void update_boids(Boids& boids, const Predators& predators, const sf::Time& dt, const sf::Window& window) {
  const sf::Vector2u& kWindowSize = window.getSize();
  const float kDeltaTimeSeconds = dt.asSeconds();
  for (auto& boid : boids) {
    boid.update(boids, predators, kDeltaTimeSeconds, kWindowSize);
  }
}

int main(int argc, char* argv[]) {
  sf::RenderWindow window(sf::VideoMode(800, 600), "Boids");

  sf::Clock clock;
  Boids boids = generate_random_boids(window);
  Predators predators;

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
        if (event.key.code == sf::Keyboard::R) {
          boids = generate_random_boids(window);
        }
      }
    }

    window.clear(sf::Color::Black);

    const sf::Time& kDt = clock.getElapsedTime();
    clock.restart();

    Predators final_predators = predators;
    {
      Predator mouse_predator;
      const sf::Vector2i& mouse_position = sf::Mouse::getPosition();
      mouse_predator.position.x = mouse_position.x;
      mouse_predator.position.y = mouse_position.y;
      final_predators.push_back(mouse_predator);
    }

    update_boids(boids, final_predators, kDt, window);
    draw_boids(boids, window);
    draw_predators(final_predators, window);

    window.display();

  }
};
