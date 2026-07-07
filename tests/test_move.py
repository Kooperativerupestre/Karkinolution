from organism.creatures import CreatureFactory, Creature
from core.coord import Coord
from decisions.presets import MovePreset, PresetExecutor, MoveOutputs
from dataclasses import dataclass
from map.map import ScaleGenValues
from decisions.perception import perceive, Perception
from map.world import WorldMotor, WorldFactory,PresetWorld, World
from hypothesis import given, strategies as st
from systems.physics import MovementSystem

@dataclass
class Basic:
    world:World
    perception:Perception
    creature:Creature


def gen_basic(seed:int) -> Basic:
    world = WorldFactory.create_world(PresetWorld(0, Coord(10, 10), ScaleGenValues.MEDIUM))
    creature = CreatureFactory.gen_creature(Coord(0, 0))
    WorldMotor.add_entity(world, creature)
    perception = perceive(creature, world.territory, world.entity_map, world.entities)
    return Basic(
        world,
        perception,
        creature
    )

@given(seed=st.integers(min_value=1, max_value=999999))
def test_update_creature_position(seed:int) -> None:
    world = gen_basic(seed)
    new_coord = Coord(0, 1)
    temporary_creature_coord = world.creature.position
    output = PresetExecutor.execute_move(MovePreset(new_coord), world.creature, world.perception, world.world)

    if output == MoveOutputs.OK:
        assert world.creature.position == new_coord
    else:
        assert world.creature.position == temporary_creature_coord

@given(seed=st.integers(min_value=1, max_value=999999), x=st.integers(min_value=1, max_value=2000), y=st.integers(min_value=1, max_value=2000))
def test_position_creature_vacated(seed:int, x:int, y:int) -> None:
    world = gen_basic(seed)
    old_position = world.creature.position
    new_coord = Coord(x, y)

    output = PresetExecutor.execute_move(MovePreset(new_coord), world.creature, world.perception, world.world)

    if output == MoveOutputs.OK:
        assert world.creature.position != old_position
    else:
        assert world.creature.position == old_position and world.creature.position != new_coord

@given(seed=st.integers(min_value=1, max_value=999999))
def test_world_entity_map_vacated(seed:int) -> None:
    world = gen_basic(seed)
    old_position = world.creature.position
    new_coord = Coord(0, 1)

    output = PresetExecutor.execute_move(MovePreset(new_coord), world.creature, world.perception, world.world)

    if output == MoveOutputs.OK:
        assert world.world.entity_map.require(old_position) is None
    else:
        assert world.world.entity_map.get(old_position) == world.creature.id

@given(seed=st.integers(min_value=1, max_value=999999))
def test_movement_energy_consumption(seed:int) -> None:
    world = gen_basic(seed)
    initial_energy = world.creature.energy.value
    new_coord = Coord(0, 1)

    output = PresetExecutor.execute_move(MovePreset(new_coord), world.creature, world.perception, world.world)

    if output == MoveOutputs.OK:
        assert world.creature.energy.value < initial_energy
    else:
        assert world.creature.energy.value == initial_energy


@given(seed=st.integers(min_value=1, max_value=999999))
def test_invalid_best_pos(seed:int) -> None:
    world = gen_basic(seed)

    creature_2 = CreatureFactory.gen_creature(Coord(0, 1))
    WorldMotor.add_entity(world.world, creature_2)

    perception_2 = perceive(creature_2, world.world.territory, world.world.entity_map, world.world.entities)
    output = MovementSystem.best_pos(creature_2, perception_2, world.creature.position)
    
    if output is not None:
        assert output != world.creature.position and output.distance_to_other(world.creature.position) <= creature_2.position.distance_to_other(world.creature.position) + 1