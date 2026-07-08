from karkinolution.organism.creatures import (
    Corpse,
    Creature,
    PregnantUterus,
)
from karkinolution.organism.identity import gen_id
from karkinolution.organism.stats import (
    Age,
    Energy,
)
            
        
class DeathSystem:
    @staticmethod
    def generate_corpse(creature:Creature) -> Corpse:
        energy:float = creature.energy.value * 0.8
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            energy += creature.uterus.pregnancy_cost * 2

        decomposition_time = round(energy/creature.energy.limit * 7.5)
        return Corpse(Energy(energy, energy), gen_id(), Age(0, decomposition_time), creature.position)
    @staticmethod
    def is_dead(creature:Creature) -> bool:
        if creature.life.value == 0:
            return True
        if creature.energy.value == 0:
            return True
        if creature.age.value >= creature.age.limit:
            return True
        return False
          
