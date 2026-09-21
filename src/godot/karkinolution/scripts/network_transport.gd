class_name NetworkTransport
extends Node

signal connected_to_server
signal connection_failed
signal disconnected_from_server
signal frame_received(frame: GodotParsedFrame)

var socket := StreamPeerTCP.new()
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


func send_frame(frame: PackedByteArray) -> bool:
	if not is_connected:
		return false
	var err := socket.put_data(frame)
	return err == OK


func receive_data() -> void:
	var available := socket.get_available_bytes()
	if available <= 0:
		return

	var result := socket.get_data(available)
	if result[0] != OK:
		return

	var bytes: PackedByteArray = result[1]
	var frame: GodotParsedFrame = Karkinolution.parse_frame(bytes)
	if frame != null:
		frame_received.emit(frame)
