from karkinolution.organism.creatures import Creature, PregnantUterus
from karkinolution.organism.stats import LimitedValue
from karkinolution.core.error import EntityError
from karkinolution.organism.ontology import Temperament

COURAGE_FACTOR:dict[Temperament, float] = {
    Temperament.PASSIVE: 0.1,
    Temperament.NEUTRAL: 0.3,
    Temperament.AGGRESSIVE: 0.7,
    Temperament.TERRITORIAL: 1
}



TRADE_OFF:dict[Temperament, float] = {
    Temperament.PASSIVE: 0.1,
    Temperament.NEUTRAL: 0.5,
    Temperament.AGGRESSIVE: 0.89,
    Temperament.TERRITORIAL: 1
}

class Factors:
    '''
    1 -> All outputs must be in the interval [0, 1]
    2 -> All factors here must accept a creature as their only argument
    3 -> All antonyms cannot always be computed using a simple 1 - constant formula; most will require their own specific formulas.
    '''
    @staticmethod
    def get_pregnancy_risk(creature:Creature) -> float:
        factor = LimitedValue(0, 1)

        if not creature.pregnant:
            raise EntityError('Only pregnant creatures have pregnancy risk constant')
        assert isinstance(creature.uterus, PregnantUterus)

        factor.add(creature.uterus.gravity * 0.75)
        factor.sub(creature.uterus.number_children.ratio * 0.35)

        return factor.value
    @staticmethod
    def get_fear(creature:Creature) -> float:
        factor = LimitedValue(0, 1)

        ph_r = creature.physical_ratio

        if ph_r < 0.30:
            factor.add(ph_r - 0.05)

        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            factor.add(creature.uterus.gravity)
        
        factor.add(creature.senescence/1.7)
        return factor.value
    @staticmethod
    def get_courage(creature:Creature) -> float:
        basal = LimitedValue(COURAGE_FACTOR[creature.genome.core.behavior], 1)

        basal.sub(creature.senescence)

        basal.add(creature.genome.metabolism.mass/3)

        return basal.value
    @staticmethod
    def get_territorial_social_indifference_factor(creature:Creature) -> float:
        return 0.54 if creature.genome.core.behavior == Temperament.TERRITORIAL else 1
    @staticmethod
    def get_pregnant_social_interest_factor(creature:Creature) -> float:
        return 1.2 if creature.pregnant else 1
    @staticmethod
    def get_pregnant_territorial_avoidance_attack_factor(creature:Creature) -> float:
        assert isinstance(creature.uterus, PregnantUterus)
        return 0.2 * (creature.uterus.gravity + 1)
    