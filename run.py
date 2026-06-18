"""
🧬 Karkino Simulation Debugger
A comprehensive GUI for simulating and debugging the Karkino ecosystem.

Modular architecture with theme support, real-time visualization, and creature analytics.
"""

import sys
import time
from typing import Optional, Dict, List
from dataclasses import dataclass
from enum import Enum

from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QGridLayout,
    QTableWidget, QTableWidgetItem, QPushButton, QLabel, QSpinBox, QCheckBox,
    QComboBox, QFrame, QTextEdit, QHeaderView, QStatusBar
)
from PySide6.QtCore import Qt, QTimer, QSize
from PySide6.QtGui import QColor, QFont, QTextCursor

from map.world import World, WorldFactory, WorldMotor, PresetWorld
from map.map import ScaleGenValues, TerrainQuery
from core.coord import Coord
from organism.creatures import CreatureFactory, Creature, Corpse
from map.cell import TerrainTypes
from tick.worldcycle import Runner


class SimulationState(Enum):
    """Enumeration for simulation states."""
    STOPPED = "Stopped"
    RUNNING = "Running"
    PAUSED = "Paused"


@dataclass
class ThemeColors:
    """Theme color palette for the application."""
    primary: str = "#1e1e2e"
    secondary: str = "#2b2b3e"
    accent: str = "#00ff88"
    text_primary: str = "#e0e0e0"
    text_secondary: str = "#a0a0a0"
    creature: str = "#2a5a2a"
    corpse: str = "#5a2a2a"
    terrain_dirt: str = "#1a3a1a"
    terrain_sand: str = "#5a4a2a"
    terrain_water: str = "#2a3a5a"
    terrain_rock: str = "#3a3a3a"
    
    def get_stylesheet(self) -> str:
        """Generate application stylesheet."""
        return f"""
            QMainWindow, QWidget {{
                background-color: {self.primary};
                color: {self.text_primary};
            }}
            QFrame {{
                background-color: {self.secondary};
                border-radius: 5px;
            }}
            QPushButton {{
                background-color: {self.accent};
                color: {self.primary};
                border: none;
                border-radius: 4px;
                padding: 8px 14px;
                font-weight: bold;
                min-width: 100px;
                min-height: 35px;
            }}
            QPushButton:hover {{
                background-color: #00dd77;
            }}
            QPushButton:pressed {{
                background-color: #00cc66;
            }}
            QPushButton:disabled {{
                background-color: {self.text_secondary};
                color: {self.primary};
            }}
            QTableWidget {{
                background-color: {self.secondary};
                gridline-color: {self.primary};
                border: 1px solid {self.accent};
            }}
            QHeaderView::section {{
                background-color: {self.primary};
                color: {self.accent};
                padding: 4px;
                border: none;
                font-weight: bold;
            }}
            QTextEdit {{
                background-color: {self.primary};
                color: {self.text_primary};
                border: 1px solid {self.accent};
            }}
            QLabel {{
                color: {self.text_primary};
            }}
            QSpinBox, QComboBox {{
                background-color: {self.primary};
                color: {self.text_primary};
                border: 1px solid {self.accent};
            }}
            QCheckBox {{
                color: {self.text_primary};
            }}
            QCheckBox::indicator {{
                border: 1px solid {self.accent};
                width: 16px;
                height: 16px;
            }}
            QCheckBox::indicator:checked {{
                background-color: {self.accent};
            }}
            QStatusBar {{
                background-color: {self.primary};
                color: {self.accent};
            }}
        """


class MapRenderer:
    """Renders the world map with terrain and entities."""
    
    SPECIES_EMOJIS: Dict[str, str] = {
        "CRAB": "🦀", "FISH": "🐟", "CROCODILE": "🐊", "HIPPOPOTAMUS": "🦛",
    }
    
    TERRAIN_SYMBOLS: Dict[TerrainTypes, str] = {
        TerrainTypes.DIRT: "🌱", TerrainTypes.SAND: "🏜️",
        TerrainTypes.WATER: "💧", TerrainTypes.ROCK: "🪨",
    }
    
    @classmethod
    def get_terrain_symbol(cls, terrain_type: TerrainTypes) -> str:
        return cls.TERRAIN_SYMBOLS.get(terrain_type, "?")
    
    @classmethod
    def get_creature_emoji(cls, creature: Creature) -> str:
        try:
            species_name = str(creature.genome.core.id).split(".")[-1]
            return cls.SPECIES_EMOJIS.get(species_name, "🐾")
        except Exception:
            return "🐾"
    
    @classmethod
    def get_entity_symbol(cls, entity) -> str:
        return cls.get_creature_emoji(entity) if isinstance(entity, Creature) else "💀"
    
    @staticmethod
    def get_map_data(world: World) -> tuple[List[List[str]], int, int]:
        coords = list(world.territory.keys)
        size_y = max((c.y for c in coords), default=0) + 1
        size_x = max((c.x for c in coords), default=0) + 1
        
        grid = [[None for _ in range(size_x)] for _ in range(size_y)]
        
        for y in range(size_y):
            for x in range(size_x):
                coord = Coord(x, y)
                try:
                    cell = world.territory.get(coord)
                    entity_id = world.entity_map.get(coord) if hasattr(world, 'entity_map') else None
                    
                    if entity_id:
                        entity = world.entities.get(entity_id)
                        grid[y][x] = MapRenderer.get_entity_symbol(entity)
                    else:
                        grid[y][x] = MapRenderer.get_terrain_symbol(cell.type)
                except Exception:
                    grid[y][x] = "."
        
        return grid, size_x, size_y


class CreatureInfoFormatter:
    """Formats creature information for display."""
    
    @staticmethod
    def get_intent_name(creature: Creature) -> str:
        try:
            return creature.intent.intent.name
        except Exception:
            return str(creature.intent)
    
    @staticmethod
    def get_species_name(creature: Creature) -> str:
        try:
            return str(creature.genome.core.id).split(".")[-1]
        except Exception:
            return "Unknown"
    
    @staticmethod
    def format_creature_details(creature: Creature) -> str:
        intent_name = CreatureInfoFormatter.get_intent_name(creature)
        species_name = CreatureInfoFormatter.get_species_name(creature)
        
        return f"""
╔══════════════════════════════════════╗
║  📋 Creature: {creature.name:<24}║
╚══════════════════════════════════════╝

📊 STATISTICS:
  Energy:   {creature.energy.value:>8.2f} / {creature.energy.limit:<8.2f}
  Life:     {creature.life.value:>8.2f} / {creature.life.limit:<8.2f}
  Age:      {creature.age.value:>8} / {creature.age.limit:<8}
  Hunger:   {creature.hungry:>8.4f}
  Pregnant: {'✓ Yes' if creature.pregnant else '✗ No':<8}

🧬 GENETICS:
  Species:      {species_name}
  Strength:     {creature.genome.body.strength}
  Life Limit:   {creature.genome.body.life_limit}

📍 LOCATION: ({creature.position.x}, {creature.position.y})

🎯 CURRENT ACTION: {intent_name}
        """.strip()


class WorldStatsCalculator:
    """Calculates and caches world statistics."""
    
    @staticmethod
    def get_stats(world: World) -> Dict:
        creatures = [e for e in world.entities.entitys.values() if isinstance(e, Creature)]
        corpses = [e for e in world.entities.entitys.values() if isinstance(e, Corpse)]
        
        total_energy = sum(c.energy.value for c in creatures)
        avg_life = sum(c.life.value for c in creatures) / len(creatures) if creatures else 0
        avg_age = sum(c.age.value for c in creatures) / len(creatures) if creatures else 0
        
        return {
            "time": world.time,
            "creatures_count": len(creatures),
            "corpses_count": len(corpses),
            "total_energy": total_energy,
            "avg_life": avg_life,
            "avg_age": avg_age,
            "creatures": creatures
        }


class ControlPanel(QFrame):
    """Panel with simulation controls."""
    
    def __init__(self):
        super().__init__()
        self.setup_ui()
    
    def setup_ui(self):
        layout = QVBoxLayout(self)
        
        self.btn_start = QPushButton("▶️  START")
        self.btn_pause = QPushButton("⏸️  PAUSE")
        self.btn_pause.setEnabled(False)
        self.btn_step = QPushButton("⏭️  STEP")
        self.btn_reset = QPushButton("🔄 RESET")
        
        control_layout = QGridLayout()
        control_layout.addWidget(self.btn_start, 0, 0)
        control_layout.addWidget(self.btn_pause, 0, 1)
        control_layout.addWidget(self.btn_step, 1, 0)
        control_layout.addWidget(self.btn_reset, 1, 1)
        
        layout.addLayout(control_layout)
        layout.addSpacing(15)
        layout.addWidget(QLabel("⚙️  SETTINGS"))
        
        self.auto_step_check = QCheckBox("🔄 Auto-Step")
        layout.addWidget(self.auto_step_check)
        
        interval_layout = QHBoxLayout()
        interval_layout.addWidget(QLabel("Interval (s):"))
        self.interval_spin = QSpinBox()
        self.interval_spin.setMinimum(1)
        self.interval_spin.setMaximum(10)
        self.interval_spin.setValue(2)
        interval_layout.addWidget(self.interval_spin)
        layout.addLayout(interval_layout)
        
        layout.addSpacing(15)
        layout.addWidget(QLabel("👁️  VIEW MODE"))
        
        self.view_combo = QComboBox()
        self.view_combo.addItems(["Summary", "Detailed", "Genome Analysis"])
        layout.addWidget(self.view_combo)
        
        layout.addStretch()


class StatsPanel(QFrame):
    """Panel displaying world statistics."""
    
    def __init__(self, theme: ThemeColors):
        super().__init__()
        self.theme = theme
        self.setup_ui()
    
    def setup_ui(self):
        layout = QVBoxLayout(self)
        
        self.time_label = self._create_stat_label("⏱️  Time", "0")
        self.creatures_label = self._create_stat_label("🦁 Creatures", "0")
        self.corpses_label = self._create_stat_label("💀 Corpses", "0")
        self.energy_label = self._create_stat_label("⚡ Total Energy", "0.0")
        self.life_label = self._create_stat_label("❤️  Avg Life", "0.0")
        self.age_label = self._create_stat_label("📅 Avg Age", "0")
        self.state_label = self._create_stat_label("🎮 State", "STOPPED")
        
        layout.addWidget(self.time_label)
        layout.addWidget(self.creatures_label)
        layout.addWidget(self.corpses_label)
        layout.addWidget(self.energy_label)
        layout.addWidget(self.life_label)
        layout.addWidget(self.age_label)
        layout.addWidget(self.state_label)
        layout.addStretch()
    
    def _create_stat_label(self, title: str, value: str) -> QLabel:
        label = QLabel(f"{title}: {value}")
        label.setFont(QFont("Courier", 11))
        label.setStyleSheet(f"color: {self.theme.accent}; font-weight: bold;")
        return label
    
    def update_stats(self, stats: Dict):
        self.time_label.setText(f"⏱️  Time: {stats['time']}")
        self.creatures_label.setText(f"🦁 Creatures: {stats['creatures_count']}")
        self.corpses_label.setText(f"💀 Corpses: {stats['corpses_count']}")
        self.energy_label.setText(f"⚡ Total Energy: {stats['total_energy']:.1f}")
        self.life_label.setText(f"❤️  Avg Life: {stats['avg_life']:.1f}")
        self.age_label.setText(f"📅 Avg Age: {stats['avg_age']:.0f}")


class MapPanel(QFrame):
    """Panel displaying the world map."""
    
    def __init__(self, theme: ThemeColors):
        super().__init__()
        self.theme = theme
        self.setup_ui()
    
    def setup_ui(self):
        layout = QVBoxLayout(self)
        layout.addWidget(QLabel("🗺️  WORLD MAP"))
        
        self.map_table = QTableWidget()
        self.map_table.setMaximumHeight(350)
        layout.addWidget(self.map_table)
    
    def update_map(self, world: World):
        try:
            grid, size_x, size_y = MapRenderer.get_map_data(world)
            
            self.map_table.setRowCount(size_y)
            self.map_table.setColumnCount(size_x)
            self.map_table.horizontalHeader().setVisible(False)
            self.map_table.verticalHeader().setVisible(False)
            
            for row in range(size_y):
                self.map_table.setRowHeight(row, 28)
            for col in range(size_x):
                self.map_table.setColumnWidth(col, 28)
            
            for y in range(size_y):
                for x in range(size_x):
                    symbol = grid[y][x]
                    item = QTableWidgetItem(symbol)
                    item.setFont(QFont("Arial", 16))
                    item.setTextAlignment(Qt.AlignCenter)
                    item.setBackground(QColor(self._get_cell_color(symbol)))
                    self.map_table.setItem(y, x, item)
        except Exception as e:
            print(f"Error updating map: {e}")
    
    def _get_cell_color(self, symbol: str) -> str:
        color_map = {
            "🦀": self.theme.creature, "🐟": self.theme.creature,
            "🐊": self.theme.creature, "🦛": self.theme.creature,
            "🐾": self.theme.creature, "💀": self.theme.corpse,
            "🌱": self.theme.terrain_dirt, "🏜️": self.theme.terrain_sand,
            "💧": self.theme.terrain_water, "🪨": self.theme.terrain_rock,
        }
        return color_map.get(symbol, self.theme.primary)


class CreaturesTable(QFrame):
    """Panel displaying creatures in a table."""
    
    def __init__(self, theme: ThemeColors):
        super().__init__()
        self.theme = theme
        self.selected_creature: Optional[Creature] = None
        self.setup_ui()
    
    def setup_ui(self):
        layout = QVBoxLayout(self)
        layout.addWidget(QLabel("🦁 CREATURES"))
        
        self.table = QTableWidget()
        self.table.setColumnCount(9)
        self.table.setHorizontalHeaderLabels([
            "Name", "Species", "Intent", "Energy", "Life", "Age", "Hungry", "Pregnant", "Pos"
        ])
        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table.itemClicked.connect(self._on_creature_selected)
        
        layout.addWidget(self.table, 1)
    
    def update_creatures(self, creatures: List[Creature]):
        self.table.setRowCount(len(creatures))
        
        for row, creature in enumerate(creatures):
            species_name = CreatureInfoFormatter.get_species_name(creature)
            intent_name = CreatureInfoFormatter.get_intent_name(creature)
            
            items = [
                creature.name, species_name, intent_name,
                f"{creature.energy.value:.1f}/{creature.energy.limit:.1f}",
                f"{creature.life.value:.1f}", f"{creature.age.value}",
                f"{creature.hungry:.2f}", "✓" if creature.pregnant else "✗",
                f"({creature.position.x},{creature.position.y})"
            ]
            
            for col, text in enumerate(items):
                item = QTableWidgetItem(str(text))
                item.setData(Qt.UserRole, creature)
                self.table.setItem(row, col, item)
    
    def _on_creature_selected(self, item: QTableWidgetItem):
        creature = item.data(Qt.UserRole)
        if isinstance(creature, Creature):
            self.selected_creature = creature
    
    def get_selected_creature(self) -> Optional[Creature]:
        return self.selected_creature


class DebugConsole(QFrame):
    """Debug output console."""
    
    def __init__(self, theme: ThemeColors):
        super().__init__()
        self.theme = theme
        self.setup_ui()
    
    def setup_ui(self):
        layout = QVBoxLayout(self)
        layout.addWidget(QLabel("🐛 DEBUG CONSOLE"))
        
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        self.text_edit.setFont(QFont("Courier", 9))
        self.text_edit.setMaximumHeight(200)
        
        layout.addWidget(self.text_edit)
    
    def log(self, message: str):
        timestamp = time.strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] {message}"
        self.text_edit.append(log_entry)
        self.text_edit.moveCursor(QTextCursor.End)
    
    def clear(self):
        self.text_edit.clear()


class CreatureDetailsPanel(QFrame):
    """Panel showing details of selected creature."""
    
    def __init__(self, theme: ThemeColors):
        super().__init__()
        self.theme = theme
        self.setup_ui()
    
    def setup_ui(self):
        layout = QVBoxLayout(self)
        layout.addWidget(QLabel("📋 CREATURE DETAILS"))
        
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        self.text_edit.setFont(QFont("Courier", 9))
        self.text_edit.setMaximumHeight(250)
        
        layout.addWidget(self.text_edit)
    
    def update_creature(self, creature: Optional[Creature]):
        if creature:
            self.text_edit.setText(CreatureInfoFormatter.format_creature_details(creature))
        else:
            self.text_edit.setText("Select a creature to view details")


class SimulationDebugger(QMainWindow):
    """Main GUI for Karkino simulation debugging."""
    
    def __init__(self):
        super().__init__()
        self.theme = ThemeColors()
        self.world: Optional[World] = None
        self.auto_step = False
        self.step_interval = 2.0
        self.state = SimulationState.STOPPED
        self.last_step_time = time.time()
        
        self.setWindowTitle("🧬 Karkino Simulation Debugger v2.0")
        self.setGeometry(50, 50, 1800, 1000)
        self.setStyleSheet(self.theme.get_stylesheet())
        
        self._setup_ui()
        self._init_world()
        self._setup_timer()
        self._setup_status_bar()
    
    def _setup_ui(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        main_layout = QHBoxLayout(central_widget)
        main_layout.setSpacing(10)
        main_layout.setContentsMargins(10, 10, 10, 10)
        
        left_panel = self._create_left_panel()
        middle_panel = self._create_middle_panel()
        right_panel = self._create_right_panel()
        
        main_layout.addWidget(left_panel, 2)
        main_layout.addWidget(middle_panel, 2)
        main_layout.addWidget(right_panel, 3)
    
    def _create_left_panel(self) -> QFrame:
        frame = QFrame()
        layout = QVBoxLayout(frame)
        
        self.map_panel = MapPanel(self.theme)
        layout.addWidget(self.map_panel, 2)
        
        self.control_panel = ControlPanel()
        self.control_panel.btn_start.clicked.connect(self._start_simulation)
        self.control_panel.btn_pause.clicked.connect(self._pause_simulation)
        self.control_panel.btn_step.clicked.connect(self._step_simulation)
        self.control_panel.btn_reset.clicked.connect(self._reset_simulation)
        self.control_panel.auto_step_check.stateChanged.connect(self._toggle_auto_step)
        self.control_panel.interval_spin.valueChanged.connect(self._update_interval)
        
        layout.addWidget(self.control_panel, 1)
        return frame
    
    def _create_middle_panel(self) -> QFrame:
        frame = QFrame()
        layout = QVBoxLayout(frame)
        
        self.stats_panel = StatsPanel(self.theme)
        layout.addWidget(self.stats_panel, 1)
        
        self.debug_console = DebugConsole(self.theme)
        layout.addWidget(self.debug_console, 1)
        
        return frame
    
    def _create_right_panel(self) -> QFrame:
        frame = QFrame()
        layout = QVBoxLayout(frame)
        
        self.creatures_table = CreaturesTable(self.theme)
        self.creatures_table.table.itemClicked.connect(self._on_creature_selected)
        layout.addWidget(self.creatures_table, 2)
        
        self.details_panel = CreatureDetailsPanel(self.theme)
        layout.addWidget(self.details_panel, 1)
        
        return frame
    
    def _setup_status_bar(self):
        self.statusBar().showMessage("🟢 Ready")
    
    def _init_world(self):
        self._log("Initializing world...")
        try:
            self.world = WorldFactory.create_world(
                PresetWorld(42, Coord(15, 15), ScaleGenValues.MEDIUM)
            )
            
            for i in range(8):
                try:
                    free_coords = TerrainQuery.random_free_coord(
                        self.world.territory, self.world.entity_map, 1
                    )
                    creature = CreatureFactory.gen_creature(position=free_coords[0])
                    WorldMotor.add_entity(
                        self.world.territory, self.world.entity_map, 
                        creature, self.world.entities
                    )
                    self._log(f"✓ Created creature #{i+1}: {creature.name}")
                except Exception as e:
                    self._log(f"✗ Error creating creature: {str(e)[:40]}")
            
            self._log("✓ World initialized successfully!")
            self._update_display()
        except Exception as e:
            self._log(f"✗ ERROR initializing world: {str(e)[:40]}")
    
    def _setup_timer(self):
        self.timer = QTimer()
        self.timer.timeout.connect(self._tick)
        self.timer.start(500)
    
    def _tick(self):
        try:
            if self.state == SimulationState.RUNNING and self.world is not None:
                current_time = time.time()
                if self.auto_step and (current_time - self.last_step_time >= self.step_interval):
                    Runner.run(self.world)
                    self.last_step_time = current_time
                    self._log(f"✓ Auto-step (T={self.world.time})")
            
            self._update_display()
        except Exception as e:
            self._log(f"✗ ERROR in tick: {str(e)[:40]}")
    
    def _update_display(self):
        if self.world is None:
            return
        
        try:
            stats = WorldStatsCalculator.get_stats(self.world)
            self.stats_panel.update_stats(stats)
            self.map_panel.update_map(self.world)
            self.creatures_table.update_creatures(stats["creatures"])
            self.statusBar().showMessage(
                f"⏱️ T:{self.world.time} | 🦁:{stats['creatures_count']} | "
                f"⚡:{stats['total_energy']:.0f} | 🎮:{self.state.value}"
            )
        except Exception as e:
            self._log(f"✗ ERROR in update_display: {str(e)[:40]}")
    
    def _on_creature_selected(self, item: QTableWidgetItem):
        creature = self.creatures_table.get_selected_creature()
        self.details_panel.update_creature(creature)
    
    def _start_simulation(self):
        self.state = SimulationState.RUNNING
        self.control_panel.btn_start.setEnabled(False)
        self.control_panel.btn_pause.setEnabled(True)
        self.auto_step = self.control_panel.auto_step_check.isChecked()
        self.last_step_time = time.time()
        self._log("▶️ SIMULATION STARTED")
    
    def _pause_simulation(self):
        self.state = SimulationState.PAUSED
        self.control_panel.btn_start.setEnabled(True)
        self.control_panel.btn_pause.setEnabled(False)
        self._log("⏸️ SIMULATION PAUSED")
    
    def _step_simulation(self):
        try:
            if self.world is not None:
                Runner.run(self.world)
                self._log(f"⏭️ Step executed (T={self.world.time})")
                self._update_display()
            else:
                self._log("✗ ERROR: World is None!")
        except Exception as e:
            self._log(f"✗ ERROR in step: {str(e)[:40]}")
    
    def _reset_simulation(self):
        self.state = SimulationState.STOPPED
        self.control_panel.btn_start.setEnabled(True)
        self.control_panel.btn_pause.setEnabled(False)
        self.auto_step = False
        self.control_panel.auto_step_check.setChecked(False)
        self.debug_console.clear()
        self.details_panel.update_creature(None)
        self._init_world()
        self._log("🔄 SIMULATION RESET")
    
    def _toggle_auto_step(self, state: int):
        self.auto_step = state == Qt.Checked
        if self.state == SimulationState.RUNNING:
            self._log(f"🔄 Auto-step {'ENABLED' if self.auto_step else 'DISABLED'}")
    
    def _update_interval(self, value: int):
        self.step_interval = value
        self._log(f"⏱️ Interval set to {value}s")
    
    def _log(self, message: str):
        self.debug_console.log(message)


def main():
    """Main entry point."""
    app = QApplication(sys.argv)
    debugger = SimulationDebugger()
    debugger.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
