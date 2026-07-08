from enum import Enum, auto

from karkinolution.terrain.world import World

from karkinolution.ui.api.api import (
    ApiInterface,
    BasicAPI,
    Outputs,
)
from karkinolution.ui.global_console import console
from karkinolution.ui.utils.cli import (
    Inputs,
    StandardMessages,
    stand_by,
)
from karkinolution.ui.utils.visualizations import (
    RenderGroups,
    Renders,
    Viewer,
)


class TypeView(Enum):
    HARD = auto()
    EASY = auto()
class BTabs:
    @staticmethod
    def kill_creature(world:World) -> None:
        id = Inputs.get_id(world)
        if id is None:
            return None

        r = BasicAPI.kill_creature(id, world)

        if r != Outputs.OK:
            StandardMessages.cannot_execute('Kill creature {}...'.format(id))
            return None
        StandardMessages.killed_creature(ApiInterface.get_creature_interface(id, world).name)
        stand_by()
    @staticmethod
    def create_random_creature(world:World) -> None:
        name = input('Name: ').strip().capitalize()
        
        r = BasicAPI.create_random_creature(name, world)

        if r != Outputs.OK:
            StandardMessages.cannot_execute('Create creature {}'.format(name))
        StandardMessages.created_creature(name)
        stand_by()
    @staticmethod
    def recreate_with_preset() -> World | None:
        StandardMessages.show_presets()
        console.print("Exit ❌ [5]")
        i = Inputs.question(['1', '2', '3', '4', '5'])

        if i == '5':
            return None

        r = BasicAPI.reset_with_a_preset(i)

        if not isinstance(r, World):
            return None
        else:
            return r
class BViewTabs:
    @staticmethod
    def see_creature(world:World) -> None:
        id = Inputs.get_id(world)
        if id is None:
            return None
        
        r = Renders.render_full_creature(ApiInterface.get_creature_interface(id, world))

        console.print(r)
        StandardMessages.enter_await()
    @staticmethod
    def see_cell(world:World) -> None:
        coord = Inputs.get_coord()

        cell_interface = ApiInterface.get_cell_interface(coord, world)

        if cell_interface is None:
            StandardMessages.cannot_execute('See cell')
            return None
        console.print(Renders.render_cell(cell_interface))
        StandardMessages.enter_await()
    @staticmethod
    def see_all_entities(world:World) -> None:
        console.print(RenderGroups.render_all_entities(world))
        StandardMessages.enter_await()
    @staticmethod
    def see_grid(world:World, mode:TypeView) -> None:
        if mode == TypeView.HARD:
            console.print(RenderGroups.render_map_log_stats(world))
            StandardMessages.enter_await()
        else:
            console.print(RenderGroups.render_map_and_states(world))
    @staticmethod
    def live_view(world:World, mode:TypeView) -> None:
        n = Inputs.get_int()
        if mode == TypeView.HARD:
            Viewer.hard_live_view(world, n)
        else:
            Viewer.easy_live_view(world, n)
        StandardMessages.passed_time(n)
        StandardMessages.enter_await()