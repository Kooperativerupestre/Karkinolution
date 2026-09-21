class_name BaseAPI
extends RefCounted

var _transport: Node = null


func _init(transport: Node = null) -> void:
	if transport != null:
		bind_transport(transport)


func bind_transport(transport: Node) -> void:
	_transport = transport
	if _transport.has_signal("frame_received"):
		if not _transport.frame_received.is_connected(_on_frame_received):
			_transport.frame_received.connect(_on_frame_received)


func send_frame(frame: PackedByteArray) -> bool:
	if _transport == null or not _transport.get("is_connected"):
		return false
	return _transport.send_frame(frame)


func _on_frame_received(_frame: GodotParsedFrame) -> void:
	pass
