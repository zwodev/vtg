extends Sprite2D

var _video_texture: VideoTexture

@export var file_name: String:
	set(value):
		file_name = value
		_video_texture.set_file_name(file_name)

func _init():
	_video_texture = VideoTexture.new()

func _ready() -> void:
	print(file_name)
	texture = _video_texture
	_video_texture.play()
	
func _process(delta: float) -> void:
	_video_texture.update_frame()
