from Crabs import Crab, Actions, ViewCrab
from CellCoord import Cell, Coord
from Territory import Geometry, Territory
from random import choices, choice

class Mind:
    @staticmethod
    def eat(crab:Crab, cell:Cell) -> None:
        cost = min(cell.energy.value, crab.energy_needed)
        
        crab.energy.add(cost)
        cell.energy.sub(cost)
    
    @staticmethod
    def avaliable_movimentation(crab:Crab, territory:Territory) -> Coord:
        '''
        Desacoplar isso no futuro. A percepção de área deve estar em uma classe separada.
        Tolerada agora por não ter necessidade de criar uma classe para apenas 2 percepções
        No estágio atual. Aqui, além de perceber, ele também já escolhe qual é melhor, fusão que precisa ser desacoplada.
        '''
        
        neighbors = list(Geometry.neighbors_4(territory.get_coord(crab.id), territory).keys())

        compatible_neighbors:list[Coord] = []

         
        for c in neighbors:
            if not territory.occupied(c):
                compatible_neighbors.append(c)


        # Retorna um set das cordenadas
        # Possível verificação futura de intencionalides baseados em instintos


        return choice(compatible_neighbors)             


    @staticmethod
    def move(crab:Crab, territory:Territory, coord:Coord) -> None:
        territory.move(crab.id, coord)

        crab.energy.sub(1)

class Planner:
    @staticmethod
    def interpretate(actions:list[Actions], crab:ViewCrab) -> Actions:
        '''
        O sistema de prioridades daqui, vai ser usado de forma mais analítica e fragmentada
        Alguns if's, irão analisar certas condições, e com base nisso aplicar essas consequências
        Nas probabilidades de cada ação. Como ainda não tenho tanto conhecimento de como pode
        Ser a hierarquia, e também não separei ainda a classe de percepção, não faz sentido
        Eu tentar adicionar mais coisas agora.

        Nota: Usar menos if
        '''

        priority = {
            Actions.EAT: 1,
            Actions.MOVE: 1,
            Actions.NOTHING: 0.5
        }


        if crab.hungry > 0.75:
            priority[Actions.EAT] *= 1.2
            priority[Actions.MOVE] /= 0.8

    
        options:list[Actions] = []
        weights:list[int] = []


        for act in actions:
            options.append(act)
            weights.append(priority[act])
        return choices(options, weights=weights, k=1)[0]

    @staticmethod
    def execute(action:Actions, crab:Crab, territory:Territory) -> None:
        if action == Actions.EAT:
            cell = territory.get_cell_by_id(crab.id)
            
            Mind.eat(crab, cell)
        elif action == Actions.MOVE:
            new_coord = Mind.avaliable_movimentation(crab, territory)
            Mind.move(crab, territory, new_coord)
