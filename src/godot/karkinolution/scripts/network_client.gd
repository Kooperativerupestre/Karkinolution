class_name NetworkClient
extends NetworkTransport

signal creature_received(creature: GodotCreature)
signal error_received(sub_type: int, message: String)

var creature_api: CreatureAPI


func _init() -> void:
	creature_api = CreatureAPI.new(self)
	creature_api.creature_received.connect(func(creature: GodotCreature, _storage: GodotEntityStorage) -> void:
		creature_received.emit(creature)
	)
	creature_api.error_received.connect(func(sub_type: int, message: String) -> void:
		error_received.emit(sub_type, message)
	)


func get_creature(creature_id: int, storage: GodotEntityStorage) -> bool:
	return creature_api.get_creature(creature_id, storage)


func request_creature(creature_id: int, storage: GodotEntityStorage = null) -> bool:
	return creature_api.get_creature(creature_id, storage)
