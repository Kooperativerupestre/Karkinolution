from random import choice
from time import sleep

from rich.box import SQUARE, SQUARE_DOUBLE_HEAD
from rich.columns import Columns
from rich.live import Live
from rich.panel import Panel
from rich.table import Table

from karkinolution.core.coord import Coord

from karkinolution.organism.creatures import (
    Corpse,
    Creature,
    EntitiesRegistry,
)
from karkinolution.organism.genetics import CreatureTypes
from karkinolution.organism.identity import EntityTypes

from karkinolution.terrain.cell import TerrainTypes
from karkinolution.terrain.map import (
    EntityMap,
    Territory,
)
from karkinolution.terrain.world import (
    Log,
    LogEntry,
    World,
)

from karkinolution.ui.api.api import BasicAPI
from karkinolution.ui.global_console import console
from karkinolution.ui.interfaces import (
    CellInterface,
    CorpseInterface,
    CreatureInterface,
    InterfaceFactory,
    WorldInterface,
)
from karkinolution.ui.utils.cli import StandardMessages

def get_emoji_creature(specie:CreatureTypes) -> str:
    if specie == CreatureTypes.CRAB:
        return '🦀'
    elif specie == CreatureTypes.HIPPOPOTAMUS:
        return '🦛'
    elif specie == CreatureTypes.CROCODILE:
        return '🐊'
    elif specie == CreatureTypes.FISH:
        return '🐟'
    else:
        return 'A$'

def get_emoji_corpse() -> str:
    return '☠️'
def get_emoji_cell(cell_type:TerrainTypes) -> str:
    if cell_type == TerrainTypes.DIRT:
        return choice(['🌱', '🌿'])
    elif cell_type == TerrainTypes.ROCK:
        return '🪨'
    elif cell_type == TerrainTypes.SAND:
        return '🟨'
    elif cell_type == TerrainTypes.WATER:
        return choice(['🌊'])
    else:
        return 'C$'
def get_emoji_help() -> str:
    return """
🦀 -> Creature, Crab
🦛 -> Creature, Hippopotamus
🐊 -> Creature, Crocodile
🐟 -> Creature, Fish

☠️ -> Corpse

🌱, 🌿 -> Solo, Grass
🪨 -> Solo, Rock
🟨 -> Solo, Sand
🌊 -> Solo, Water
"""

class Renders:
    @staticmethod
    def render_map_grid(territory:Territory, entity_map:EntityMap, entities:EntitiesRegistry) -> Table:
        table = Table(show_header=True, box=SQUARE_DOUBLE_HEAD, show_lines=True, padding=(0, 0), header_style="bold magenta")
        
        table.add_column("Y/X", justify="center")
        for x in range(territory.size_x):
            table.add_column(str(x), justify="center")

        for y in range(territory.size_y):
            row:list[str] = [f"[bold cyan]{y}[/]"]
            for x in range(territory.size_x):
                coord = Coord(x, y)
                if entity_map.exists(coord):
                    creature_type = entity_map.get(coord).e_type
                    if creature_type == EntityTypes.CREATURE:
                        row.append(get_emoji_creature(entities.get_creature(entity_map.get(coord)).genome.core.id))
                    else:
                        row.append(get_emoji_corpse())
                else:
                    row.append(get_emoji_cell(territory.get(coord).type)) 
            table.add_row(*row)
        return table
    @staticmethod
    def render_stats_panel(world:WorldInterface) -> Panel:
        stats_table = Table.grid(padding=(0, 2))
        stats_table.add_column(style="bold white")
        stats_table.add_column()

        stats_table.add_row("Entities", f"[yellow]{world.entities}[/]")
        stats_table.add_row("Creatures", f"[yellow]{world.creatures_percent}%[/]")
        stats_table.add_row("Corpses", f"[yellow]{world.corpses_percent}%[/]")
        stats_table.add_row("World time", f"[magenta]{world.time}[/]")

        return Panel(
            stats_table,
            title="World Stats",
            border_style="green",
            expand=False,
            height=11,
            box=SQUARE
        )


    @staticmethod
    def render_full_creature(creature: CreatureInterface) -> Panel:
        table_left = Table(show_header=False, box=None)
        table_left.add_column(style="dim")
        table_left.add_column()

        table_left.add_row("ID", creature.id)
        table_left.add_row("Gender", creature.gender)
        table_left.add_row("Species", creature.specie.name.lower().capitalize())
        table_left.add_row("Position", f"({creature.position.x}, {creature.position.y})")

        table_right = Table(show_header=False, box=None)
        table_right.add_column(style="dim")
        table_right.add_column()

        table_right.add_row("Energy", f"{creature.energy.value}/{creature.energy.limit}")
        table_right.add_row("Life", f"{creature.life.value}/{creature.life.limit}")
        table_right.add_row("Age", f"{creature.age.value}/{creature.age.limit}")
        table_right.add_row("Hungry", f"{creature.hungry:.2f}")
        table_right.add_row("Pregnant", "Yes" if creature.pregnant else "No")

        if creature.gestation is not None:
            table_right.add_row("Gestation", f"{creature.gestation.value}/{creature.gestation.limit}")

        return Panel(Columns([table_left, table_right]), title=f"{creature.name}")
    @staticmethod
    def render_simply_creature(creature: CreatureInterface) -> Panel:
        table = Table(show_header=False, box=None)
        table.add_column(style="dim")
        table.add_column()

        table.add_row("ID", creature.id)
        table.add_row("Species", creature.specie.name.lower().capitalize())
        table.add_row("Hungry", f"{creature.hungry:.1f}%")
        table.add_row("Life", str(creature.life.value))
        table.add_row("Age", str(creature.age.value))
        table.add_row("Pregnant", "Yes" if creature.pregnant else "No")

        return Panel(table, title=creature.name)
    @staticmethod
    def render_corpse(corpse: CorpseInterface) -> Panel:
        table = Table(show_header=False, box=None)
        table.add_column(style="dim")
        table.add_column()

        table.add_row("Meat", f"{corpse.meat.value}/{corpse.meat.limit}")
        table.add_row("Time Left", str(corpse.time_left))

        return Panel(table, title="☠️ Corpse")
    @staticmethod
    def render_cell(cell: CellInterface) -> Panel:
        table = Table(show_header=False, box=None)
        table.add_column(style="dim")
        table.add_column()

        table.add_row("Terrain", cell.solo_type.name.lower().capitalize())
        table.add_row("Food", f'{cell.food.value}/{cell.food.limit}' if cell.food is not None else "—")
        table.add_row("Damage", str(cell.damage) if cell.damage is not None else "—")
        table.add_row("Movement Cost", str(cell.movement_cost) if cell.movement_cost is not None else "—")

        return Panel(table, title=f"{get_emoji_cell(cell.solo_type)} Cell")

    @staticmethod
    def render_log_panel(log_instance: Log) -> Panel:
        table = Table(show_header=True, header_style="bold cyan", box=None)
        table.add_column("Time", style="dim yellow", width=12)
        table.add_column("Message", style="green")
        
        for entry in log_instance.log_registry:
            table.add_row(str(entry.time), entry.message)
            
        return Panel(
            table,
            title="[bold white]System Log Registry[/bold white]",
            border_style="blue",
            expand=False
        )
    @staticmethod
    def render_filtered_log_panel(log_instance: Log, target_time: int) -> Panel:
        table = Table(show_header=True, header_style="bold cyan", box=None)
        table.add_column("Timestamp", style="dim yellow", width=12)
        table.add_column("Message", style="green")

        filtered_entries:list[LogEntry] = []
        
        filtered_entries.extend(log_instance.get_by_time(target_time-1))
        for entry in filtered_entries:
            table.add_row(str(entry.time), entry.message)
            
        return Panel(
            table,
            title=f"[bold white]Log Registry for Timestamp: {target_time}[/bold white]",
            border_style="magenta",
            expand=False
        )
class RenderGroups:
    @staticmethod
    def render_map_and_states(world:World) -> Table:
        container = Table.grid(expand=False)
        container.add_column(ratio=2)
        container.add_column(ratio=1)
        container.add_row(Renders.render_map_grid(world.territory, world.entity_map, world.entities),
                           Renders.render_stats_panel(InterfaceFactory.create_world_interface(world)))
        return container
    @staticmethod
    def render_map_log_stats(world: World) -> Table:
        container = Table.grid(expand=False)
        container.add_column()
        container.add_row(RenderGroups.render_map_and_states(world))
        container.add_row(Renders.render_filtered_log_panel(world.log, world.time))
        return container
    
    @staticmethod
    def render_all_entities(world:World) -> Columns:
        renderables:list[Panel] = []
        for e in world.entities.entities:
            if e.id.e_type == EntityTypes.CREATURE:
                assert isinstance(e, Creature)
                renderables.append(Renders.render_simply_creature(InterfaceFactory.create_creature_interface(e)))
            else: # CORPSE
                assert isinstance(e, Corpse)
                renderables.append(Renders.render_corpse(InterfaceFactory.create_corpse_interface(e)))
        columns = Columns(renderables, equal=False, expand=False, padding=(1, 2))
        return columns
    
class Viewer:
    @staticmethod
    def easy_live_view(world:World, time:int = 10) -> None:
        with Live(refresh_per_second=4) as live:
            for _ in range(0, time):
                BasicAPI.run_n_time(world,n=1)
                display = RenderGroups.render_map_and_states(world)

                live.update(display)
                sleep(1.1)

    @staticmethod
    def hard_live_view(world: World, time: int = 10) -> None:
        with Live(refresh_per_second=4) as live:
            for _ in range(time):
                BasicAPI.run_n_time(world, n=1)
                live.update(RenderGroups.render_map_log_stats(world), refresh=True)
    @staticmethod
    def cascade_view(world:World, n:int = 10) -> None:
        for _ in range(0, n):
            BasicAPI.run_n_time(world, 1)
            console.print(RenderGroups.render_map_log_stats(world))
            StandardMessages.pr(n=3)
            