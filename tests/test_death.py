from organism.creatures import CreatureFactory
from systems.death_system import DeathSystem


def test_verify_creature_death() -> None:
    # INIT

    creature = CreatureFactory.gen_creature()
    # ENERGY VERIFICATION
    creature.energy.value = 0

    assert DeathSystem.is_dead(creature) == True, "Creature with energy = 0 must die"
    creature.energy.value=creature.energy.limit
    # LIFE VERIFICATION

    creature.life.value = 0
    assert DeathSystem.is_dead(creature) == True, "Creature with life = must die"
    creature.life.value=creature.life.limit     
    # AGE VERIFICATION
    creature.age.value=creature.age.limit
    assert DeathSystem.is_dead(creature) == True, "Creature with age = max age must die"

    
