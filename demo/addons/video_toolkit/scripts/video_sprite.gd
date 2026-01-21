@tool
class_name VideoSprite extends Sprite2D

var _video_player: VideoPlayer

@export var file_name: String = "":
	get:
		return file_name
	set(value):
		file_name = value
		if _video_player:
			_video_player.file_name = file_name

@export var looping: bool = false:
	get:
		return looping
	set(value):
		looping = value
		if _video_player:
			_video_player.set_looping(looping)

func _ready() -> void:
	_video_player = VideoPlayerSoft.new()
	_video_player.file_name = file_name
	
func _process(_delta: float) -> void:
	_video_player.update_frame()
	texture = _video_player.get_texture()
	
func play() -> void:	
	_video_player.play()
	
func pause() -> void:
	_video_player.pause()
	
func stop() -> void:
	_video_player.stop()
	
func _validate_property(property: Dictionary):
	if property.name == "texture" or property.name == "_video_player":
		property.usage = PROPERTY_USAGE_NONE
