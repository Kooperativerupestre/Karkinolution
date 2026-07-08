from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import Enum, auto
from typing import Callable, Iterable

from karkinolution.decisions.actions import MoveActions
from karkinolution.organism.stats import Energy

class Properties(Enum):
    EDIBLE = auto()
    DANGEROUS = auto()

    def __str__(self):
        return self.name
    def __repr__(self):
        return self.name

#
#
# COMPONENTS
#
#

class Component(ABC):
    pass

    def __str__(self):
        return self.__class__.__name__
    def __repr__(self):
        return self.__class__.__name__

class Reactive(Component, ABC):
    @abstractmethod
    def pass_time(self):...


class FoodState(Reactive):
    def __init__(self, food:Energy, regen_tax:int | float):
        self.food = food
        self.regen_tax = regen_tax
    def pass_time(self) -> None:
        self.food.add(self.regen_tax)
    
class MovimentCost(Component):
    def __init__(self, moviment_cost:int | float):
        self.moviment_cost = moviment_cost


class Damage(Component):
    def __init__(self, damage:int | float):
        self.damage = damage

#
#   
# BLUEPRINTS
#
#

class TerrainTypes(Enum):
    SAND = auto()
    WATER = auto()
    DIRT = auto()
    ROCK = auto()

    def __str__(self):
        return self.name
    def __repr__(self):
        return self.name


@dataclass(frozen=True)
class Blueprint:
    extra_values:dict[type[Component], Callable[[], Component]]
    properties:set[Properties]
    required_capabilities:set[MoveActions]
    
    def generate_components(self) -> dict[type[Component], Component]:
        extra_values = {}
        for t, f in self.extra_values.items():
            extra_values[t] = f()
        return extra_values


blueprints = {
    TerrainTypes.DIRT: Blueprint(
        extra_values={
            FoodState: lambda: FoodState(food=Energy(10, 10), regen_tax=1),
            MovimentCost: lambda: MovimentCost(1)
        },
        properties={Properties.EDIBLE},
        required_capabilities={MoveActions.WALK}
    ),

    TerrainTypes.SAND: Blueprint(
        extra_values={
            FoodState: lambda: FoodState(food=Energy(10, 10), regen_tax=1),
            MovimentCost: lambda: MovimentCost(2)
        },
        properties={Properties.EDIBLE},
        required_capabilities={MoveActions.WALK}
    ),
    TerrainTypes.ROCK: Blueprint(
        extra_values={MovimentCost: lambda: MovimentCost(1)},
        properties={Properties.DANGEROUS},
        required_capabilities={MoveActions.WALK}
    ),
    TerrainTypes.WATER: Blueprint(
        extra_values={
            FoodState: lambda: FoodState(Energy(10, 10), regen_tax=1),
            MovimentCost: lambda: MovimentCost(1)
        },
        properties={Properties.EDIBLE},
        required_capabilities={MoveActions.SWIMM}
    )
}


class Cell:
    def __init__(self,
                 type_cell:TerrainTypes,
                 extra_values:dict[type[Component], Component] | None = None,
                 properties:set[Properties] | None = None,
                 required_capabilities:set[MoveActions] | None = None) :
        self.extra_values:dict[type[Component], Component] = dict() if extra_values is None else extra_values
        self.properties:set[Properties] = set() if properties is None else properties
        self.required_capabilities:set[MoveActions] = set() if required_capabilities is None else required_capabilities
        self.type = type_cell
    # TO FIND
    def property_is_in(self, property:Properties) -> bool:
        return property in self.properties
    def component_is_in(self, component_type:type[Component]) -> bool:
        return component_type in self.extra_values
    # SHORTCUTS
    @property
    def values(self) -> Iterable[Component]:
        return list(self.extra_values.values())
    
    # COMPONENT
    def add_component(self, component:Component) -> None:
        self.extra_values[type(component)] = component
    def get_component(self, component_type:type[Component]) -> Component:
        if not self.component_is_in(component_type):
            raise ValueError('Component ({}) was not found'.format(component_type))
        return self.extra_values[component_type]
    def delete_component(self, component_type:type[Component]) -> None:
        if not self.component_is_in(component_type):
            raise ValueError('Component ({}) was not found'.format(component_type))
        del self.extra_values[component_type]

    # PROPERTIES

    def add_property(self, property:Properties) -> None:
        if self.property_is_in(property):
            raise ValueError('Property ({}) already exists'.format(property))
        self.properties.add(property)
    def delete_property(self, property:Properties) -> None:
        if not self.property_is_in(property):
            raise ValueError('Property ({}) was not found'.format(property))
        self.properties.discard(property)

    # METHODS

    def pass_time(self) -> None:
        for component in self.values:
            if isinstance(component, Reactive):
                component.pass_time()

def gen_cell(terrain_type:TerrainTypes) -> Cell:
    blueprint = blueprints[terrain_type]

    return Cell(extra_values=blueprint.generate_components(), 
                properties=blueprint.properties.copy(),
                required_capabilities=blueprint.required_capabilities.copy(),
                type_cell=terrain_type)
