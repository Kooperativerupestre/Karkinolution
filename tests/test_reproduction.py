from organism.creatures import CreatureFactory, PregnantUterus, EmptyUterus, Gender
from systems.reproduction import UterusSystem
from organism.genetics import creatures_genomes, CreatureTypes

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

