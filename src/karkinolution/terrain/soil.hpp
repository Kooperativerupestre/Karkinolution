#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>
#include <karkinolution/core/stats.hpp>
#include <vector>
#include <karkinolution/actions/actions.hpp>
#include <functional>

struct Component {
    virtual ~Component() = default;
    Component() = default;
    Component(const Component&) = default;
    Component& operator=(const Component&) = default;
    Component(Component&&) = default;
    Component& operator=(Component&&) = default;
};


struct Components {
    std::unordered_map<std::type_index, std::unique_ptr<Component>> data;

    template <typename T>
    bool exists() const {
        return data.contains(std::type_index(typeid(T)));
    }

    template <typename T>
    bool add(T component) {
        if (exists<T>()) {
            return false;
        }

        data[std::type_index(typeid(T))] =
            std::make_unique<T>(std::move(component));

        return true;
    }

    bool add(std::unique_ptr<Component> component) {
        if (!component) {
            return false;
        }

        const auto component_type = std::type_index(typeid(*component));
        if (data.contains(component_type)) {
            return false;
        }

        data[component_type] = std::move(component);
        return true;
    }

    template <typename T>
    bool del() {
        return data.erase(std::type_index(typeid(T))) > 0;
    }

    template <typename T>
    T* try_get() const {
        auto it = data.find(std::type_index(typeid(T)));

        if (it == data.end()) {
            return nullptr;
        }

        return static_cast<T*>(it->second.get());
    }
};


enum class SoilTypes : uint8_t {
    SAND,
    ROCK,
    DIRT,
    WATER
};

enum class Properties : uint8_t {
    EDIBLE,
    DANGEROUS
};



struct FoodState : Component {
    public:

    Energy food;
    float regen_tax;

    FoodState(Energy food, float regen_tax): food(food), regen_tax(regen_tax) {}

};

struct Damage : Component {
    public:

    float damage;

    Damage(float damage): damage(damage) {}
};


struct MovementCost : Component {
    public:

    float cost;

    MovementCost(float cost): cost(cost) {}
};

struct Blueprint {
    std::vector<std::function<std::unique_ptr<Component>()>> default_components;
    std::vector<Properties> properties;
    std::vector<MoveActions> required_capabilities;

    Components gen_components() const;
};

inline const std::unordered_map<SoilTypes, Blueprint> blueprints = {
    {SoilTypes::DIRT, Blueprint{
                    .default_components = {
                        []() -> std::unique_ptr<Component>{ return std::make_unique<FoodState>(Energy(10, 10), 1); },
                        []() -> std::unique_ptr<Component>{ return std::make_unique<MovementCost>(1); }
                    },
                    .properties = {Properties::EDIBLE},
                    .required_capabilities = {MoveActions::WALK}

    }},

    {SoilTypes::SAND, Blueprint{
        .default_components = {
            []() -> std::unique_ptr<Component>{ return std::make_unique<FoodState>(Energy(10, 10), 1);},
            []() -> std::unique_ptr<Component>{ return std::make_unique<MovementCost>(2);}
        },
        .properties = {Properties::EDIBLE}
    }},

    {SoilTypes::ROCK, Blueprint{
        .default_components = {
            []() -> std::unique_ptr<Component>{ return std::make_unique<MovementCost>(1); }
        },
        .properties = {Properties::DANGEROUS},
        .required_capabilities = {MoveActions::WALK}
    }},

    {SoilTypes::WATER, Blueprint{
        .default_components = {
            []() -> std::unique_ptr<Component>{ return std::make_unique<FoodState>(Energy(10, 10), 1); },
            []() -> std::unique_ptr<Component>{ return std::make_unique<MovementCost>(1); }
        },
        .properties = {Properties::EDIBLE},
        .required_capabilities = {MoveActions::SWIMM}
    }}
};

struct SoilPiece {
    SoilTypes type;
    std::vector<Properties> properties;
    std::vector<MoveActions> required_capabilities;
    Components components;

    SoilPiece(SoilTypes type, std::vector<Properties> properties,
    std::vector<MoveActions> required_capabilities, Components components):
    type(type), properties(properties), required_capabilities(required_capabilities), components(std::move(components)) {}
};

namespace SoilF{
    SoilPiece gen_soil_piece(SoilTypes s_t);
}