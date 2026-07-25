// tests/test_reproduction.cpp

#include <catch2/catch_test_macros.hpp>

#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/systems/reproduction.hpp>

#include <variant>

TEST_CASE("Uterus starts in the correct state based on gender") {
    SECTION("female creature starts with an empty uterus") {
        Creature female = CreatureFactory::gen_creature(CreatureConfig{.gender = Gender::FEMALE});
        REQUIRE(std::holds_alternative<EmptyUterus>(female.uterus));
    }

    SECTION("male creature has no uterus") {
        Creature male = CreatureFactory::gen_creature(CreatureConfig{.gender = Gender::MALE});
        REQUIRE(std::holds_alternative<std::monostate>(male.uterus));
    }
}

TEST_CASE("Conceiving turns an empty uterus into a pregnant one") {
    Creature female = CreatureFactory::gen_creature(CreatureConfig{
        .specie = CreatureSpecies::CRAB,
        .gender = Gender::FEMALE
    });

    Genome male_genome = global_creature_genomes.get_genome(CreatureSpecies::CRAB);
    UterusSystem::conceive(female, male_genome);

    REQUIRE(std::holds_alternative<PregnantUterus>(female.uterus));
}

TEST_CASE("Reproductive compatibility between creatures") {
    SECTION("different species cannot reproduce") {
        Creature female = CreatureFactory::gen_creature(CreatureConfig{
            .specie = CreatureSpecies::CRAB,
            .gender = Gender::FEMALE
        });
        Creature male = CreatureFactory::gen_creature(CreatureConfig{
            .specie = CreatureSpecies::HIPPOPOTAMUS,
            .gender = Gender::MALE
        });
        
        female.fertility.full();
        male.fertility.full();
        REQUIRE_FALSE(ReproductiveSystem::can_reproduce(female, Perceiver::perceive_creature(male)));
    }

    SECTION("same gender cannot reproduce") {
        Creature female_1 = CreatureFactory::gen_creature(CreatureConfig{
            .specie = CreatureSpecies::CRAB,
            .gender = Gender::FEMALE
        });
        Creature female_2 = CreatureFactory::gen_creature(CreatureConfig{
            .specie = CreatureSpecies::CRAB,
            .gender = Gender::FEMALE
        });

        REQUIRE_FALSE(ReproductiveSystem::can_reproduce(female_1, Perceiver::perceive_creature(female_2)));
    }

    SECTION("opposite gender, same species can reproduce") {
        Creature female = CreatureFactory::gen_creature(CreatureConfig{
            .specie = CreatureSpecies::CRAB,
            .gender = Gender::FEMALE
        });
        Creature male = CreatureFactory::gen_creature(CreatureConfig{
            .specie = CreatureSpecies::CRAB,
            .gender = Gender::MALE
        });
        female.fertility.full();
        male.fertility.full();  

        REQUIRE(ReproductiveSystem::can_reproduce(female, Perceiver::perceive_creature(male)));
    }
}