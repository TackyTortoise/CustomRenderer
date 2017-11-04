Adjust render settings:
	To modify render settings, open Data/RenderSettings.txt and adjust the property values to your liking

Controls:
	wasd - move camera
	z/x - rotate camera around y axis
	up/down arrows - change render mode
	space - clear render
	
Create own materials:
	Go to Data/MaterialLibrary.txt and add your own material in this format
	<name>
		properties
	</name>
	
Create own scenes:
	Create a txt file with your scene info inside Data/Scenes, see Data/Scenes/TextScene.txt for an example
		Objects that can be created: plane, sphere, box, model and light
		These objects use materials by name from the MaterialLibrary.txt mentioned above
		Rotation angles are defined in radians!
		
	To load your own scene go to Data/SceneLibrary.txt and add your own scene in this format
	<Scene>
	File = [scenename].txt
	</Scene>