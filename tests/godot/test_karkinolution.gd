extends SceneTree

func _init() -> void:
	print("[Test] Validating Karkinolution extension...")
	test_creature_instantiation()
	test_offline_parsing()
	print("[Test] All local tests passed successfully!")
	quit(0)


func test_creature_instantiation() -> void:
	var creature := GodotCreature.new()
	creature.set_id(42)
	creature.set_gender(0)
	creature.set_specie(0)

	assert(creature.get_id() == 42, "Incorrect ID")
	assert(creature.get_gender() == 0, "Incorrect gender")
	assert(creature.get_gender_name() == "MALE", "Incorrect gender name")
	assert(creature.get_specie() == 0, "Incorrect specie")
	assert(creature.get_specie_name() == "CRAB", "Incorrect specie name")

	print("[Test] GodotCreature instantiation verified successfully: ID=", creature.get_id(), " Specie=", creature.get_specie_name(), " Gender=", creature.get_gender_name())


func test_offline_parsing() -> void:
	var request_bytes := Karkinolution.build_get_creature_request(100)
	assert(request_bytes.size() > 0, "Request payload was not generated")

	var frame: GodotParsedFrame = Karkinolution.parse_frame(request_bytes)
	assert(frame != null, "Failed to parse generated frame")
	assert(frame.is_request(), "Frame should be identified as a request")

	print("[Test] Offline parsing verified: frame size=", frame.get_size(), " type=", frame.get_type(), " is_request=", frame.is_request())
