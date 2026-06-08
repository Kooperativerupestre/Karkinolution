from dataclasses import dataclass
from map import TerrainMotor, Territory, EntityMap
from coord import Coord
from organism.creatures import Creature, EntitysRegistry, Corpse
from organism.identity import Id

class WorldMotor:
    @staticmethod
    def add_entity(territory:Territory, entity_map:EntityMap, entity:Creature | Corpse, coord:Coord, entitys:EntitysRegistry) -> None:
        TerrainMotor.add_entity(territory=territory, entity_map=entity_map, coord=coord, id=entity.id)
        entitys.add(entity)
    @staticmethod
    def delete_entity_by_id(entity_map:EntityMap, id:Id, entitys:EntitysRegistry) -> None:
        TerrainMotor.delete_entity(id=id, entity_map=entity_map) # O(n)
        entitys.delete(id)
    @staticmethod
    def delete_entity_by_coord(entity_map:EntityMap, coord:Coord, id:Id, entitys:EntitysRegistry) -> None:
        TerrainMotor.delete_entity_by_coord(coord, entity_map) # O(1)
        entitys.delete(id)



@dataclass
class World:
    territory: Territory
    entity_map: EntityMap
    entitys: EntitysRegistry
    time:int = 0

    def pass_time(self) -> None:
        self.time += 1