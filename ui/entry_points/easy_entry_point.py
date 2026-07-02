from ui.global_console import console
from rich.layout import Layout
from rich.panel import Panel
from ui.interfaces import WorldInterface, InterfaceFactory
from ui.utils.cli import StandardMessages, Inputs
from map.world import WorldPresets, World
from ui.utils.visualizations import render_full_creature, render_cell, render_stats_panel, render_map_grid, render_live, render_simply_creature, render_corpse
from ui.api.api_world_interface import APIWorld
from time import sleep
from organism.identity import EntityTypes

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

        stats = render_stats_panel(world)

        layout["commands"].update(commands)
        layout["stats"].update(stats)

        return layout


class Tabs:
    @staticmethod
    def create_creature_tab(world: World) -> None:
        name = input("Name: ").strip()
        result = APIWorld.create_creature(name, world)

        if not result:
            StandardMessages.cannot_execute("Create Creature")
            return None
        StandardMessages.created_creature(name)
        return None

    @staticmethod
    def kill_creature_tab(world: World) -> None:
        id = input('ID: ').strip()
        creature = APIWorld.get_creature_simply_interface(id, world)
        if creature is None:
            StandardMessages.cannot_execute("Get & Kill Creature")
            return None
        
        APIWorld.kill_creature(id, world)
        StandardMessages.killed_creature(creature.name)

    @staticmethod
    def recreate_with_preset() -> World | None:
        StandardMessages.show_presets()
        console.log("Exit ❌ [5]")
        i = Inputs.question(['1', '2', '3', '4', '5'])

        if i == '5':
            return None

        return APIWorld.reset_with_a_preset(i)


class ViewTab:
    @staticmethod
    def menu():
        console.print("See grid 🌎 [1]")
        console.print("See creature 🐊🩻 [2]")
        console.print("See cell 🌱 [3]")
        console.print("Live view for 10 times 👁️‍🗨️ [4]")
        console.print("See all creatures 🐊🦀🦛🐟 [5]")
        console.print("Exit [6] ❌ ")
        
    @staticmethod
    def see_grid(world: World) -> None:
        console.print(render_map_grid(world.territory, world.entity_map, world.entities))

    @staticmethod
    def see_creature(world: World) -> None:
        id = input('ID: ').strip()
        creature = APIWorld.get_creature_full_interface(id, world)
        if creature is None:
            StandardMessages.cannot_execute('Get creature')
            return None
        console.print(render_full_creature(creature))

    @staticmethod
    def see_cell(world: World) -> None:
        coord = Inputs.get_coord()
        cell = APIWorld.get_cell_interface(coord, world)
        if cell is None:
            StandardMessages.cannot_execute("Get cell")
            return None
        console.print(render_cell(cell))

    @staticmethod
    def live_view_for_10_times(world: World) -> None:
        render_live(world, 10)

    @staticmethod
    def view_tab(world: World) -> None:
        while True:
            console.clear()
            ViewTab.menu()
            StandardMessages.pr(2)

            i = Inputs.question(['1', '2', '3', '4', '5', '6'])

            if i == '1':
                ViewTab.see_grid(world)
                StandardMessages.enter_await()
            elif i == '2':
                ViewTab.see_creature(world)
                StandardMessages.enter_await()
            elif i == '3':
                ViewTab.see_cell(world)
                StandardMessages.enter_await()
            elif i == '4':
                ViewTab.live_view_for_10_times(world)
            elif i == '5':
                from rich.columns import Columns

                renderables:list[Panel] = []
                for e in world.entities.entities:
                    e_type = e.id.e_type

                    if e_type == EntityTypes.CORPSE:
                        renderables.append(render_corpse(InterfaceFactory.create_corpse_interface(e))) # type: ignore
                    else: # creature
                        renderables.append(render_simply_creature(InterfaceFactory.create_simply_creature_interface(e))) # type: ignore
                

                console.print(Columns(renderables, equal=True, expand=False))
                
                StandardMessages.pr(2)
                StandardMessages.enter_await()
                    
                        
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
                Tabs.create_creature_tab(self.world)
                sleep(1) 
            elif i == '2':
                Tabs.kill_creature_tab(self.world)
                sleep(1)
            elif i == '3':
                APIWorld.run_1_time(self.world)
            elif i == '4':
                APIWorld.run_5_time(self.world)
            elif i == '5':
                ViewTab.view_tab(self.world)
            elif i == '6':
                result = Tabs.recreate_with_preset()
                if result is not None:
                    console.log("Reseting...")
                    sleep(0.5)
                    self.world = result
            elif i == '7':
                StandardMessages.exit()
                break