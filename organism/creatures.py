from __future__ import annotations
from organism.genetics import Genome, CreatureTypes, _creatures_genomes
from organism.stats import LimitedValue, Energy, Life, Age, Fertility
from decisions.actions import Intent, IntentActs
from organism.ontology import Gender, AtackedEvent

from core.error import ReproductiveError, IdNotFoundError, GenderError, IdAlreadyExistsError
from organism.identity import Id, EntityTypes, gen_id
from random import uniform
from math import exp
from dataclasses import dataclass
from core.coord import Coord
from utils.namegenerator import gen_name



MAX_STRENGTH = 35


class Gestation:
    def __init__(self, limit:int):
        self.value = 0
        self.limit = limit
    @property
    def is_overdue(self) -> bool:
        return self.value > self.limit + 1
    @property
    def is_ready_to_born(self) -> bool:
        return self.value >= self.limit
    @property
    def is_premature(self) -> bool:
        return self.value + 1 < self.limit 
    @property
    def is_at_point(self) -> bool:
        return self.value == self.limit
    @property
    def death_factor(self) -> float: # [0, 1]
        factor = 0

        if self.is_premature:
            factor += self.time_to_born/(self.time_to_born + self.limit)*1.40
        elif self.is_overdue:
            factor += self.overdue/(self.overdue + self.limit)
        else: # at_point
            pass
        return factor

    @property
    def overdue(self) -> int:
        return max(0, self.value - self.limit)
    @property
    def time_to_born(self) -> int:
        return max(0, self.limit - self.value)

    @property
    def ratio(self) -> float:
        return self.value/self.limit
    @property
    def overdue_factor(self) -> float:
        return abs(self.overdue)


class Uterus:
    def __init__(self, female_genome:Genome):
        self.female_genome = female_genome
        self.male_genome: None | Genome = None
        self.number_children: None | LimitedValue = None
        self.gestation: None | Gestation = None

    @property
    def all_children_borned(self) -> bool:
        return self.number_children.value == self.number_children.limit # type: ignore
    @property
    def pregnant(self) -> bool:
        return self.gestation is not None
    
    
    @property
    def pregnancy_cost(self) -> float:
        if not self.pregnant:
            raise ReproductiveError('Uterus {} is not pregnant'.format(self))
            
        return (1 + self.gestation.value/2)*self.number_children # type: ignore
    @property
    def pregnancy_factor(self) -> float:
        if not self.pregnant:
            raise ReproductiveError('Uterus {} is not pregnant'.format(self))
        
        return (1 + self.gestation.ratio) * self.number_children.value / 6 # type: ignore
    @property
    def birth_energy(self) -> float:
        if not self.pregnant:
            raise ReproductiveError('Uterus {} is not pregnant'.format(self))
        return self.pregnancy_cost / self.number_children.value # type: ignore
    
@dataclass(frozen=True)
class CreatureInterface:
    name:str
    id:Id
    age:Age
    energy:Energy
    life:Life

    pregnant:bool
    intent:Intent
    position:Coord
    @property
    def hungry(self) -> float:
        return 1 - self.energy.ratio


class Creature:
    def __init__(self,
                 genome:Genome,
                 gender:Gender,
                 name:str,
                 initial_energy:Energy,
                 position:Coord,
                 id:str,
                 ):
        self.genome = genome
        self.gender = gender
        self.name = name
        self.id = Id(id, EntityTypes.CREATURE)

        self.energy = initial_energy
        self.life = Life(value=genome.body.life_limit, limit=genome.body.life_limit)
        self.fertility = Fertility(value=genome.reproduction.fertility_limit, limit=genome.reproduction.fertility_limit)
        self.uterus = Uterus(self.genome) if self.gender is Gender.FEMALE else None
        self.age = Age(value=0, limit=genome.body.life_limit)

        self.intent: Intent = Intent(IntentActs.NOTHING)
        self.last_atack: None | AtackedEvent = None
        self.position = position

    @property
    def hungry(self) -> float:
        return 1 - self.energy.ratio
    @property
    def needed_energy(self) -> float:
        if self.gender is Gender.FEMALE:
            if self.uterus.pregnant: # type: ignore
                return self.hungry * uniform(0.95, 1) * self.energy.limit
        return self.hungry * uniform(0.8, 1) * self.energy.limit
    @property
    def reproductive_maturity(self) -> float:
        return exp(-(self.age.ratio - 0.45)**2/(0.2)**2)
    @property
    def reproductive_factor(self) -> float:
        return (self.energy.value * self.genome.reproduction.extra_reproduction_multiplier)/self.genome.reproduction.reproduction_cost
    @property
    def reproductively_capable(self) -> bool:
        energy_condition = self.energy.value * self.genome.reproduction.extra_reproduction_multiplier >= self.genome.reproduction.reproduction_cost
        return self.fertility.reproductive_capability() and not self.pregnant and energy_condition
    
    @property
    def pregnancy_factor(self) -> int | float:
        if self.gender is not Gender.FEMALE:
            raise GenderError('Creature {} is not female'.format(self))
        if not self.uterus.pregnant: # type: ignore
            raise ReproductiveError('Creature {} is not pregnant'.format(self))
        return 1 + self.uterus.gestation_time/2 # type: ignore
    @property
    def strength_factor(self) -> int | float:
        return self.genome.body.strength / MAX_STRENGTH
    @property
    def physical_ratio(self) -> int | float:
        '''
        Always return a value in [0, 1]
        '''
        return (self.energy.ratio*1.2 + self.life.ratio*1.5 + self.strength_factor*0.9)/3.6
    @property
    def senescence(self) -> float:
        return self.age.value/(self.age.limit**1.4)
    
    @property
    def pregnant(self) -> bool:
        return self.uterus is not None and self.uterus.pregnant

    @property
    def interface(self) -> CreatureInterface:
        pregnant = True if self.gender == Gender.FEMALE and self.uterus.pregnant else False # type: ignore
        return CreatureInterface(
            self.name,
            self.id,
            self.age,
            self.energy,
            self.life,
            pregnant,
            self.intent,
            self.position
        )
    
    
    
    def __str__(self):
        return f'({self.name} | {self.id})'

class Corpse:
    def __init__(self, energy:Energy, id:str, decomposition_time:Age, position:Coord):
        self.energy = energy
        self.id = Id(id, EntityTypes.CORPSE)
        self.decomposition_time = decomposition_time
        self.position = position
    
    @property
    def ready_to_disapear(self) -> bool:
        return self.decomposition_time.value == self.decomposition_time.limit
    


    



class EntitysRegistry:
    def __init__(self):
        self.entitys:dict[Id, Corpse | Creature] = {}

    def add(self, entity:Creature | Corpse) -> str:
        if entity.id in self.entitys:
            raise IdAlreadyExistsError('ID {} exists'.format(entity.id))
        self.entitys[entity.id] = entity
        return entity.id.id
    def delete(self, id:Id) -> None:
        if id not in self.entitys:
            raise IdNotFoundError('ID {} does not exists'.format(id))
        del self.entitys[id]
    
    def get_creature(self, id:Id) -> Creature:
        if id.e_type != EntityTypes.CREATURE:
            raise TypeError('Can not get creature: ID {} type must be CREATURE'.format(id))
        if id not in self.entitys:
            raise IdNotFoundError('ID {} does not exists'.format(id))
        return self.entitys[id] # type: ignore
    
    def get_corpse(self, id:Id) -> Corpse:
        if id.e_type != EntityTypes.CORPSE:
            raise TypeError('Can not get corpse: ID {} type must be CORPSE'.format(id))
        if id not in self.entitys:
            raise IdNotFoundError('ID {} does not exists'.format(id))
        return self.entitys[id] # type: ignore
    
    def get(self, id:Id) -> Creature | Corpse:
        if id not in self.entitys:
            raise IdNotFoundError('ID {} does not exists'.format(id))
        return self.entitys[id]
    
    
class CreatureFactory:
    @staticmethod
    def gen_creature(
        position:Coord,
        creature_type:CreatureTypes | None = None,
        id:str | None = None,
        initial_energy:Energy | None = None,
        genome:Genome | None = None
    ) -> Creature:
        
        if creature_type is None:
            creature_type = CreatureTypes.choice()
        if id is None:
            id = gen_id()
        if genome is None:
            genome = _creatures_genomes[creature_type]
        if initial_energy is None:
            initial_energy = Energy(uniform(0.5, 1) * genome.metabolism.energy_limit, genome.metabolism.energy_limit)

        return Creature(
            genome, 
            Gender.choice(),
            gen_name(),
            initial_energy,
            position,
            id
        )
    

    