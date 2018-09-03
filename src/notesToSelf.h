#pragma once
/*
try to get pythia initialization faster and also multiple different types of processes

working on getting particles going down beamline
Need to render the extra alpha detail in the beamline shader based on the time gotten in beam::Update()

Also need to interface Beam with Event class so the Event class can know what a event happens for syncronization

Add more ways to control the pileup



Need to speed up drawing a ton of lines because it is taking too long to make all the draw commands
Idea:   1 vertex array with all the vertices with x,y,z,i (will be big, i means its the ith index on a track)
		1 large index array that uses multiDrawElements here or primitive restart
		https://www.khronos.org/opengl/wiki/Vertex_Rendering















*/