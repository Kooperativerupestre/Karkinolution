from organism.creatures import Creature, Corpse, PregnantUterus
from dataclasses import dataclass
from organism.stats import Energy, Life, Age, LimitedValue
from core.coord import Coord
from map.cell import TerrainTypes, Cell, Properties, FoodState, MovimentCost, Damage
from organism.genetics import CreatureTypes
from map.world import World
from organism.identity import EntityTypes
from decisions.actions import Intent

@dataclass(frozen=True)
class CreatureInterface:
    name:str
    id:str
    gender:str
    pregnant:bool
    specie:CreatureTypes
    energy:Energy
    life:Life
    age:Age
    hungry:float
    
    intent:Intent
    position:Coord
    gestation:LimitedValue | None


@dataclass(frozen=True)
class CorpseInterface:
    meat:Energy
    time_left:int



@dataclass(frozen=True)
class CellInterface:
    food:LimitedValue | None
    damage: int | float | None
    movement_cost: int | float | None
    solo_type: TerrainTypes

@dataclass(frozen=True)
class BlockInterface:
    coord:Coord
    cell:CellInterface
    creature:CreatureInterface

@dataclass(frozen=True)
class WorldInterface:
    entities:int
    creatures:int
    corpses:int
    time:int
    size:Coord

    @property
    def creatures_percent(self) -> float:
        return self.creatures/self.entities*100
    @property
    def corpses_percent(self) -> float:
        return self.corpses/self.entities*100
    
class InterfaceFactory:
    @staticmethod
    def create_creature_interface(creature:Creature) -> CreatureInterface:
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            gestation = LimitedValue(creature.uterus.gestation.value, creature.uterus.gestation.limit)
        else:
            gestation = None
        
        return CreatureInterface(
            creature.name,
            creature.id.id,
            creature.gender.name.lower().capitalize(),
            creature.pregnant,
            creature.genome.core.id,
            creature.energy,
            creature.life,
            creature.age,
            creature.hungry,
            creature.intent,
            creature.position,
            gestation
        )
    @staticmethod
    def create_corpse_interface(corpse:Corpse) -> CorpseInterface:
        return CorpseInterface(
            corpse.energy,
            int(corpse.time_left)
        )
    @staticmethod
    def create_cell_interface(cell:Cell) -> CellInterface:
        food:Energy | None = None
        damage:int | float | None = None
        movement_cost: int | float | None = None
        
        if cell.property_is_in(Properties.EDIBLE):
            food = cell.get_component(FoodState).food  # type: ignore
        if cell.property_is_in(Properties.DANGEROUS):
            damage = cell.get_component(Damage).damage # type: ignore
        if cell.component_is_in(MovimentCost):
            movement_cost = cell.get_component(MovimentCost).moviment_cost # type: ignore
        return CellInterface(
            food, # type: ignore
            damage,    # type: ignore
            movement_cost,  # type: ignore
            cell.type
        )
    @staticmethod
    def create_block_interface(coord:Coord, cell:Cell, creature:Creature) -> BlockInterface:
        return BlockInterface(
            coord, 
            InterfaceFactory.create_cell_interface(cell),
            InterfaceFactory.create_creature_interface(creature)
        ) 
    @staticmethod
    def create_world_interface(world:World) -> WorldInterface:
        entities = 0
        creatures = 0
        corpses = 0

        for id in world.entities.entitys.keys():
            entities += 1

            if id.e_type == EntityTypes.CREATURE:
                creatures += 1
            else:
                corpses += 1
        
        return WorldInterface(
            entities,
            creatures,
            corpses,
            world.time,
            Coord(world.territory.size_x, world.territory.size_y),
        )