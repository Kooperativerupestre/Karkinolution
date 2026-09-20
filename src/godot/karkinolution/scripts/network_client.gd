extends Node

signal connected_to_server
signal connection_failed
signal disconnected_from_server
signal creature_received(creature: GodotCreature)
signal error_received(sub_type: int, message: String)

var socket := StreamPeerTCP.new()
var last_requested_id: int = 1
var is_connected: bool = false


func connect_to_server(host: String, port: int) -> void:
	var error := socket.connect_to_host(host, port)
	if error != OK:
		connection_failed.emit()


func _process(_delta: float) -> void:
	var poll_error := socket.poll()
	if poll_error != OK:
		return

	var status := socket.get_status()

	if status == StreamPeerSocket.STATUS_CONNECTED:
		if not is_connected:
			is_connected = true
			connected_to_server.emit()

		receive_data()

	elif is_connected and status == StreamPeerSocket.STATUS_NONE:
		is_connected = false
		disconnected_from_server.emit()
		set_process(false)

	elif status == StreamPeerSocket.STATUS_ERROR:
		is_connected = false
		connection_failed.emit()
		set_process(false)


func request_creature(creature_id: int) -> void:
	last_requested_id = creature_id
	var frame := Karkinolution.build_get_creature_request(creature_id)
	socket.put_data(frame)


func receive_data() -> void:
	var available := socket.get_available_bytes()
	if available <= 0:
		return

	var result := socket.get_data(available)
	if result[0] != OK:
		return

	var bytes: PackedByteArray = result[1]
	var frame: GodotParsedFrame = Karkinolution.parse_frame(bytes)

	if frame.is_response():
		if frame.sub_type == 0:
			var creature: GodotCreature = Karkinolution.parse_creature(frame.payload, last_requested_id)
			creature_received.emit(creature)
	elif frame.is_error():
		var error_msg := frame.get_payload_string()
		error_received.emit(frame.sub_type, error_msg)
