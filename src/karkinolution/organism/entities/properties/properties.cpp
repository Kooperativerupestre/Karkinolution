#include <karkinolution/organism/entities/properties/properties.hpp>


bool PropertiesContainer::has(GenericProperty property) const {
    return value.contains(property);
}


bool PropertiesContainer::add(GenericProperty property) {
    for (const PropertyConflict& conflict : GLOBAL_PROPERTY_CONFLICTS) {
        if (
            (conflict.A == property && value.contains(conflict.B)) ||
            (conflict.B == property && value.contains(conflict.A))
        ) {
            return false;
        }
    }

    return value.insert(property).second;
}


bool PropertiesContainer::remove(GenericProperty property) {
    return value.erase(property) > 0;
}


void PropertiesContainer::clear() {
    value.clear();
}