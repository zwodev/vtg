@tool
extends EditorInspectorPlugin

func _can_handle(object: Object) -> bool:
	# Replace VideoSprite with your actual node or class name
	var result = object is VideoSprite
	print("can_handle called: ", result)
	return result

func _parse_category(object: Object, category: String) -> void:
	if category != "video_sprite.gd":
		return
		
	print("parse_begin called")
	var play_button = Button.new()
	play_button.text = "Play"
	play_button.pressed.connect(func():
		if object:
			object.play()
	)
	add_custom_control(play_button)
	
	var stop_button = Button.new()
	stop_button.text = "Stop"
	stop_button.pressed.connect(func():
		if object:
			object.stop()
	)
	add_custom_control(stop_button)
