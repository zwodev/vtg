@tool
extends EditorPlugin

var play_button: Button
var selected_sprite: VideoSprite

func _enter_tree() -> void:
	# Create the button
	play_button = Button.new()
	play_button.text = "Play Video"
	play_button.pressed.connect(_on_play_pressed)
	
	# Hide by default
	play_button.hide()
	
	# Add to the editor's canvas menu
	add_control_to_container(EditorPlugin.CONTAINER_CANVAS_EDITOR_MENU, play_button)

func _exit_tree() -> void:
	# Clean up
	if play_button:
		remove_control_from_container(EditorPlugin.CONTAINER_CANVAS_EDITOR_MENU, play_button)
		play_button.queue_free()

func _handles(object: Object) -> bool:
	return object is VideoSprite

func _edit(object: Object) -> void:
	selected_sprite = object as VideoSprite

func _make_visible(visible: bool) -> void:
	if play_button:
		play_button.visible = visible

func _on_play_pressed() -> void:
	if selected_sprite:
		selected_sprite.play() 
