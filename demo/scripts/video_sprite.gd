@tool
class_name VideoSprite extends Sprite2D

var _video_player: VideoPlayer

@export var file_name: String = ""
@export var use_hardware: bool = false

func _ready() -> void:
	if use_hardware:
		_video_player = VideoPlayerMetal.new()
	else:
		_video_player = VideoPlayerSoft.new()
	
func _process(delta: float) -> void:
	_video_player.update_frame()
	texture = _video_player.get_texture()
	
func play() -> void:	
	_video_player.set_file_name(file_name)
	_video_player.play()
	
func stop() -> void:
	_video_player.stop()
	
func _validate_property(property: Dictionary):
	if property.name == "texture" or property.name == "_video_player":
		property.usage = PROPERTY_USAGE_NONE
