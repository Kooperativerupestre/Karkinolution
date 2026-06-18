import sys
import time
from typing import Optional
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QGridLayout,
    QTableWidget, QTableWidgetItem, QPushButton, QLabel, QSpinBox, QCheckBox,
    QComboBox, QScrollArea, QFrame, QTextEdit, QTabWidget, QHeaderView
)
from PySide6.QtCore import Qt, QTimer, Signal, QObject, QThread, QSize
from PySide6.QtGui import QColor, QFont, QIcon, QTextCursor
from enum import Enum

from map.world import World, WorldFactory, WorldMotor, PresetWorld
from map.map import ScaleGenValues, TerrainQuery
from core.coord import Coord
from organism.creatures import CreatureFactory, Creature, Corpse
from map.cell import TerrainTypes
from tick.worldcycle import Runner


class SimulationState(Enum):
    STOPPED = "Stopped"
    RUNNING = "Running"
    PAUSED = "Paused"


class MapRenderer:
    """Helper class to render world map."""
    
    @staticmethod
    def get_terrain_symbol(terrain_type: TerrainTypes) -> str:
        """Get emoji symbol for terrain type."""
        terrain_map = {
            TerrainTypes.DIRT: "🌱",
            TerrainTypes.SAND: "🏜",
            TerrainTypes.WATER: "💧",
            TerrainTypes.ROCK: "🪨",
        }
        return terrain_map.get(terrain_type, "?")
    
    @staticmethod
    def get_entity_symbol(entity) -> str:
        """Get symbol for entity."""
        if isinstance(entity, Creature):
            return "C"
        else:
            return "X"
    
    @staticmethod
    def get_map_data(world: World) -> tuple[list[list[str]], int, int]:
        """Generate map data as 2D grid."""
        coords = list(world.territory.keys)
        size_y = max((c.y for c in coords), default=0) + 1
        size_x = max((c.x for c in coords), default=0) + 1
        
        grid = [[None for _ in range(size_x)] for _ in range(size_y)]
        
        for y in range(size_y):
            for x in range(size_x):
                coord = Coord(x, y)
                try:
                    cell = world.territory.get(coord)
                    entity_id = None
                    try:
                        entity_id = world.entity_map.get(coord)
                    except:
                        pass
                    
                    if entity_id:
                        entity = world.entities.get(entity_id)
                        grid[y][x] = MapRenderer.get_entity_symbol(entity)
                    else:
                        grid[y][x] = MapRenderer.get_terrain_symbol(cell.type)
                except:
                    grid[y][x] = "."
        
        return grid, size_x, size_y


class CreatureInfoFormatter:
    """Helper class to format creature information."""
    
    @staticmethod
    def get_intent_name(creature: Creature) -> str:
        """Get readable intent name."""
        try:
            return creature.intent.intent.name
        except:
            return str(creature.intent)
    
    @staticmethod
    def get_species_name(creature: Creature) -> str:
        """Get readable species name."""
        try:
            return str(creature.genome.core.id).split(".")[-1]
        except:
            return "Unknown"


class SimulationWorker(QObject):
    """Worker for simulation updates in background."""
    update_signal = Signal()
    
    def __init__(self, world: World):
        super().__init__()
        self.world = world
        self.auto_step = False
        self.step_interval = 2.0
        self.last_step = time.time()
    
    def run_step(self):
        """Execute a single simulation step."""
        if self.auto_step:
            current_time = time.time()
            if current_time - self.last_step >= self.step_interval:
                self.world.time += 1
                self.last_step = current_time
        self.update_signal.emit()


class SimulationDebugger(QMainWindow):
    """PySide6 GUI for Karkino simulation debugging."""
    
    def __init__(self):
        super().__init__()
        self.world: Optional[World] = None
        self.auto_step = False
        self.step_interval = 2.0
        self.state = SimulationState.STOPPED
        self.last_step_time = time.time()
        
        self.setWindowTitle("🧬 Karkino Simulation Debugger")
        self.setGeometry(100, 100, 1600, 900)
        
        self.setup_ui()
        self.init_world()
        self.setup_timer()
        
    def setup_ui(self):
        """Setup the UI components."""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        main_layout = QHBoxLayout(central_widget)
        
        # Left panel: Map and stats
        left_panel = self.create_left_panel()
        
        # Right panel: Controls and creatures table
        right_panel = self.create_right_panel()
        
        main_layout.addWidget(left_panel, 2)
        main_layout.addWidget(right_panel, 3)
        
    def create_left_panel(self) -> QFrame:
        """Create the left panel with map and stats."""
        frame = QFrame()
        layout = QVBoxLayout(frame)
        
        # Map display as table
        self.map_table = QTableWidget()
        self.map_table.setMaximumHeight(400)
        self.map_table.setStyleSheet("QTableWidget { border: 1px solid #444; }")
        layout.addWidget(QLabel("🗺️ World Map"), 0)
        layout.addWidget(self.map_table, 0)
        
        # Stats panel
        stats_frame = QFrame()
        stats_frame.setStyleSheet("background-color: #2b2b2b; border-radius: 5px; padding: 10px;")
        stats_layout = QGridLayout(stats_frame)
        
        self.time_label = QLabel("⏱️  Time: 0")
        self.creatures_label = QLabel("🦁 Creatures: 0")
        self.corpses_label = QLabel("💀 Corpses: 0")
        self.total_energy_label = QLabel("⚡ Total Energy: 0.0")
        self.avg_life_label = QLabel("❤️  Avg Life: 0.0")
        self.state_label = QLabel("State: STOPPED")
        
        for label in [self.time_label, self.creatures_label, self.corpses_label,
                      self.total_energy_label, self.avg_life_label, self.state_label]:
            label.setStyleSheet("color: #00ff00; font-weight: bold;")
            label.setFont(QFont("Arial", 10))
        
        stats_layout.addWidget(self.time_label, 0, 0)
        stats_layout.addWidget(self.creatures_label, 0, 1)
        stats_layout.addWidget(self.corpses_label, 1, 0)
        stats_layout.addWidget(self.total_energy_label, 1, 1)
        stats_layout.addWidget(self.avg_life_label, 2, 0)
        stats_layout.addWidget(self.state_label, 2, 1)
        
        layout.addWidget(QLabel("📊 World Statistics"), 0)
        layout.addWidget(stats_frame, 0)
        
        # Debug panel
        self.debug_text = QTextEdit()
        self.debug_text.setReadOnly(True)
        self.debug_text.setFont(QFont("Courier", 8))
        self.debug_text.setMaximumHeight(250)
        layout.addWidget(QLabel("🐛 Debug Log"), 0)
        layout.addWidget(self.debug_text, 1)
        
        frame.setMaximumWidth(800)
        return frame
    
    def create_right_panel(self) -> QFrame:
        """Create the right panel with controls and creatures table."""
        frame = QFrame()
        layout = QVBoxLayout(frame)
        
        # Control buttons
        control_layout = QHBoxLayout()
        
        self.btn_start = QPushButton("▶️ Start")
        self.btn_start.clicked.connect(self.start_simulation)
        
        self.btn_pause = QPushButton("⏸️ Pause")
        self.btn_pause.clicked.connect(self.pause_simulation)
        self.btn_pause.setEnabled(False)
        
        self.btn_step = QPushButton("⏭️ Step")
        self.btn_step.clicked.connect(self.step_simulation)
        
        self.btn_reset = QPushButton("🔄 Reset")
        self.btn_reset.clicked.connect(self.reset_simulation)
        
        control_layout.addWidget(self.btn_start)
        control_layout.addWidget(self.btn_pause)
        control_layout.addWidget(self.btn_step)
        control_layout.addWidget(self.btn_reset)
        
        layout.addLayout(control_layout)
        
        # Settings
        settings_layout = QHBoxLayout()
        
        self.auto_step_check = QCheckBox("🔄 Auto-Step")
        self.auto_step_check.stateChanged.connect(self.toggle_auto_step)
        
        settings_layout.addWidget(self.auto_step_check)
        
        settings_layout.addWidget(QLabel("Interval (s):"))
        self.interval_spin = QSpinBox()
        self.interval_spin.setMinimum(1)
        self.interval_spin.setMaximum(10)
        self.interval_spin.setValue(2)
        self.interval_spin.valueChanged.connect(self.update_interval)
        settings_layout.addWidget(self.interval_spin)
        
        settings_layout.addWidget(QLabel("View:"))
        self.view_combo = QComboBox()
        self.view_combo.addItems(["Summary", "Detailed", "Genome"])
        self.view_combo.currentTextChanged.connect(self.update_creatures_view)
        settings_layout.addWidget(self.view_combo)
        
        settings_layout.addStretch()
        
        layout.addLayout(settings_layout)
        
        # Creatures table
        self.creatures_table = QTableWidget()
        self.creatures_table.setColumnCount(9)
        self.creatures_table.setHorizontalHeaderLabels([
            "Name", "Species", "Intent", "Energy", "Life", "Age", "Hungry", "Pregnant", "Position"
        ])
        self.creatures_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.creatures_table.itemClicked.connect(self.on_creature_selected)
        
        layout.addWidget(QLabel("🦁 Creatures Table"), 0)
        layout.addWidget(self.creatures_table, 1)
        
        # Selected creature info
        info_layout = QHBoxLayout()
        self.selected_creature_text = QTextEdit()
        self.selected_creature_text.setReadOnly(True)
        self.selected_creature_text.setFont(QFont("Courier", 9))
        self.selected_creature_text.setMaximumHeight(200)
        info_layout.addWidget(self.selected_creature_text)
        
        layout.addLayout(info_layout)
        
        return frame
    
    def init_world(self):
        """Initialize the simulation world."""
        self.log_debug("Initializing world...")
        self.world = WorldFactory.create_world(PresetWorld(42, Coord(15, 15), ScaleGenValues.MEDIUM))
        
        for i in range(8):
            try:
                free_coords = TerrainQuery.random_free_coord(self.world.territory, self.world.entity_map, 1)
                creature = CreatureFactory.gen_creature(position=free_coords[0])
                WorldMotor.add_entity(self.world.territory, self.world.entity_map, creature, self.world.entities)
                self.log_debug(f"  ✓ Created creature #{i+1}: {creature.name}")
            except Exception as e:
                self.log_debug(f"  ✗ Error creating creature: {e}")
        
        self.log_debug("World initialized!")
        self.update_display()
    
    def setup_timer(self):
        """Setup the update timer."""
        self.timer = QTimer()
        self.timer.timeout.connect(self.tick)
        self.timer.start(500)  # Update every 500ms
    
    def tick(self):
        """Main tick for simulation."""
        try:
            if self.state == SimulationState.RUNNING and self.world is not None:
                current_time = time.time()
                if self.auto_step and (current_time - self.last_step_time >= self.step_interval):
                    Runner.run(self.world)
                    self.last_step_time = current_time
                    self.log_debug(f"✓ Auto-step executed (Time: {self.world.time})")
            
            self.update_display()
        except Exception as e:
            self.log_debug(f"ERROR in tick: {str(e)}")
    
    def update_display(self):
        """Update all display elements."""
        if self.world is None:
            return
        
        self.update_map_display()
        self.update_stats()
        self.update_creatures_table()
    
    def update_map_display(self):
        """Update the map display as table."""
        if self.world is None:
            return
        
        try:
            grid, size_x, size_y = MapRenderer.get_map_data(self.world)
            
            self.map_table.setRowCount(size_y)
            self.map_table.setColumnCount(size_x)
            
            # Remove header
            self.map_table.horizontalHeader().setVisible(False)
            self.map_table.verticalHeader().setVisible(False)
            
            # Set cell size
            for row in range(size_y):
                self.map_table.setRowHeight(row, 20)
            for col in range(size_x):
                self.map_table.setColumnWidth(col, 20)
            
            # Fill cells
            for y in range(size_y):
                for x in range(size_x):
                    symbol = grid[y][x]
                    item = QTableWidgetItem(symbol)
                    item.setFont(QFont("Arial", 12))
                    item.setTextAlignment(Qt.AlignCenter)
                    
                    # Color code
                    if symbol == "C":
                        item.setBackground(QColor("#FFD700"))  # Gold for creature
                    elif symbol == "X":
                        item.setBackground(QColor("#FF4444"))  # Red for corpse
                    else:
                        item.setBackground(QColor("#333333"))  # Dark for terrain
                    
                    self.map_table.setItem(y, x, item)
        except Exception as e:
            self.log_debug(f"ERROR in update_map_display: {str(e)}")
    
    def update_stats(self):
        """Update statistics labels."""
        try:
            if self.world is None:
                self.log_debug("ERROR: World is None in update_stats")
                return
            
            creatures = [e for e in self.world.entities.entitys.values() if isinstance(e, Creature)]
            corpses = [e for e in self.world.entities.entitys.values() if isinstance(e, Corpse)]
            
            total_energy = sum(c.energy.value for c in creatures)
            avg_life = sum(c.life.value for c in creatures) / len(creatures) if creatures else 0
            
            self.time_label.setText(f"⏱️  Time: {self.world.time}")
            self.creatures_label.setText(f"🦁 Creatures: {len(creatures)}")
            self.corpses_label.setText(f"💀 Corpses: {len(corpses)}")
            self.total_energy_label.setText(f"⚡ Total Energy: {total_energy:.1f}")
            self.avg_life_label.setText(f"❤️  Avg Life: {avg_life:.1f}")
            self.state_label.setText(f"State: {self.state.value.upper()}")
        except Exception as e:
            self.log_debug(f"ERROR in update_stats: {str(e)}")
    
    def update_creatures_table(self):
        """Update the creatures table."""
        if self.world is None:
            return
        
        creatures = [e for e in self.world.entities.entitys.values() if isinstance(e, Creature)]
        self.creatures_table.setRowCount(len(creatures))
        
        for row, creature in enumerate(creatures):
            species_name = CreatureInfoFormatter.get_species_name(creature)
            intent_name = CreatureInfoFormatter.get_intent_name(creature)
            
            items = [
                creature.name,
                species_name,
                intent_name,
                f"{creature.energy.value:.1f}/{creature.energy.limit:.1f}",
                f"{creature.life.value:.1f}",
                f"{creature.age.value}",
                f"{creature.hungry:.2f}",
                "✓" if creature.pregnant else "✗",
                f"({creature.position.x}, {creature.position.y})"
            ]
            
            for col, text in enumerate(items):
                item = QTableWidgetItem(str(text))
                item.setData(Qt.UserRole, creature)
                self.creatures_table.setItem(row, col, item)
    
    def update_creatures_view(self, view_name: str):
        """Update creatures view based on selection."""
        self.log_debug(f"Switched to {view_name} view")
    
    def on_creature_selected(self, item: QTableWidgetItem):
        """Handle creature selection from table."""
        creature = item.data(Qt.UserRole)
        if isinstance(creature, Creature):
            intent_name = CreatureInfoFormatter.get_intent_name(creature)
            species_name = CreatureInfoFormatter.get_species_name(creature)
            
            info = f"""
Selected Creature: {creature.name}

📊 Stats:
  - Energy: {creature.energy.value:.2f}/{creature.energy.limit:.2f}
  - Life: {creature.life.value:.2f}/{creature.life.limit:.2f}
  - Age: {creature.age.value}/{creature.age.limit}
  - Hungry: {creature.hungry:.4f}
  - Pregnant: {creature.pregnant}

🧬 Genome:
  - Species: {species_name}
  - Strength: {creature.genome.body.strength}
  - Life Limit: {creature.genome.body.life_limit}

📍 Position: ({creature.position.x}, {creature.position.y})

🎯 Intent: {intent_name}
            """
            self.selected_creature_text.setText(info.strip())
    
    def start_simulation(self):
        """Start the simulation."""
        self.state = SimulationState.RUNNING
        self.btn_start.setEnabled(False)
        self.btn_pause.setEnabled(True)
        self.log_debug("▶️ Simulation started")
        self.auto_step = self.auto_step_check.isChecked()
        self.last_step_time = time.time()
    
    def pause_simulation(self):
        """Pause the simulation."""
        self.state = SimulationState.PAUSED
        self.btn_start.setEnabled(True)
        self.btn_pause.setEnabled(False)
        self.log_debug("⏸️ Simulation paused")
    
    def step_simulation(self):
        """Execute a single step."""
        try:
            if self.world is not None:
                Runner.run(self.world)
                self.log_debug(f"⏭️ Step executed (Time: {self.world.time})")
                self.update_display()
                self.log_debug(f"   Display updated")
            else:
                self.log_debug("ERROR: World is None!")
        except Exception as e:
            self.log_debug(f"ERROR in step_simulation: {str(e)}")
    
    def reset_simulation(self):
        """Reset the simulation."""
        self.state = SimulationState.STOPPED
        self.btn_start.setEnabled(True)
        self.btn_pause.setEnabled(False)
        self.auto_step = False
        self.auto_step_check.setChecked(False)
        self.debug_text.clear()
        self.selected_creature_text.clear()
        self.init_world()
        self.log_debug("🔄 Simulation reset")
    
    def toggle_auto_step(self, state: int):
        """Toggle auto-step mode."""
        self.auto_step = state == Qt.Checked
        if self.state == SimulationState.RUNNING:
            self.log_debug(f"🔄 Auto-step {'enabled' if self.auto_step else 'disabled'}")
    
    def update_interval(self, value: int):
        """Update auto-step interval."""
        self.step_interval = value
        self.log_debug(f"⏱️ Step interval set to {value}s")
    
    def log_debug(self, message: str):
        """Log a debug message."""
        timestamp = time.strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] {message}\n"
        self.debug_text.append(log_entry)
        self.debug_text.moveCursor(QTextCursor.End)


def main():
    """Main entry point."""
    app = QApplication(sys.argv)
    
    # Set dark theme
    app.setStyle('Fusion')
    
    debugger = SimulationDebugger()
    debugger.show()
    
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
