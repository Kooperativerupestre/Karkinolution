extends SceneTree

func _init() -> void:
	print("[Test] Validating Karkinolution extension...")
	test_offline_parsing()
	print("[Test] All local tests passed successfully!")
	quit(0)


func test_offline_parsing() -> void:
	var request_bytes := Karkinolution.build_get_creature_request(100)
	assert(request_bytes.size() > 0, "Request payload was not generated")

	var frame: GodotParsedFrame = Karkinolution.parse_frame(request_bytes)
	assert(frame != null, "Failed to parse generated frame")
	assert(frame.is_request(), "Frame should be identified as a request")

	print("[Test] Offline parsing verified: frame size=", frame.get_size(), " type=", frame.get_type(), " is_request=", frame.is_request())
