#pragma once
#include <cmath>
#include <random>
#include <vector>
#include <iterator>
#include <type_traits>
#include <array>

inline std::random_device rd;
inline std::mt19937_64 gen(rd());

namespace Choices {
    template <std::ranges::random_access_range Range>
    decltype(auto) choice(Range&& range) {
        std::uniform_int_distribution<size_t> dist(0, std::ranges::size(range) - 1);
        return range[dist(gen)];
    }

    template <std::ranges::forward_range Range>
        requires (!std::ranges::random_access_range<Range> &&
                std::ranges::sized_range<Range>)
    decltype(auto) choice(Range&& range) {
        std::uniform_int_distribution<size_t> dist(0, std::ranges::size(range) - 1);

        auto it = std::ranges::begin(range);
        std::ranges::advance(it, dist(gen));

        return *it;
    }

    template <std::ranges::forward_range Range>
        requires (!std::ranges::random_access_range<Range> &&
                !std::ranges::sized_range<Range>)
    decltype(auto) choice(Range&& range) {
        auto size = std::ranges::distance(range);

        std::uniform_int_distribution<std::ranges::range_difference_t<Range>>
            dist(0, size - 1);

        auto it = std::ranges::begin(range);
        std::ranges::advance(it, dist(gen));

        return *it;
    }

    template <typename T>
    T choice(std::initializer_list<T> list) {
        std::uniform_int_distribution<size_t> dist(0, list.size() - 1);
        return *(list.begin() + dist(gen));
    }
    template <typename ItemsContainer, typename WeightsContainer>
    auto choices(ItemsContainer&& items, WeightsContainer&& weights, size_t k) {
        using PlainItems = std::remove_reference_t<ItemsContainer>;
        using ItemType   = typename PlainItems::value_type;
        
        std::vector<ItemType> selected;
        selected.reserve(k);

        std::discrete_distribution<size_t> dist(
            std::begin(std::forward<WeightsContainer>(weights)),
            std::end(std::forward<WeightsContainer>(weights))
        );

        for (size_t i = 0; i < k; ++i) {
            size_t sampled_index = dist(gen);
            auto it = std::begin(std::forward<ItemsContainer>(items));
            std::advance(it, sampled_index);
            selected.push_back(*it);
        }

        return selected;
    }
    template <typename ItemsContainer>
    auto choices(const ItemsContainer& items, size_t k) {
        using ItemType = std::decay_t<decltype(*std::begin(items))>;
        
        size_t num_items = std::distance(std::begin(items), std::end(items));
        
        if (num_items == 0 || k == 0) return std::vector<ItemType>{};

        std::vector<double> uniform_weights(num_items, 1.0);
        return choices(items, uniform_weights, k);
    }
    template <typename T, typename U>
    bool choice_bool(T true_weight, U false_weight) {
        auto weights = {true_weight, false_weight};
        return Choices::choices(std::array<bool, 2>{true, false}, weights, 1)[0];
    } 

    template <typename T>
    bool choice_bool(const std::vector<T>&weights) {
        return Choices::choices(std::array<bool, 2>{true, false}, weights, 1)[0];
    }

}

namespace Disturbs {

    template <typename T, typename U>
    float mini_scramble(T v1, U v2) {
        std::uniform_real_distribution<float> dist{0.9f, 1.1f};
        return (v2 + v1)/2 * dist(gen);
    }

    template <typename T, typename U>
    auto gen_disturb(T v1, U v2) {
        return std::uniform_real_distribution<float>{static_cast<float>(v1), static_cast<float>(v2)}(gen);
    }

    inline float gen_little_disturb() {
        return gen_disturb(0.90f, 1.10f);
    }

    inline float gen_medium_disturb() {
        return gen_disturb(0.86f, 1.16f);
    }

    template <typename T, typename U>
    float smooth_scramble(T v1, U v2) {
        return (static_cast<float>(v1) + static_cast<float>(v2))/2 * gen_little_disturb();
    }

    template <typename T, typename U>
    int time_scramble(T v1, U v2) {
        return std::round((v1 + v2)/2) + Choices::choice({-1, 0, 1});
    }

}