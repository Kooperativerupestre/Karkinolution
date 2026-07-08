from map.world import World, WorldPresets, WorldMotor
from tick.worldcycle import Runner
from map.map import TerrainQuery, TerrainMotor
from organism.creatures import CreatureFactory
from ui.interfaces import CreatureInterface, CellInterface, InterfaceFactory
from organism.identity import Id, EntityTypes, create_creature_id
from core.coord import Coord
from core.error import CoordinateAlreadyExistsError, IdNotFoundError, CoordinateNotFoundError
from organism.ontology import Gender
from organism.genetics import creatures_genomes, CreatureTypes
from systems.reproduction import UterusSystem
from decisions.actions import IntentActs, Intent
from enum import Enum, auto


def create_id_creature(id:str) -> Id:
    return Id(id, EntityTypes.CREATURE)
def create_id_corpse(id:str) -> Id:
    return Id(id, EntityTypes.CORPSE)        


class Outputs(Enum):
    CREATURE_NOT_FOUND = auto()
    INVALID_SPECIE = auto()
    INVALID_ENTRY = auto()
    COORD_NOT_FOUND = auto()
    ID_NOT_FOUND = auto()
    COORD_ALREADY_EXISTS = auto()
    CANNOT_EXECUTE = auto()
    OK = auto()
    
class BasicAPI:
    @staticmethod
    def reset_with_a_preset(entry:str) -> World | Outputs:
        if entry == '1':
            return WorldPresets.crab_chaos()
        elif entry == '2':
            return WorldPresets.paranoic()
        elif entry == '3':
            return WorldPresets.titanic()
        elif entry == '4':
            return WorldPresets.normal()
        return Outputs.INVALID_ENTRY
    
    @staticmethod
    def create_random_creature(name:str, world:World) -> Outputs:
        free_coords = TerrainQuery.random_free_coord(world.territory, world.entity_map, 1)
        if len(free_coords) == 0:
            return Outputs.COORD_NOT_FOUND
        free_coord = free_coords[0]
        creature = CreatureFactory.gen_creature(position=free_coord, name=name)
        WorldMotor.add_entity(world, creature)
        return Outputs.OK

    @staticmethod
    def kill_creature(id_s:str, world:World) -> Outputs:
        id = create_id_creature(id_s)
        try:
            WorldMotor.delete_entity(world.entity_map, id, world.entities)
            return Outputs.OK
        except Exception:
            return Outputs.CANNOT_EXECUTE
    @staticmethod
    def run_n_time(world:World, n:int = 1) -> None:
        for _ in range(0, n):
            Runner.run(world)
    @staticmethod
    def exists(id:str, world:World) -> bool:
        return world.entities.exists_generic(id)

class ApiInterface:
    @staticmethod
    def get_creature_interface(id:str, world:World) -> CreatureInterface | None:
        try:
            creature = InterfaceFactory.create_creature_interface(world.entities.get_creature(create_creature_id(id)))
            return creature
        except Exception:
            return None

    @staticmethod
    def get_cell_interface(coord:Coord, world:World) -> CellInterface | None:
        try:
            cell = world.territory.get(coord)
        except Exception:
            return None
        return InterfaceFactory.create_cell_interface(cell)
    



class EasyApiWorld:
    @staticmethod
    def run_1_time(world:World) -> None:
        BasicAPI.run_n_time(world, 1)
    @staticmethod
    def run_5_time(world:World) -> None:
        BasicAPI.run_n_time(world, 5)


class HardApiWorld:
    @staticmethod
    def create_creature(
        world:World,
        name:str | None,
        e_gender: str | None,
        e_specie: str | None,
        position: Coord | None) -> Outputs:

        if e_gender is not None:
            if e_gender.lower() == Gender.MALE.name.lower():
                gender = Gender.MALE
            elif e_gender.lower() == Gender.FEMALE.name.lower():
                gender = Gender.FEMALE
            else:
                return Outputs.INVALID_ENTRY
        else:
            gender = None
        
        if e_specie is not None:
            if e_specie.lower() == CreatureTypes.CRAB.name.lower():
                specie = CreatureTypes.CRAB
            elif e_specie.lower() == CreatureTypes.CROCODILE.name.lower():
                specie = CreatureTypes.CROCODILE
            elif e_specie.lower() == CreatureTypes.FISH.name.lower():
                specie = CreatureTypes.FISH
            elif e_specie.lower() == CreatureTypes.HIPPOPOTAMUS.name.lower():
                specie = CreatureTypes.HIPPOPOTAMUS
            else:
                return Outputs.INVALID_ENTRY
        else:
            specie = None

        creature = CreatureFactory.gen_creature(
            position=position,
            name=name,
            gender=gender,
            creature_type=specie
        )

        try:
            WorldMotor.add_entity(world, creature)
        except CoordinateAlreadyExistsError:
            return Outputs.COORD_ALREADY_EXISTS
        except CoordinateNotFoundError:
            return Outputs.COORD_NOT_FOUND
        return Outputs.OK
    @staticmethod
    def turn_gender(world:World, id:str) -> Outputs:
        try:
            creature = world.entities.get_creature(create_id_creature(id))
        except IdNotFoundError:
            return Outputs.CREATURE_NOT_FOUND
        creature.gender=Gender.other_sex(creature.gender)
        return Outputs.OK
    @staticmethod
    def turn_pregnant(world:World, e_id:str) -> Outputs:
        try:
            creature = world.entities.get_creature(create_id_creature(e_id))
        except IdNotFoundError:
            return Outputs.CREATURE_NOT_FOUND
        male_specie = creature.genome.core.id
        male_genome = creatures_genomes.get_type_genome(male_specie)

        if creature.gender != Gender.FEMALE:
            return Outputs.INVALID_ENTRY
        if creature.fertility.value < creature.fertility.limit:
            creature.fertility.value=creature.fertility.limit
        UterusSystem.conceive(creature, male_genome)
        return Outputs.OK
    @staticmethod
    def fill_energy(world:World, id:str) -> Outputs:
        try:
            creature = world.entities.get_creature(create_id_creature(id))
        except IdNotFoundError:
            return Outputs.CREATURE_NOT_FOUND
        creature.energy.value=creature.energy.limit
        return Outputs.OK
    @staticmethod
    def change_intent(world:World, id:str, e_intent:str) -> Outputs:
        try:
            creature = world.entities.get_creature(create_id_creature(id))
        except IdNotFoundError:
            return Outputs.CREATURE_NOT_FOUND
        
        if e_intent == IntentActs.FIND_FOOD.name.lower():
            creature.intent = Intent(IntentActs.FIND_FOOD)
        elif e_intent == IntentActs.FIND_MATCH.name.lower():
            creature.intent = Intent(IntentActs.FIND_MATCH)
        elif e_intent == IntentActs.NOTHING.name.lower():
            creature.intent = Intent(IntentActs.NOTHING)
        else:
            return Outputs.INVALID_ENTRY
        return Outputs.OK
    @staticmethod
    def move(world:World, id:str, new_coord:Coord) -> Outputs:
        try:
            creature = world.entities.get_creature(create_id_creature(id))
        except IdNotFoundError:
            return Outputs.ID_NOT_FOUND
        
        try:
            TerrainMotor.move(creature.position, new_coord,  world.entity_map, world.territory)
        except Exception:
            return Outputs.CANNOT_EXECUTE
        
        return Outputs.OK
    