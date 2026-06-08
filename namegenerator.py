from random import choice, choices

class LeterPool:
    def __init__(self, leters:str):
        self.leters = leters
    @property
    def to_list(self) -> list[str]:
        return [c for c in self.leters]
    
    
    def pick(self) -> str:
        return choice(self.to_list)


vowels = LeterPool('aeiouy')
long_vowels = LeterPool('āēīōūȳ')
high_vowels = LeterPool('áéíóúý')
geral_vowels = LeterPool('aeiouyāēīōūȳáéíóúý')

plosives = LeterPool('pbtdkg')
fricatives = LeterPool('fvszh')
liquids = LeterPool('rl')
nasals = LeterPool('mn')
aproximants = LeterPool('jw')

consoants = LeterPool('pbtdkgfvszhrlmnjw')


class Pool:
    commands = {
        'v': vowels.pick,
        'lv': long_vowels.pick,
        'hv': high_vowels.pick,
        'gv': geral_vowels.pick,

        'p': plosives.pick,
        'f': fricatives.pick,
        'l': liquids.pick,
        'n': nasals.pick,
        'a': aproximants.pick,

        'c': consoants.pick
    }
    @staticmethod
    def interpretate(command:str) -> str:
        '''
        v = (aeiouy)
        lv = (āēīōūȳ)
        hv = (áéíóúý)

        p = (pbtdkg)
        f = (fvszh)
        l = (rl)
        n = (mn)
        a = (wj)

        c = (pbtdkgfvszhrlmnwj)
        / = command separator
        '''
        final_text = ''
        cut_text = command.split('/')
        for c in cut_text:
            if c in Pool.commands:
                final_text += Pool.commands[c]()
            else:
                final_text += c
        return final_text
    @staticmethod
    def weight_pick(commands:dict[str, int | float]) -> str:
        command_choiced = choices(list(commands.keys()), weights=list(commands.values()), k=1)[0]

        return Pool.interpretate(command_choiced)
    

def gen_name() -> str:
    commands = {'c/gv/gv/a': 2,
                'c/hv/a/v': 2,
                'hv/v/c/f': 1}
    return Pool.weight_pick(commands) # type: ignore