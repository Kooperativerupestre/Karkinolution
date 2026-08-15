#pragma once
#include "model.hpp"

namespace Vec3F {
    [[nodiscard]] constexpr double angle_between(const Vec3& a, const Vec3& b) noexcept {
        const double dot_val = a.dot(b);
        const double len_product = a.length() * b.length();
        if (len_product <= 0.0) return 0.0;
        return std::acos(std::clamp(dot_val / len_product, -1.0, 1.0));
    }

    [[nodiscard]] constexpr Vec3 project_onto(const Vec3& a, const Vec3& b) noexcept {
        const double b_len_sq = b.length_squared();
        if (b_len_sq <= 0.0) return Vec3(0.0, 0.0, 0.0);
        const double scale = a.dot(b) / b_len_sq;
        return b * scale;
    }

    [[nodiscard]] constexpr Vec3 reject_from(const Vec3& a, const Vec3& b) noexcept {
        return a - project_onto(a, b);
    }

    [[nodiscard]] constexpr Vec3 gravitational_force(
        const Vec3& pos1, const Vec3& pos2,
        double mass1, double mass2, double G = 6.67430e-11
    ) noexcept {
        const Vec3 delta = pos2 - pos1;
        const double dist_sq = delta.length_squared();
        if (dist_sq <= 0.0) return Vec3(0.0, 0.0, 0.0);
        const double force_magnitude = G * mass1 * mass2 / dist_sq;
        return delta.normalized() * force_magnitude;
    }

    [[nodiscard]] constexpr Vec3 spring_force(const Vec3& displacement, double spring_constant) noexcept {
        return displacement * (-spring_constant);
    }

    [[nodiscard]] constexpr Vec3 damping_force(const Vec3& velocity, double damping_coefficient) noexcept {
        return velocity * (-damping_coefficient);
    }

    [[nodiscard]] constexpr Vec3 friction_force(const Vec3& velocity, double mu, double normal_force) noexcept {
        if (velocity.is_zero()) return Vec3(0.0, 0.0, 0.0);
        const double friction_magnitude = mu * normal_force;
        return velocity.normalized() * (-friction_magnitude);
    }

    [[nodiscard]] constexpr Vec3 centripetal_acceleration(const Vec3& velocity, const Vec3& position) noexcept {
        const double radius = position.length();
        if (radius <= 0.0) return Vec3(0.0, 0.0, 0.0);
        const double speed_sq = velocity.length_squared();
        return position.normalized() * (-speed_sq / radius);
    }

    [[nodiscard]] constexpr Vec3 projectile_position(
        const Vec3& initial_pos, const Vec3& initial_vel,
        double time, const Vec3& gravity = Vec3(0.0, -9.80665, 0.0)
    ) noexcept {
        return initial_pos + initial_vel * time + gravity * (0.5 * time * time);
    }

    [[nodiscard]] constexpr Vec3 projectile_velocity(
        const Vec3& initial_vel, double time, const Vec3& gravity = Vec3(0.0, -9.80665, 0.0)
    ) noexcept {
        return initial_vel + gravity * time;
    }

    [[nodiscard]] constexpr double kinetic_energy(double mass, const Vec3& velocity) noexcept {
        return 0.5 * mass * velocity.length_squared();
    }

    [[nodiscard]] constexpr Vec3 linear_momentum(double mass, const Vec3& velocity) noexcept {
        return velocity * mass;
    }

    [[nodiscard]] constexpr Vec3 impulse(const Vec3& force, double delta_time) noexcept {
        return force * delta_time;
    }

    [[nodiscard]] constexpr double work(const Vec3& force, const Vec3& displacement) noexcept {
        return force.dot(displacement);
    }

    [[nodiscard]] constexpr double power(const Vec3& force, const Vec3& velocity) noexcept {
        return force.dot(velocity);
    }
}
