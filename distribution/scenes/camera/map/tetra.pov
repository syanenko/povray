
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	tetra
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Itetra.pov
+H600
+W1732
+A0.001
*/
