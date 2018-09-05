#pragma once
/*
try to get pythia initialization faster and also multiple different types of processes

working on getting particles going down beamline
Need to render the extra alpha detail in the beamline shader based on the time gotten in beam::Update()

Also need to interface Beam with Event class so the Event class can know what a event happens for syncronization

Add more ways to control the pileup

1) swithc to multiDrawArrays - done

2) add in a unit vector with 2 angles that give the velocity direction (using the vector of r-(0,0,0) as reference) and pass in a uniform that says how far 'between' points you are
this will increase vtx array size by a factor of 1/5th (1 extra short w/ 2 (byte) angles of around degree precision) but reduce number of vertices by a factor of 2 (or more) total
bonus: fade variable can be divided by the number of interpolated points and multiplied by it again in the shader (uniform) which means you can safely combine
it with the color byte (reduction of another short, so no net loss)

3) Try to speed up buffer binding by splitting up binds around multiple frames (index and vertex currently already split up)

4) SubData seems faster, but need to figure out what size of buffer to create (right now using 100MB), maybe put some smart code to figure out when buffer is full and Cull tracks?















*/