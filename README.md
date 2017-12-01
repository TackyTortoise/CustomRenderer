# CustomRenderer  
This is a raytracer I wrote as part of my graduation work for the biggest part of 6 weeks.  
It is written completely from scratch in C++, using SDL for window managment and input and Assimp for loading model data.  

# Controls
wasd - move camera  
z/x - rotate camera around y axis  
up/down arrows - change render mode  
space - clear render  

# Usage  
You can also check out the readme in the Sample Build folder.  
  
Adjust render settings:  
	To modify render settings, open Data/RenderSettings.txt and adjust the property values to your liking  
  
Create own materials:  
	Go to Data/MaterialLibrary.txt and add your own material in this format  
	<name>  
		properties  
	</name>  
  
Create own scenes:  
	Create a txt file with your scene info inside Data/Scenes, see Data/Scenes/TextScene.txt for an example  
		Objects that can be created: plane, sphere, box, model and light  
		These objects use materials by name from the MaterialLibrary.txt mentioned above  
		Rotation angles are defined in degrees    
		
	To load your own scene go to Data/SceneLibrary.txt and add your own scene in this format  
	<Scene>  
	File = [scenename].txt  
	</Scene>  
