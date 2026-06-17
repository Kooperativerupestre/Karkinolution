from dataclasses import dataclass
from map.map import TerrainMotor, Territory, EntityMap
from core.coord import Coord
from organism.creatures import Creature, EntitiesRegistry, Corpse
from organism.identity import Id
from systems.reproductivebuffer import ReproductiveBuffer

class WorldMotor:
    @staticmethod
    def add_entity(territory:Territory, entity_map:EntityMap, entity:Creature | Corpse, entities:EntitiesRegistry) -> None:
        TerrainMotor.add_entity(territory=territory, entity_map=entity_map, coord=entity.position, id=entity.id)
        entities.add(entity)
    @staticmethod
    def delete_entity_by_id(entity_map:EntityMap, id:Id, entities:EntitiesRegistry) -> None:
        entity_map.delete(entity_map.get_key_by_value(id)) # O(n)
        entities.delete(id)


        # Slow path O(n)
        # Use only creature's coordinate is unknow
        # Prefer delete_entity whenever possible
    @staticmethod
    def delete_entity(entity_map:EntityMap, coord:Coord, id:Id, entities:EntitiesRegistry) -> None:
        entity_map.delete(coord) # O(1)
        entities.delete(id)



@dataclass
class World:
    territory: Territory
    entity_map: EntityMap
    entities: EntitiesRegistry
    reproductive_buffer:ReproductiveBuffer
    time:int = 0

    def pass_time(self) -> None:
        self.time += 1