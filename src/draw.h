#pragma once

#include "boid.h"

/**
 * Draw boid debug info.
 *
 * \param boid Boid.
 * \param window Window.
 */
void draw_boid_debug_info(const Boid& boid, sf::RenderWindow& window);

/**
 * Draw boids.
 *
 * \param boids Boids.
 * \param window Window.
 * \param debug_boid_drawing If debug info should be drawn.
 */
void draw_boids(const Boids& boids, sf::RenderWindow& window, bool debug_boid_drawing);

/**
 * Draw predators.
 *
 * \param predators Predators.
 * \param window Window.
 */
void draw_predators(const Predators& predators, sf::RenderWindow& window);

