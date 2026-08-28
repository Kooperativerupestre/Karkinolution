#pragma once
#include <ranges>
#include <stdexcept>
#include <unordered_map>

/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

template <typename K, typename V> class BaseStorage {
  private:
    std::unordered_map<K, V> data{};

  public:
    const std::unordered_map<K, V> internal_data() const { return data; }
    auto keys() { return data | std::views::keys; }

    auto keys() const { return data | std::views::keys; }

    auto values() { return data | std::views::values; }

    auto values() const { return data | std::views::values; }

    const std::unordered_map<K, V>& iter() const { return data; }

    size_t size() const { return data.size(); }

    bool exists(const K& k) const { return data.contains(k); }

    bool exists_value(const V& v) const {
        for (const auto& [key, value] : data) {
            if (value == v) {
                return true;
            }
        }

        return false;
    }

    void reserve(size_t i) { data.reserve(i); }

    template <typename Predicate> bool exists_value(Predicate predicate) const {
        for (const auto& [key, value] : data) {
            if (predicate(key, value)) {
                return true;
            }
        }

        return false;
    }

    template <typename Predicate> V* at_by_value(Predicate predicate) {
        for (auto& [key, value] : data) {
            if (predicate(key, value)) {
                return &value;
            }
        }

        return nullptr;
    }

    template <typename Predicate> const V* at_by_value(Predicate predicate) const {
        for (const auto& [key, value] : data) {
            if (predicate(key, value)) {
                return &value;
            }
        }

        return nullptr;
    }

    void add(const K& k, const V& v) { data.emplace(k, v); }

    template <typename V2> bool try_add(const K& k, V2&& v) {
        auto [it, inserted] = data.emplace(k, std::forward<V2>(v));

        return inserted;
    }

    void del(const K& k) {
        size_t removed = data.erase(k);

        if (removed == 0) {
            throw std::out_of_range("inexistent key");
        }
    }

    bool try_del(const K& k) {
        size_t removed = data.erase(k);
        if (removed == 0) {
            return false;
        }
        return true;
    }

    V& at(const K& k) { return data.at(k); }

    const V& at(const K& k) const { return data.at(k); }

    V* try_at(const K& k) {
        auto it = data.find(k);

        if (it != data.end()) {
            return &it->second;
        }

        return nullptr;
    }

    const V* try_at(const K& k) const {
        auto it = data.find(k);

        if (it != data.end()) {
            return &it->second;
        }

        return nullptr;
    }
};