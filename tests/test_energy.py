from karkinolution.organism.creatures import CreatureFactory
from karkinolution.systems.metabolism import MetabolismSystem, FoodHint
from karkinolution.organism.stats import Energy

def test_non_negative_energy() -> None:
    creature = CreatureFactory.gen_creature()
    creature.energy.sub(creature.energy.limit)

    assert creature.energy.value >= 0
def test_feed_creature() -> None:
    creature = CreatureFactory.gen_creature()
    creature.energy.value = 0

    MetabolismSystem.eat(creature, Energy(1000, 1000), FoodHint.GRASS)

    assert creature.energy.value > 0
