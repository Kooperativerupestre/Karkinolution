#include <karkinolution/organism/entities/embryo/embryo.hpp>

EmbryoHungry Embryo::hungry() const {
    return (1.0f - life.ratio()) + (1.0f - energy.ratio()) + (1.0f - health.ratio()/2.0f); 
}