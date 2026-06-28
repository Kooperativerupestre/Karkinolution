from organism.creatures import CreatureFactory
from core.coord import Coord
from systems.metabolism_system import MetabolismSystem, FoodHint
from organism.stats import Energy

def test_non_negative_energy() -> None:
    creature = CreatureFactory.gen_creature(Coord(0, 0))
    creature.energy.sub(creature.energy.limit)

    assert creature.energy.value >= 0
def test_feed_creature() -> None:
    creature = CreatureFactory.gen_creature(Coord(0, 0))
    creature.energy.value = 0

    MetabolismSystem.eat(creature, Energy(1000, 1000), FoodHint.GRASS)

    assert creature.energy.value > 0
    