from random import choice, choices

def choice_bool(yes_weight:int | float, no_weight:int | float) -> bool:
    return choices([True, False], weights=[yes_weight, no_weight], k=1)[0]

