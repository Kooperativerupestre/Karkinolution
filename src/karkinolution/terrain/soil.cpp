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

