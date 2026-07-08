from ui.global_console import console
from rich.layout import Layout
from rich.panel import Panel
from ui.interfaces import WorldInterface, InterfaceFactory
from ui.utils.cli import StandardMessages, Inputs, stand_by
from map.world import WorldPresets, World
from ui.utils.visualizations import Renders, Viewer
from ui.api.api import HardApiWorld, BasicAPI, Outputs, ApiInterface
from ui.entry_points.basic_tabs import BViewTabs, TypeView, BTabs
from rich.box import SQUARE



class Messages:
    @staticmethod
    def main_menu(world: WorldInterface) -> Layout:
        layout = Layout(size=11)

        layout.split_row(
            Layout(name="commands 1"),
            Layout(name="commands 2"),
            Layout(name="stats")
        )

        commands_1 = Panel(
            "[bold cyan]Creature's[/]\n\n"
            "[green]1[/] - Create Creature (random) ➕  \n"
            "[green]2[/] - Create Creature (complete) 🪬  \n"
            "[green]3[/] - Kill Creature 🗡️  \n"
            "[green]4[/] - Turn pregnant 🪬 🧬  \n"
            "[green]5[/] - Turn gender 🪬 ⚧️  \n" 
            "[green]6[/] - Move creature 🪬 🫠  \n",
            title="Actions",
            border_style="cyan",
            height=11,
            expand=False,
            box=SQUARE
        )

        commands_2 = Panel(
            "[bold cyan]God's eyes[/]\n\n"
            "[blue]7[/] - See 🧿\n"
            "[green]8[/] - Run 1 times ⏳\n"
            "[green]9[/] - Run 5 times ⏳\n"
            "[green]10[/] - Run 10 times ⏳\n"
            "[green]11[/] - Run n times ⏳\n"
            "[green]12[/] - Get all log 📜\n"
            "[yellow]13[/] - Reset with a preset\n"
            "[red]14[/] - Exit\n",
            title="Powers",
            border_style="cyan",
            height=12,
            expand=False,
            box=SQUARE
        )

        stats = Renders.render_stats_panel(world)

        layout["commands 1"].update(commands_1)
        layout["commands 2"].update(commands_2)
        layout["stats"].update(stats)

        return layout   
    @staticmethod
    def view_menu(world:WorldInterface) -> Panel:
        return Panel(
            "[bold yellow] See🧿\n"
            "[green]1[/] - See creature 🦀🩻\n"
            "[green]2[/] - See cell\n"
            "[green]3[/] - See all entities 🐟 🦛 🐊 🦀 🩻 \n"
            "[green]4[/] - Live view, n times 🌎 👀 🔴 \n"
            "[green]5[/] - Cascade view, n times 🌎 👀 \n"
            "[green]6[/] - See grid 🌎 \n"
            "[red]7[/] - Exit\n"
        )





class Tabs:
    @staticmethod
    def create_complete_creature(world:World) -> None:
        name = input('Name: ').strip().capitalize()

        gender = Inputs.get_gender()
        specie = Inputs.get_specie()
        StandardMessages.pr(1)
        StandardMessages.enter_coordinate()
        position = Inputs.get_coord()

        r = HardApiWorld.create_creature(world, name, gender, specie, position)

        if r != Outputs.OK:
            StandardMessages.cannot_execute('Create creature {}'.format(name))
            StandardMessages.error(r.name)
            return None
        StandardMessages.created_creature(name)
        stand_by()


    @staticmethod
    def turn_pregnant(world:World) -> None:
        id = Inputs.get_id(world)
        if id is None:
            return None
        
        r = HardApiWorld.turn_pregnant(world, id)
        name = ApiInterface.get_creature_interface(id, world).name

        if r != Outputs.OK:
            StandardMessages.cannot_execute('Turn pregnant creature {}'.format(name))
            return None
        console.print('Creature {} is pregnant now'.format(name))
        stand_by()
    @staticmethod
    def move_creature(world:World) -> None:
        id = Inputs.get_id(world)
        if id is None:
            return None
        StandardMessages.enter_coordinate()
        new_coord = Inputs.get_coord()
        c_i = ApiInterface.get_creature_interface(id, world)

        name = c_i.name
        old_position = c_i.position

        r = HardApiWorld.move(world, id, new_coord)
        if r != Outputs.OK:
            StandardMessages.cannot_execute('Move creature {}'.format(name))
            return None
        console.print('Moved the creature {} position {} -> position {}'.format(name, old_position, new_coord))
        stand_by()
    @staticmethod
    def turn_gender(world:World) -> None:
        id = Inputs.get_id(world)
        if id is None:
            return None
        HardApiWorld.turn_gender(world, id)
        console.print('Gender has turned')
        stand_by()
    @staticmethod
    def run_n_times(world:World, n:int) -> None:
        BasicAPI.run_n_time(world, n)
        StandardMessages.passed_time(n)
    @staticmethod
    def get_all_log(world:World) -> None:
        console.print(Renders.render_log_panel(world.log))
        StandardMessages.enter_await()

class ViewTab:
    @staticmethod
    def main(world:World) -> None:
        while True:
            console.print(Messages.view_menu(InterfaceFactory.create_world_interface(world)))

            i = Inputs.question(['1', '2', '3', '4', '5', '6', '7'])

            if i == '1':
                BViewTabs.see_creature(world)
            elif i == '2':
                BViewTabs.see_cell(world)
            elif i == '3':
                BViewTabs.see_all_entities(world)
            elif i == '4':
                BViewTabs.live_view(world, TypeView.HARD)
            elif i == '5':
                ViewTab.cascade_view(world)
            elif i == '6':
                BViewTabs.see_grid(world, TypeView.HARD)
            elif i == '7':
                break
            
    @staticmethod
    def cascade_view(world:World) -> None:
        n = Inputs.get_int()
        Viewer.cascade_view(world, n)
        StandardMessages.enter_await()

class Main:
    def __init__(self):
        self.world = WorldPresets.normal()
    
    def run(self) -> None:
        while True:
            console.print(Messages.main_menu(InterfaceFactory.create_world_interface(self.world)))

            i = Inputs.question([str(i) for i in range(1, 15)])

            if i == '1':
                BTabs.create_random_creature(self.world)
            elif i == '2':
                Tabs.create_complete_creature(self.world)
            elif i == '3':
                BTabs.kill_creature(self.world)
            elif i == '4':
                Tabs.turn_pregnant(self.world)
            elif i == '5':
                Tabs.turn_gender(self.world)
            elif i == '6':
                Tabs.move_creature(self.world)
            elif i == '7':
                ViewTab.main(self.world)
            elif i == '8':
                BasicAPI.run_n_time(self.world, 1)
            elif i == '9':
                BasicAPI.run_n_time(self.world, 5)
            elif i == '10':
                BasicAPI.run_n_time(self.world, 10)
            elif i == '11':
                n = Inputs.get_int()
                BasicAPI.run_n_time(self.world, n)
            elif i == '12':
                Tabs.get_all_log(self.world)
            elif i == '13':
                world = BTabs.recreate_with_preset()
                if world is not None:
                    self.world = world
                    console.print('Reseting...')
            elif i == '14':
                break
    