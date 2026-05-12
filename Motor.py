from Territory import Territory
from Crabs import Instincts, Crab, CrabsRegistry
from CrabsMentality import Mind, Planner
from random import sample
from CellCoord import Coord


class Motor:
    '''
    Hoje, motor é um monólito. 

    A seção "per creatures", deve ser substituida por uma forma de simular a criatura
    Ou seja, o motor apenas chamar a função que roda a criatura

    Sobre o negócio de verificação de morte. Não tenho ideias suficientes para saber onde colocar.
    '''
    @staticmethod
    def add(crab:Crab, registry:CrabsRegistry, territory:Territory, coord:Coord) -> None:
        registry.add(crab)
        territory.add(crab.id, coord)

    @staticmethod
    def delete(id:str, registry:CrabsRegistry, territory:Territory) -> Crab:
        crab = registry.get(id)

        registry.delete(id)
        territory.delete(id)

        return crab

    @staticmethod
    def step(territory:Territory, registry:CrabsRegistry) -> None:
        # REGEN CELLS
        for cell in territory.list_cells:
            cell.regen()

        # PER CREATURES

        # Decide Actions -> Interpretate -> Execute Action
        # Instincts -> Planner -> Planner (Mind)
        # Actions derivate from Mind
        for c in sample(list(registry.list_crabs), len(registry.list_crabs)):
            if c.is_dead:
                Motor.delete(c.id, registry, territory)
            
            c.energy.sub(1) # Custo por existir
            actions = Instincts.next_actions(c)
            interpretate = Planner.interpretate(actions, c.view)
            Planner.execute(interpretate, c, territory)
 