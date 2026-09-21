class_name CreatureAPI
extends BaseAPI

signal creature_received(creature: GodotCreature, storage: GodotEntityStorage)
signal error_received(sub_type: int, message: String)

var _pending_requests: Array[Dictionary] = []


func _init(transport: Node = null) -> void:
	super._init(transport)


func get_creature(creature_id: int, storage: GodotEntityStorage) -> bool:
	if _transport == null or not _transport.get("is_connected"):
		return false

	_pending_requests.push_back({
		"id": creature_id,
		"storage": storage
	})

	var frame: PackedByteArray = Karkinolution.build_get_creature_request(creature_id)
	return send_frame(frame)


func _on_frame_received(frame: GodotParsedFrame) -> void:
	if frame == null:
		return

	if frame.is_response() and frame.sub_type == 0:
		var creature_id: int = 0
		var storage: GodotEntityStorage = null

		if not _pending_requests.is_empty():
			var request: Dictionary = _pending_requests.pop_front()
			creature_id = request.get("id", 0)
			storage = request.get("storage", null)

		var creature: GodotCreature = Karkinolution.parse_creature(frame.payload, creature_id)
		if storage != null and creature != null:
			storage.add_creature(creature_id, creature)

		creature_received.emit(creature, storage)

	elif frame.is_error():
		if not _pending_requests.is_empty():
			_pending_requests.pop_front()

		var error_msg: String = frame.get_payload_string()
		error_received.emit(frame.sub_type, error_msg)
