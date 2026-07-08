from time import sleep

from rich.layout import Layout
from rich.panel import Panel

from karkinolution.terrain.world import (
    World,
    WorldPresets,
)

from karkinolution.ui.api.api import BasicAPI
from karkinolution.ui.entry_points.basic_tabs import (
    BTabs,
    BViewTabs,
    TypeView,
)
from karkinolution.ui.global_console import console
from karkinolution.ui.interfaces import (
    InterfaceFactory,
    WorldInterface,
)
from karkinolution.ui.utils.cli import (
    Inputs,
    StandardMessages,
)
from karkinolution.ui.utils.visualizations import Renders

class Messages:
    @staticmethod
    def main_menu(world: WorldInterface) -> Layout:
        layout = Layout(size=11)

        layout.split_row(
            Layout(name="commands", ratio=1),
            Layout(name="stats", ratio=1)
        )

        commands = Panel(
            "[bold cyan]Commands[/]\n\n"
            "[green]1[/] - Create Creature 🪬\n"
            "[green]2[/] - Kill Creature ☠️\n"
            "[green]3[/] - Run 1 time ⏳\n"
            "[green]4[/] - Run 5 time ⏳\n"
            "[green]5[/] - See 👀\n" 
            "[green]6[/] - Reset with a preset 🌏\n"
            "[green]7[/] - Exit 🚫",
            title="Actions",
            border_style="cyan",
            height=11,
            expand=False
        )

        stats = Renders.render_stats_panel(world)

        layout["commands"].update(commands)
        layout["stats"].update(stats)

        return layout



class ViewTab:
    @staticmethod
    def menu():
        console.print("See grid 🌎 [1]")
        console.print("See creature 🐊🩻 [2]")
        console.print("See cell 🌱 [3]")
        console.print("Live view for n times 👁️‍🗨️ [4]")
        console.print("See all creatures 🐊🦀🦛🐟 [5]")
        console.print("Exit [6] ❌ ")


    @staticmethod
    def view_tab(world: World) -> None:
        while True:
            console.clear()
            ViewTab.menu()
            StandardMessages.pr(2)

            i = Inputs.question(['1', '2', '3', '4', '5', '6'])

            if i == '1':
                BViewTabs.see_grid(world, TypeView.EASY)
                StandardMessages.enter_await()
            elif i == '2':
                BViewTabs.see_creature(world)
                StandardMessages.enter_await()
            elif i == '3':
                BViewTabs.see_cell(world)
                StandardMessages.enter_await()
            elif i == '4':
                BViewTabs.live_view(world, TypeView.EASY)
            elif i == '5':
                BViewTabs.see_all_entities(world)
                    
                        
            elif i == '6':
                StandardMessages.pr(1)
                break


class Main:
    def __init__(self):
        self.world = WorldPresets.normal()

    def run(self) -> None:
        while True:
            console.clear()
            
            menu_layout = Messages.main_menu(InterfaceFactory.create_world_interface(self.world))
            console.print(menu_layout)
            
            i = Inputs.question(['1', '2', '3', '4', '5', '6', '7'])

            if i == '1':
                BTabs.create_random_creature(self.world)
                sleep(1) 
            elif i == '2':
                BTabs.kill_creature(self.world)
                sleep(1)
            elif i == '3':
                BasicAPI.run_n_time(self.world, 1)
            elif i == '4':
                BasicAPI.run_n_time(self.world, 5)
            elif i == '5':
                ViewTab.view_tab(self.world)
            elif i == '6':
                result = BTabs.recreate_with_preset()
                if result is not None:
                    console.log("Reseting...")
                    sleep(0.5)
                    self.world = result
            elif i == '7':
                StandardMessages.exit()
                break