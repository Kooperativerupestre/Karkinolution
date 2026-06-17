from organism.creatures import Creature
from decisions.actions import  IntentActs
from decisions.instincts import Planner, ReproductiveBuffer, DecideIntention
from decisions.perception import Perception
from systems.presets import EatPreset, MovePreset, AttackPreset, ReproducePreset

class IntentResolver:
    @staticmethod
    def cancel_invalid_intents(creature:Creature) -> None:
        intent = creature.intent.intent
        intent_time = creature.intent.time
        if creature.hungry < creature.genome.metabolism.max_hungry and intent_time > 2 and intent == IntentActs.FIND_FOOD:
            creature.intent.intent = IntentActs.NOTHING

        if intent_time > 5 and intent ==  IntentActs.FIND_MATCH:
            creature.intent.intent = IntentActs.NOTHING
    @staticmethod
    def transform_to_preset(creature:Creature, perception:Perception) -> MovePreset | EatPreset | AttackPreset | None:
        if creature.intent.intent == IntentActs.FIND_FOOD:
            act = Planner.plan_find_food_intent(perception, creature)
            return act
        
        elif creature.intent.intent == IntentActs.FIND_MATCH:
            act = Planner.plan_find_match_intent(perception)
            return act

    @staticmethod
    def update_intent(creature:Creature, reproductive_buffer:ReproductiveBuffer) -> (
        MovePreset | EatPreset | AttackPreset | ReproducePreset | None):

        if creature.intent.intent == IntentActs.NOTHING:
            creature.intent = DecideIntention.decide(creature, reproductive_buffer)
    @staticmethod
    def resolve_intent(creature:Creature, reproductive_buffer:ReproductiveBuffer, perception:Perception) -> MovePreset | AttackPreset | EatPreset | None:
        IntentResolver.cancel_invalid_intents(creature)
        IntentResolver.update_intent(creature, reproductive_buffer)
        return IntentResolver.transform_to_preset(creature, perception)