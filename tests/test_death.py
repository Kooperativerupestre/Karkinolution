from karkinolution.organism.creatures import CreatureFactory
from karkinolution.systems.death import DeathSystem


def test_energy_death() -> None:
    creature = CreatureFactory.gen_creature()
    creature.energy.zero()
    assert DeathSystem.is_dead(creature)
def test_life_death() -> None:
    creature = CreatureFactory.gen_creature()
    creature.life.zero()
    assert DeathSystem.is_dead(creature)
def test_age_death() -> None:
    creature = CreatureFactory.gen_creature()
    creature.age.full()
    assert DeathSystem.is_dead(creature)
