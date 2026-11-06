#pragma once
#include <cmath>
#include <karkinolution/math/physic/vec/model.hpp>

namespace Karkinolution {
	namespace Math {
		namespace Vec {
			[[nodiscard]] constexpr double angle_between(const Vec2 &a, const Vec2 &b) noexcept {
				const double dot_val     = a.dot(b);
				const double len_product = a.length() * b.length();
				if (len_product <= 0.0) {
					return 0.0;
				}
				return std::acos(std::clamp(dot_val / len_product, -1.0, 1.0));
			}

			[[nodiscard]] constexpr Vec2 project_onto(const Vec2 &a, const Vec2 &b) noexcept {
				const double b_len_sq = b.length_squared();
				if (b_len_sq <= 0.0) {
					return Vec2(0.0, 0.0);
				}
				return b * (a.dot(b) / b_len_sq);
			}

			[[nodiscard]] constexpr Vec2 reject_from(const Vec2 &a, const Vec2 &b) noexcept {
				return a - project_onto(a, b);
			}

			[[nodiscard]] constexpr Vec2 gravitational_force(const Vec2 &pos1,
															 const Vec2 &pos2,
															 double      mass1,
															 double      mass2,
															 double      G = 6.67430e-11) noexcept {
				const Vec2   delta   = pos2 - pos1;
				const double dist_sq = delta.length_squared();
				if (dist_sq <= 0.0) {
					return Vec2(0.0, 0.0);
				}
				return delta.normalized() * (G * mass1 * mass2 / dist_sq);
			}

			[[nodiscard]] constexpr Vec2 spring_force(const Vec2 &displacement, double spring_constant) noexcept {
				return displacement * (-spring_constant);
			}

			[[nodiscard]] constexpr Vec2 damping_force(const Vec2 &velocity, double damping_coefficient) noexcept {
				return velocity * (-damping_coefficient);
			}

			[[nodiscard]] constexpr Vec2 friction_force(const Vec2 &velocity, double mu, double normal_force) noexcept {
				if (velocity.is_zero()) {
					return Vec2(0.0, 0.0);
				}
				return velocity.normalized() * (-(mu * normal_force));
			}

			[[nodiscard]] constexpr Vec2 centripetal_acceleration(const Vec2 &velocity, const Vec2 &position) noexcept {
				const double radius = position.length();
				if (radius <= 0.0) {
					return Vec2(0.0, 0.0);
				}
				return position.normalized() * (-velocity.length_squared() / radius);
			}

			[[nodiscard]] constexpr Vec2 projectile_position(const Vec2 &initial_pos,
															 const Vec2 &initial_vel,
															 double      time,
															 const Vec2 &gravity = Vec2(0.0, -9.80665)) noexcept {
				return initial_pos + initial_vel * time + gravity * (0.5 * time * time);
			}

			[[nodiscard]] constexpr Vec2 projectile_velocity(const Vec2 &initial_vel,
															 double      time,
															 const Vec2 &gravity = Vec2(0.0, -9.80665)) noexcept {
				return initial_vel + gravity * time;
			}

			[[nodiscard]] constexpr double kinetic_energy(double mass, const Vec2 &velocity) noexcept {
				return 0.5 * mass * velocity.length_squared();
			}

			[[nodiscard]] constexpr Vec2 linear_momentum(double mass, const Vec2 &velocity) noexcept {
				return velocity * mass;
			}

			[[nodiscard]] constexpr Vec2 impulse(const Vec2 &force, double delta_time) noexcept {
				return force * delta_time;
			}

			[[nodiscard]] constexpr double work(const Vec2 &force, const Vec2 &displacement) noexcept {
				return force.dot(displacement);
			}

			[[nodiscard]] constexpr double power(const Vec2 &force, const Vec2 &velocity) noexcept {
				return force.dot(velocity);
			}
		} // namespace Vec
	} // namespace Math
} // namespace Karkinolution
