extends Node2D

@onready var network_client: Node = $NetworkClient


func _ready() -> void:
	network_client.connect_to_server("127.0.0.1", 9000)


func _on_network_client_connected_to_server() -> void:
	print("[Main] Successfully connected to server!")


func _on_network_client_connection_failed() -> void:
	printerr("[Main] Failed to connect to server.")


func _on_network_client_disconnected_from_server() -> void:
	print("[Main] Server disconnected.")


func _on_network_client_creature_received(creature: GodotCreature) -> void:
	print("[Main] Creature received: ID=", creature.get_id(),
		" | Specie=", creature.get_specie_name(), "(", creature.get_specie(), ")",
		" | Gender=", creature.get_gender_name(), "(", creature.get_gender(), ")")


func _on_network_client_error_received(sub_type: int, message: String) -> void:
	const ERROR_NAMES: Dictionary = {
		1: "CreatureWasNotFound",
	}
	var error_name: String = ERROR_NAMES.get(sub_type, "Unknown(%d)" % sub_type)
	printerr("[Main] Error (%s) received from server: %s" % [error_name, message])
