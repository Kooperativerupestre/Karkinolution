from organism.creatures import Creature
from decisions.actions import  IntentActs, Intent
from decisions.instincts import PlannerFindFood, PlannerFindMatch, Instincts, try_call_reproductive_buffer
from decisions.perception import Perception
from decisions.presets import EatPreset, MovePreset, AttackPreset, ReproducePreset
from systems.reproductivebuffer import ReproductiveBuffer

class IntentResolver:
    @staticmethod
    def to_nothing_intent(creature:Creature) -> None:
        creature.intent = Intent(IntentActs.NOTHING)
    @staticmethod
    def cancel_invalid_intents(creature:Creature) -> None:
        intent = creature.intent.intent
        intent_time = creature.intent.time
        if creature.hungry < creature.genome.metabolism.max_hungry and intent_time > 2 and intent == IntentActs.FIND_FOOD:
            IntentResolver.to_nothing_intent(creature)
        if intent_time > 5 and intent ==  IntentActs.FIND_MATCH:
            IntentResolver.to_nothing_intent(creature)
    @staticmethod
    def transform_to_preset(creature:Creature, perception:Perception) -> MovePreset | EatPreset | AttackPreset | None:
        if creature.intent.intent == IntentActs.FIND_FOOD:
            act = PlannerFindFood.plan_intent(perception, creature)
            return act
        
        elif creature.intent.intent == IntentActs.FIND_MATCH:
            act = PlannerFindMatch.plan_intent(perception, creature)
            return act

    @staticmethod
    def update_intent(creature:Creature, reproductive_buffer:ReproductiveBuffer) -> (
        MovePreset | EatPreset | AttackPreset | ReproducePreset | None):
        creature.intent.time += 1
        if creature.intent.intent == IntentActs.NOTHING:
            acts = Instincts.take(creature, reproductive_buffer)
            Instincts.apply_noise(acts)

            act = Instincts.chose(acts)
            creature.intent = act
            try_call_reproductive_buffer(act, creature, reproductive_buffer)




    @staticmethod
    def resolve_intent(creature:Creature, reproductive_buffer:ReproductiveBuffer, perception:Perception) -> MovePreset | AttackPreset | EatPreset | None:
        IntentResolver.cancel_invalid_intents(creature)
        IntentResolver.update_intent(creature, reproductive_buffer)
        return IntentResolver.transform_to_preset(creature, perception)