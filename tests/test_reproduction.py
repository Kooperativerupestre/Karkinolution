from karkinolution.organism.creatures import CreatureFactory, PregnantUterus, EmptyUterus, Gender
from karkinolution.systems.reproduction import UterusSystem, ReproductiveSystem
from karkinolution.organism.genetics import creatures_genomes, CreatureTypes
from karkinolution.decisions.perception import Perceiver
def test_uterus_female() -> None:
    female = CreatureFactory.gen_creature(gender=Gender.FEMALE)

    assert isinstance(female.uterus, EmptyUterus)

def test_uterus_male() -> None:
    male = CreatureFactory.gen_creature(gender=Gender.MALE)

    assert male.uterus is None

def test_uterus_conceive() -> None:
    female = CreatureFactory.gen_creature(gender=Gender.FEMALE, creature_type=CreatureTypes.CRAB)
    UterusSystem.conceive(female, creatures_genomes.get_type_genome(CreatureTypes.CRAB))
    assert isinstance(female.uterus, PregnantUterus)

def test_different_species_cannot_reproduce() -> None:
    female = CreatureFactory.gen_creature(gender=Gender.FEMALE, creature_type=CreatureTypes.CRAB)
    male = CreatureFactory.gen_creature(gender=Gender.MALE, creature_type=CreatureTypes.HIPPOPOTAMUS)

    assert ReproductiveSystem.can_reproduce(female, Perceiver.perceive_entity(male)) == False

def test_same_gender_cannot_reproduce() -> None:
    female_1 = CreatureFactory.gen_creature(gender=Gender.FEMALE, creature_type=CreatureTypes.CRAB)
    female_2 = CreatureFactory.gen_creature(gender=Gender.FEMALE, creature_type=CreatureTypes.CRAB)

    assert ReproductiveSystem.can_reproduce(female_1, Perceiver.perceive_entity(female_2)) == False

def test_opposite_gender_same_species_can_reproduce() -> None:
    female = CreatureFactory.gen_creature(
        gender=Gender.FEMALE,
        creature_type=CreatureTypes.CRAB,
    )
    male = CreatureFactory.gen_creature(
        gender=Gender.MALE,
        creature_type=CreatureTypes.CRAB,
    )

    assert ReproductiveSystem.can_reproduce(
        female,
        Perceiver.perceive_entity(male),
    )