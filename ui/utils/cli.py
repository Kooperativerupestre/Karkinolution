from ui.global_console import console
from rich.style import Style
from core.coord import Coord



# STYLES

class Styles:
    RED_EXTREME = Style(color="red")
    PHOENIX_YELLOW = Style(color="bright_yellow", bold=True)
    BLACK_WHITE = Style(color="black", bold=True)
    BLUE = Style(color='bright_blue')
    ORANGE = Style(color='orange3', bold=True)
# STANDARD MESSAGES

class StandardMessages:
    @staticmethod
    def creature_birth(name:str) -> None:
        console.print('Creature {} was born!!!'.format(name), style=Styles.PHOENIX_YELLOW)
    @staticmethod
    def creature_died(name:str) -> None:
        console.print('Creature {} died ☠️ ...'.format(name), style=Styles.BLACK_WHITE)
    @staticmethod
    def created_creature(name:str) -> None:
        console.print('You created the creature {}'.format(name), style=Styles.PHOENIX_YELLOW)
    @staticmethod
    def killed_creature(name:str) -> None:
        console.print('You killed the creature {}'.format(name), style=Styles.BLACK_WHITE)
    @staticmethod
    def cannot_execute(action_name:str) -> None:
        console.print('Cannot execute the action {}...'.format(action_name.title()), style=Styles.RED_EXTREME)
    @staticmethod
    def pr(n:int = 3) -> None:
        console.print('\n'*n)
    @staticmethod
    def error(message:str | None = None) -> None:
        if message is None:
            console.print('ERROR!', style=Styles.RED_EXTREME)
        else:
            console.print('ERROR: {}'.format(message), style=Styles.RED_EXTREME)
        StandardMessages.pr(n=2)
    @staticmethod
    def show_presets() -> None:
        console.print('Crab chaos 🦀 [1]', style=Styles.ORANGE)
        console.print('🧿Paranoic🧿 [2]', style=Styles.BLUE)
        console.print('Titanic 🪨 [3]')
        console.print('Normal [4]')

    @staticmethod
    def exit() -> None:
        console.print("Goodbye, 🦀", style=Styles.PHOENIX_YELLOW)
    @staticmethod
    def enter_await() -> None:
        console.input('\nPress enter to continue >>>')
class Inputs:
    @staticmethod
    def question(options:list[str]) -> str:
        while True:
            try:
                option = input(">>>  ")
                if option not in options:
                    raise ValueError
                return option
            except ValueError:
                StandardMessages.error("Error: Invalid option")
                continue
    @staticmethod
    def continue_question() -> bool:
        while True:
            r = console.input("Continue? [Y/N] >>>  ").strip().upper()
            if r == "Y":
                return True
            elif r == "N":
                return False
            else:
                StandardMessages.error()
                continue
    @staticmethod
    def get_coord() -> Coord:
        while True:
            try:
                x = int(input("x: "))
                break
            except TypeError:
                StandardMessages.error("x must be integer")
                continue
        while True:
            try:
                y = int(input("y: "))
                break
            except TypeError:
                StandardMessages.error("y must be integer too")
                continue
        return Coord(x, y)