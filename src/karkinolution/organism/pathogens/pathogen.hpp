#pragma once


#include <vector>
#include <unordered_map>
#include <format>
#include <karkinolution/core/error.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/core/basestorage.hpp>

template<typename T>
concept HasIdSpecie = requires(T obj) {
    obj.id().id;
    obj.id().specie;
    obj.id().type;
};

template<typename T>
concept HasId = requires(T obj) {
    obj.id;
};

template<typename T>
concept HasHealth = requires(T obj) {
    requires std::same_as<decltype(obj.health), OrganismStats::Health::Health>;
};


template <HasIdSpecie GenericPathogen, HasId PathogenId>
class PathogenContainer {
protected:

    std::vector<GenericPathogen> _pathogens;

public:

    GenericPathogen& at(PathogenId id) {
        for (auto& pathogen : _pathogens) {
            if (pathogen.id() == id) {
                return pathogen;
            }
        }

        throw IdNotFoundError(
            std::format("ID {} was not found", id.id)
        );
    }


    const GenericPathogen& at(PathogenId id) const {
        for (const auto& pathogen : _pathogens) {
            if (pathogen.id() == id) {
                return pathogen;
            }
        }

        throw IdNotFoundError(
            std::format("ID {} was not found", id.id)
        );
    }


    void add(GenericPathogen&& pathogen) {
        _pathogens.push_back(std::move(pathogen));
    }


    void remove(PathogenId id) {
        for (auto it = _pathogens.begin(); it != _pathogens.end(); ++it) {
            if (it->id() == id) {
                _pathogens.erase(it);
                return;
            }
        }

        throw IdNotFoundError(
            std::format("ID {} was not found", id.id)
        );
    }


    const std::vector<GenericPathogen>& virus() const {
        return _pathogens;
    }


    std::vector<GenericPathogen>& virus() {
        return _pathogens;
    }


    size_t size() const {
        return _pathogens.size();
    }
};


template <HasIdSpecie GenericPathogen, HasId PathogenId, typename Specie>
class PathogenImages {
private:

    std::unordered_map<Specie, GenericPathogen> _pathogens;


public:

    PathogenImages(
        PathogenContainer<GenericPathogen, PathogenId>&& container
    ) {
        _pathogens.reserve(container.size());

        for (auto& pathogen : container.virus()) {
            _pathogens.emplace(
                pathogen.id.specie,
                std::move(pathogen)
            );
        }
    }


    const GenericPathogen& at(Specie specie) const {
        auto it = _pathogens.find(specie);

        if (it == _pathogens.end()) {
            throw IdNotFoundError("Species was not found");
        }

        return it->second;
    }
};
