from rich.table import Table
from rich.panel import Panel
from rich.live import Live
from rich.columns import Columns
from map.map import Territory, EntityMap
from organism.creatures import EntitiesRegistry
from map.cell import TerrainTypes
from organism.genetics import CreatureTypes
from random import choice
from core.coord import Coord
from ui.interfaces import FullCreatureInterface, SimplyCreatureInterface, CorpseInterface, CellInterface, WorldInterface, InterfaceFactory
from ui.api.api_world_interface import APIWorld
from map.world import World
from time import sleep
from organism.identity import EntityTypes
from rich.box import SQUARE_DOUBLE_HEAD

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
        height=11
    )


def render_map_and_states(map_table: Table, stats_panel: Panel) -> Table:
    container = Table.grid(expand=False)
    container.add_column(ratio=2)
    container.add_column(ratio=1)
    container.add_row(map_table, stats_panel)
    return container

def render_full_creature(creature: FullCreatureInterface) -> Panel:
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

def render_simply_creature(creature: SimplyCreatureInterface) -> Panel:
    table = Table(show_header=False, box=None)
    table.add_column(style="dim")
    table.add_column()

    table.add_row("ID", creature.id)
    table.add_row("Species", creature.specie.name.lower().capitalize())
    table.add_row("Hungry", f"{creature.hungry:.1f}%")
    table.add_row("Life", str(creature.life))
    table.add_row("Age", str(creature.age))
    table.add_row("Pregnant", "Yes" if creature.pregnant else "No")

    return Panel(table, title=creature.name)

def render_corpse(corpse: CorpseInterface) -> Panel:
    table = Table(show_header=False, box=None)
    table.add_column(style="dim")
    table.add_column()

    table.add_row("Meat", f"{corpse.meat.value}/{corpse.meat.limit}")
    table.add_row("Time Left", str(corpse.time_left))

    return Panel(table, title="☠️ Corpse")

def render_cell(cell: CellInterface) -> Panel:
    table = Table(show_header=False, box=None)
    table.add_column(style="dim")
    table.add_column()

    table.add_row("Terrain", cell.solo_type.name.lower().capitalize())
    table.add_row("Food", str(cell.food) if cell.food is not None else "—")
    table.add_row("Damage", str(cell.damage) if cell.damage is not None else "—")
    table.add_row("Movement Cost", str(cell.movement_cost) if cell.movement_cost is not None else "—")

    return Panel(table, title=f"{get_emoji_cell(cell.solo_type)} Cell")

def render_live(world:World, time:int = 10) -> None:
    with Live(refresh_per_second=4) as live:
        for _ in range(0, time):
            grid = render_map_grid(world.territory, world.entity_map, world.entities)
            stats = render_stats_panel(InterfaceFactory.create_world_interface(world))
            display = render_map_and_states(grid, stats)

            live.update(display)
            APIWorld.run_1_time(world)
            sleep(1.1)

    