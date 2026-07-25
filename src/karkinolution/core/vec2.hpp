#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <karkinolution/core/global_epsilon.hpp>

/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

struct Vec2 {
    int x, y;

    constexpr Vec2(int x, int y) : x(x), y(y) {}

    constexpr Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    constexpr Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    constexpr Vec2 operator*(int number) const {
        return Vec2(x * number, y * number);
    }
    constexpr Vec2 operator/(int number) const {
        return Vec2(x / number, y / number);
    }
    constexpr bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
    constexpr bool operator!=(const Vec2& other) const {
        return x != other.x || y != other.y;
    }

    constexpr float length() const {
        return std::sqrt(x*x + y*y);
    }
};

namespace std {
    template<>
    struct hash<Vec2> {
        size_t operator()(const Vec2& c) const {
            return hash<int>()(c.x) ^ (hash<int>()(c.y) << 1);
        }
    };
}

inline std::ostream& operator<<(std::ostream& os, const Vec2& c) {
    os << "x: " << c.x << ", y: " << c.y;
    return os;
}

namespace Vec2F {
    constexpr inline int distance_squared(const Vec2& c1, const Vec2& c2) {
        int dx = c2.x - c1.x;
        int dy = c2.y - c1.y;
        return dx * dx + dy * dy;
    }

    constexpr inline float distance(const Vec2& c1, const Vec2& c2) {
        return std::sqrt(static_cast<float>(distance_squared(c1, c2)));
    }

    inline bool distance_exceeds_one(const Vec2& c1, const Vec2& c2) {
        return static_cast<float>(distance_squared(c1, c2)) > Approx<float>(1);
    }

    inline std::vector<Vec2> four_movements(const Vec2& c) {
        return {
            Vec2(c.x + 1, c.y),
            Vec2(c.x, c.y + 1),
            Vec2(c.x - 1, c.y),
            Vec2(c.x, c.y - 1)
        };
    }

    inline std::vector<Vec2> eight_movements(const Vec2& c) {
        std::vector<Vec2> result;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                result.push_back(Vec2(c.x + dx, c.y + dy));
            }
        }
        return result;
    }
}