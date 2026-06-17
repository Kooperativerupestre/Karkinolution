from organism.creatures import Creature, Corpse
from map.cell import Cell, FoodState, Damage, MovimentCost, Properties
from organism.stats import Energy

class Serializer:
    @staticmethod
    def serialize_creature(creature:Creature) -> dict[str, int | float | str]:
        creature_interface = creature.interface
        
        return {
            'name': creature_interface.name,
            'id': str(creature_interface.id),
            'specie_id': str(creature_interface.specie_id),
            'age': creature_interface.age.value,
            'energy': creature_interface.energy.value,
            'life': creature_interface.life.value,
            'pregnant': creature_interface.pregnant,
            'intent': str(creature_interface.intent),
            'position_x': creature_interface.position.x,
            'position_y': creature_interface.position.y,
            'hungry': creature_interface.hungry
        }
    @staticmethod
    def serialize_corpse(corpse:Corpse) -> dict[str, int | float | str]:
        return {
            'id': str(corpse.id),
            'energy': corpse.energy.value,
            'decomposition_time': corpse.decomposition_time.value,
            'position_x': corpse.position.x,
            'position_y': corpse.position.y
        }
    @staticmethod
    def serialize_cell(cell:Cell) -> dict[str, int | float | str | bool | None]:
        is_edible = False
        is_dangerous = False
        is_movable = False
        
        food:Energy | None = None
        damage:int | float | None = None
        movement_cost:int | float | None = None


        if cell.property_is_in(Properties.EDIBLE):
            food = cell.get_component(FoodState).food # type: ignore
            is_edible = True
        
        if cell.property_is_in(Properties.DANGEROUS):
            damage = cell.get_component(Damage).damage # type: ignore
            is_dangerous = True

        if cell.component_is_in(MovimentCost):
            movement_cost = cell.get_component(MovimentCost).moviment_cost # type: ignore
            is_movable = True
        
        return {
            'type': str(cell.type),
            'is_edible': is_edible,
            'food': food.value if food else None,
            'is_dangerous': is_dangerous,
            'damage': damage,
            'is_movable': is_movable,
            'movement_cost': movement_cost
        }