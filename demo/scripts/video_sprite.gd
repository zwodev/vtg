@tool
class_name VideoSprite extends Sprite2D

var _video_texture: VideoTexture

@export var file_name: String = ""

func _init() -> void:
	_video_texture = VideoTexture.new()
	
func _ready() -> void:
	texture = _video_texture
	#play()
	
func _process(delta: float) -> void:
	_video_texture.update_frame()
	
func play() -> void:
	_video_texture.set_file_name(file_name)
	_video_texture.play()
	
func stop() -> void:
	_video_texture.stop()
	
func _validate_property(property: Dictionary):
	if property.name == "texture" or property.name == "_video_texture":
		property.usage = PROPERTY_USAGE_NONE
