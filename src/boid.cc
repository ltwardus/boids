#include "boid.h"

const Boid::Config Boid::kConfig_ = {};


void Boid::update(const Boids& boids, const Predators& predators, float dt, const sf::Vector2u& window_size) {
  /** Update position */
  {
    sf::Transform rotation;
    rotation.rotate(rot_);
    const float kDeltaMoveSpeed = move_speed_ * dt;
    pos_ += rotation.transformPoint(0, -kDeltaMoveSpeed);
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


  {
    float temp_rot = rot_ + 360;
    float temp_target_rot = target_rot_ + 360;
    float rotation_direction = 1;
    while(temp_rot >= 360) {
      temp_rot -= 360;
    }
    while(temp_target_rot >= 360) {
      temp_target_rot -= 360;
    }

    float rotation_delta = temp_target_rot - temp_rot;

    while(rotation_delta < 0) {
      rotation_delta += 360;
    }

    if (rotation_delta > 180) {
      rotation_direction = -1;
    }

    rot_ += rotation_direction * rotation_speed_ * dt;
  }

  if (rot_ < -180) {
    rot_ += 360;
  }
  if (rot_ > 179) {
    rot_ -= 360;
  }


  /** Predators */
  if (handle_predators(predators, dt)) {
    return;
  }

  /** No predators, perform normal tasks */
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> random_rotation_jitter(-45, 45);


  /** Cohesion */
  const std::vector<Boid> kCohesionFlockmates = get_flockmates(boids, cohesion_distance());
  /** If at this point there is only one flockmate (this boid) then there is nothing to do */
  if (kCohesionFlockmates.size() == 1) {
    target_rot_ += random_rotation_jitter(gen);
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

    target_rot_ = kBoidToCenterOfMassRotation - 90;
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

      target_rot_ = kAverageRotation + random_rotation_jitter(gen);
  } else if (kCohesionFlockmates.size() > 1) {
    const float kBoidToCenterOfMassRotation =
      rad2deg(std::atan2(kCohesionFlockmateCenterOfMass.y - pos_.y, kCohesionFlockmateCenterOfMass.x - pos_.x));

    target_rot_ = kBoidToCenterOfMassRotation + 90;
  }

}

sf::Vector2f Boid::position() const {
  return pos_;
}

float Boid::rotation() const {
  return rot_;
}

sf::Color Boid::color() const {
  return col_;
}

int Boid::size() const {
  return kConfig_.kSize;
}

int Boid::cohesion_distance() const {
  return kConfig_.kSize * kConfig_.kCohesionDistanceFactor;
}

int Boid::alignment_distance() const {
  return kConfig_.kSize * kConfig_.kAlignmentDistanceFactor;
}

int Boid::separation_distance() const {
  return kConfig_.kSize * kConfig_.kSeparationDistanceFactor;
}

Predators Boid::get_local_predators(const Predators& predators, int distance) const {
  Predators result;
  std::copy_if(predators.begin(), predators.end(), std::back_inserter(result), [&](const auto& predator) {
    return distance_2d(pos_, predator.position) < distance + predator.size;
  });
  return result;
}

sf::Vector2f Boid::center_of_mass(const Predators& predators) const {
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

bool Boid::handle_predators(const Predators& predators, float dt) {
  const int kPredatorDetectionDistance = alignment_distance();
  const Predators& kLocalPredators = get_local_predators(predators, kPredatorDetectionDistance);
  if (!kLocalPredators.empty()) {
    const sf::Vector2f& kPreadtorsCenterOfMass = center_of_mass(kLocalPredators);
    const float kBoidToCenterOfMassRotation =
      rad2deg(std::atan2(kPreadtorsCenterOfMass.y - pos_.y,
                         kPreadtorsCenterOfMass.x - pos_.x));

    target_rot_ = kBoidToCenterOfMassRotation - 90;
    /** Run away from the predator */
    const float kFearFactor =
      1 - std::min(1.0f, distance_2d(kPreadtorsCenterOfMass, pos_) / kPredatorDetectionDistance);
    const float kPredatorMoveSpeed =
      std::min(kConfig_.kDefaultMoveSpeed + (kConfig_.kPredatorEscapeMoveSpeed * kFearFactor),
               kConfig_.kPredatorEscapeMoveSpeed);

    move_speed_ = std::max(move_speed_, kPredatorMoveSpeed);

    const float kPredatorRotationSpeed =
      std::min(kConfig_.kDefaultMoveSpeed + (kConfig_.kPredatorEscapeRotationSpeed * kFearFactor),
               kConfig_.kPredatorEscapeRotationSpeed);

    rotation_speed_ = std::max(rotation_speed_, kPredatorRotationSpeed);

    return true;
  } else {
    /** No predator, decelerate if needed */
    if (move_speed_ > kConfig_.kDefaultMoveSpeed) {
      move_speed_ -= kConfig_.kPredatorEscapeMoveSpeed * dt;
    }

    move_speed_= std::max(move_speed_, kConfig_.kDefaultMoveSpeed);

    if (rotation_speed_ > kConfig_.kDefaultRotationSpeed) {
      rotation_speed_ -= kConfig_.kPredatorEscapeRotationSpeed * dt;
    }

    rotation_speed_= std::max(rotation_speed_, kConfig_.kDefaultRotationSpeed);
  }

  return false;
}
