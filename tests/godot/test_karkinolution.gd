extends SceneTree

const HOST := "127.0.0.1"
const PORT := 9000
const TIMEOUT_SECONDS := 5.0

func _init() -> void:
	print("[Test] Validating Karkinolution extension...")
	test_offline_parsing()
	test_server_connection()
	print("[Test] All tests passed successfully!")
	quit(0)


func test_offline_parsing() -> void:
	var request_bytes := Karkinolution.build_get_creature_request(100)
	assert(request_bytes.size() > 0, "Request payload was not generated")

	var frame: GodotParsedFrame = Karkinolution.parse_frame(request_bytes)
	assert(frame != null, "Failed to parse generated frame")
	assert(frame.is_request(), "Frame should be identified as a request")

	print("[Test] Offline parsing verified: frame size=", frame.get_size(), " type=", frame.get_type(), " is_request=", frame.is_request())


func test_server_connection() -> void:
	print("[Test] Connecting to server at %s:%d..." % [HOST, PORT])
	var socket := StreamPeerTCP.new()
	var err := socket.connect_to_host(HOST, PORT)
	assert(err == OK, "Failed to initiate connection to server")

	var start_time := Time.get_ticks_msec()
	while socket.get_status() == StreamPeerSocket.STATUS_CONNECTING:
		socket.poll()
		OS.delay_msec(10)
		if (Time.get_ticks_msec() - start_time) > TIMEOUT_SECONDS * 1000:
			assert(false, "Connection to server timed out")

	assert(socket.get_status() == StreamPeerSocket.STATUS_CONNECTED, "Socket is not connected")
	print("[Test] Connected to server successfully.")

	var request_bytes := Karkinolution.build_get_creature_request(1)
	var put_err := socket.put_data(request_bytes)
	assert(put_err == OK, "Failed to send request data to server")

	start_time = Time.get_ticks_msec()
	while socket.get_available_bytes() <= 0:
		socket.poll()
		if socket.get_status() != StreamPeerSocket.STATUS_CONNECTED:
			assert(false, "Disconnected from server while waiting for response")
		OS.delay_msec(10)
		if (Time.get_ticks_msec() - start_time) > TIMEOUT_SECONDS * 1000:
			assert(false, "Timed out waiting for server response")

	var available := socket.get_available_bytes()
	assert(available > 0, "No bytes available from socket")

	var result := socket.get_data(available)
	assert(result[0] == OK, "Failed to read data from socket")

	var bytes: PackedByteArray = result[1]
	var response_frame: GodotParsedFrame = Karkinolution.parse_frame(bytes)
	assert(response_frame != null, "Failed to parse response frame")

	assert(response_frame.is_error(), "Expected error response for non-existent creature")
	assert(response_frame.get_sub_type() == 1, "Expected sub_type 1 (CREATURE_WAS_NOT_FOUND)")
	assert(response_frame.get_payload_string() == "Creature with id = 1", "Payload string mismatch")

	socket.disconnect_from_host()
	print("[Test] Server response verified: is_error=", response_frame.is_error(),
		" sub_type=", response_frame.get_sub_type(),
		" message=", response_frame.get_payload_string())
