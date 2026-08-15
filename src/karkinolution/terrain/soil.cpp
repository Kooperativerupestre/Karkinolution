#include <karkinolution/terrain/soil.hpp>


// Blueprint
Components Blueprint::gen_components() const {
    Components comps;

    for (const auto& f_component : default_components) {
        comps.add(f_component());
    } 
    return comps;
}


// SoilF

namespace SoilF {

SoilPiece gen_soil_piece(SoilTypes s_t) {
    const Blueprint* blueprint = &blueprints.at(s_t);

    return SoilPiece{
        s_t, blueprint->properties, blueprint->required_properties,
        blueprint->gen_components()
    };
}
} 