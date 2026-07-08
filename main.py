from ui.entry_points.easy_entry_point import Main as MainEasy
from ui.entry_points.hard_entry_point import Main as MainHard
from ui.global_console import console
from ui.utils.cli import Inputs, StandardMessages


def main() -> None:
    while True:
        console.clear()
        console.print()
        console.print('Choose - Welcome, Karkinolution 🦀')
        console.print('[red]Hard [1][/]')
        console.print('[yellow]Easy [2][/]')
        console.print('Exit [3]')
        i = Inputs.question(['1', '2', '3'])
        if i == '1':
            main = MainHard()
            main.run()
            StandardMessages.enter_await()
            StandardMessages.pr(2)
        elif i == '2':
            main = MainEasy()
            main.run()
            StandardMessages.enter_await()
            StandardMessages.pr(2)
        elif i == '3':
            StandardMessages.exit()
            break
        
if __name__ == "main":
    main()