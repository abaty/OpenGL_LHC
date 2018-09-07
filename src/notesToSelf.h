#pragma once
/*

different types of pythia processes

working on getting particles going down beamline
Need to render the extra alpha detail in the beamline shader based on the time gotten in beam::Update()

3) Try to speed up buffer binding by splitting up binds around multiple frames (index and vertex currently already split up)





                            OLD IDEAS
--------------------------------------------------------------------------

in a unit vector with 2 angles that give the velocity direction (using the vector of r-(0,0,0) as reference) and pass in a uniform that says how far 'between' points you are
this will increase vtx array size by a factor of 1/5th (1 extra short w/ 2 (byte) angles of around degree precision) but reduce number of vertices by a factor of 2 (or more) total
bonus: fade variable can be divided by the number of interpolated points and multiplied by it again in the shader (uniform) which means you can safely combine
it with the color byte (reduction of another short, so no net loss)
*/
