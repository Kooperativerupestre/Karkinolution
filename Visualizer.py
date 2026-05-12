from Crabs import ViewCrab, CrabsRegistry, Crab
from Territory import Territory
from Motor import Motor
from CellCoord import Coord

# É factual que o projeto ainda engatinha em densidade informacional e funcionalidades
# Por conta disso, decidi agora por enquanto, permanecer com uma interface ainda extremamente básica
# Que não é "Interativa" ainda para o úsuario
# Pois fazer uma interface agora, seria como tentar projetar uma visão 360 graus num mundo plano preto e branco
# Visualizer.py também funcionará como um Main por enquanto

class Visualizer:
    @staticmethod
    def show(territory:Territory, registry:CrabsRegistry) -> None:
        print(f'(Coordenada x ID): {territory.grid}')
        print(f'(ID x Coordenada): {territory.positions}')

        print('-'*30, end='\n\n\n')

        for crab in registry.list_crabs:
            print(crab)

t = Territory()
t.generate(9, 9)
r = CrabsRegistry()

Motor.add(Crab(), coord=Coord(0, 0), registry=r, territory=t)

Visualizer.show(t, r)

Motor.step(t, r)

Visualizer.show(t, r)


Motor.step(t, r)

Visualizer.show(t, r)


Motor.step(t, r)

Visualizer.show(t, r)
