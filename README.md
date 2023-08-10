# MSci_Project-_Calculating_Curvature
C++ Code for Calculating Interfacial Curvature from Pore-Scale microCT Images

This repository displays the C++ code I wrote from scratch as part of my MSci in Geophysics at Imperial College London.
Along with the C++ files, there is a copy of the paper I submitted explaining the project and the maths behind it in great detail.
For a more simple overview, a copy of a poster I created which explains the project is also included.

In summary, the code reads in a .TIFF file (which is a 3-D rasterised image of a microCT scan) of a rock which contains fluids within its pore space. The code then creates a mesh to represent the rock-fluid or fluid-fluid interfaces. The interfaces are then smoothed to create a more accurate model. Finally, the interfacial curvature is calculated from the smoothed mesh, which can be used to understand the pore-scale physics of the fluids within the rock.

I wrote this code to be run from the compiler, with different algorithms being used depending on which stage of the maths is required.
The code requires significant further cleaning and refactoring, and the surface smoothing algorithm requires significant further work.
The Marching Cubes and Calculating Curvature algorithms run and produce the desired outputs, with minor errors.

If I were to develop this further, I would start by debugging some of the current errors in the code, creating an application with the code so that the project doesn't need to be run from within the compiler, and improving the surface smoothing algorithm.

Please note, this code was written for the purposes of my research and so functionality is prioritised over computational efficiency.

For a full list of known errors, please see Error_List.txt
