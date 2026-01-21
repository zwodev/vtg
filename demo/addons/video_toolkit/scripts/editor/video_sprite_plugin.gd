@tool
extends EditorPlugin

var inspector_plugin : EditorInspectorPlugin

func _enter_tree():
	inspector_plugin = preload("res://addons/video_toolkit/scripts/editor/video_sprite_inspector_plugin.gd").new()
	add_inspector_plugin(inspector_plugin)
	print("EditorPlugin _enter_tree called")

func _exit_tree():
	remove_inspector_plugin(inspector_plugin)
	print("EditorPlugin _exit_tree called")
