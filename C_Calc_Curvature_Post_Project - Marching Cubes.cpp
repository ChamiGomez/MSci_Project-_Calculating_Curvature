#include "C_Calc_Curvature_Post_Project.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

//Declare lookup tables

/*EdgeTable maps the vertices under the isosurface to the intersecting edges.
An 8 bit index is formed (where each bit corresponds to a vertex).
Looking up EdgeTable returns a 12 bit number (where each bit corresponds to an edge, and 0 or 1 means the edge isn't or is cut by the isosurface).
This is achieved using hexadecimal code to represent the 12 bit binary number, with each element of the number corresponding to an edge.
If none of the edges are cut then the table returns a 0, which occurs when cubeindex is 0 (all vertices below isosurface) or 0xff (all vertices above isosurface).
*/
int EdgeTable[256] = {
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

/*TriTable is used to form the correct triangle(s) to represent the isosurface from within the cube.
The triangles are listed by returning the three edges that each triangle touches, for up to 5 triangles that can represent the isosurface within the cube.
The list is terminated by the invalid entry -1, hence (5*3)+1 = 16 elements in the lists.
*/
int TriTable[256][16] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
        {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
        {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
        {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
        {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
        {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
        {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
        {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
        {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
        {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
        {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
        {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
        {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
        {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
        {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
        {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
        {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
        {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
        {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
        {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
        {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
        {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
        {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
        {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
        {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
        {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
        {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
        {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
        {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
        {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
        {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
        {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
        {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
        {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
        {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
        {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
        {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
        {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
        {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
        {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
        {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
        {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
        {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
        {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
        {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
        {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
        {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
        {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
        {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
        {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
        {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
        {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
        {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
        {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
        {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
        {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
        {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
        {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
        {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
        {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
        {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
        {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
        {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
        {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
        {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
        {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
        {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
        {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
        {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
        {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
        {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
        {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
        {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
        {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
        {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
        {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
        {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
        {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
        {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
        {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
        {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
        {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
        {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
        {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
        {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
        {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
        {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
        {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
        {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
        {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
        {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
        {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
        {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
        {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
        {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
        {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
        {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

/*Functions to be called during marching cubes*/

//Parse voxel data into a cube (returned as a list with 8 values)
void CCalculatingCurvature::Parse_Voxel_Data_into_Cube(int x, int y, int z, vector<int> dataset_size) {
    //Note: (0,0,0) represents the top-left corner of a top-layer (where z is depth) of a 3D raster. This is due to how data was stored in voxel_field_data

    //Check the index to make sure the top layer's top-left corner is not on the bottom layer, on the bottom row or on the right column
    if (x >= dataset_size[2] - 1) { //Indexing starts at 0
        cout << "Error: index for this cube is incorrect! x = " << x << " when the maximum value allowed is " << dataset_size[2] - 2 << "!" << endl;
        exit(0);
    }
    if (y >= dataset_size[1] - 1) { //Indexing starts at 0
        cout << "Error: index for this cube is incorrect! y = " << y << " when the maximum value allowed is " << dataset_size[1] - 2 << "!" << endl;
        exit(0);
    }
    if (z >= dataset_size[0] - 1) { //Indexing starts at 0
        cout << "Error: index for this cube is incorrect! z = " << z << " when the maximum value allowed is " << dataset_size[0] - 2 << "!" << endl;
        exit(0);
    }

    //Required to remove byte error
    int x1, y1, z1;
    x1 = x + 1;
    y1 = y + 1;
    z1 = z + 1;

    //Initial setup
    vertices_coordinates.clear();
    vertices_coordinates.resize(8); //Resize outer list
    vertices_values.clear();

    //Top Layer
    vertices_coordinates[0].push_back(x); //Top left corner
    vertices_coordinates[0].push_back(y);
    vertices_coordinates[0].push_back(z);
    vertices_coordinates[1].push_back(x1); //Top right corner
    vertices_coordinates[1].push_back(y);
    vertices_coordinates[1].push_back(z);
    vertices_coordinates[2].push_back(x1); //Bottom right corner
    vertices_coordinates[2].push_back(y1);
    vertices_coordinates[2].push_back(z);
    vertices_coordinates[3].push_back(x); //Bottom left corner
    vertices_coordinates[3].push_back(y1);
    vertices_coordinates[3].push_back(z);
    //Bottom layer
    vertices_coordinates[4].push_back(x); //Top left corner
    vertices_coordinates[4].push_back(y);
    vertices_coordinates[4].push_back(z1);
    vertices_coordinates[5].push_back(x1); //Top right corner
    vertices_coordinates[5].push_back(y);
    vertices_coordinates[5].push_back(z1);
    vertices_coordinates[6].push_back(x1); //Bottom right corner
    vertices_coordinates[6].push_back(y1);
    vertices_coordinates[6].push_back(z1);
    vertices_coordinates[7].push_back(x); //Bottom left corner
    vertices_coordinates[7].push_back(y1);
    vertices_coordinates[7].push_back(z1);

    //Indices are reveresed due to voxel_field_data structure
    vertices_values.push_back(pixel_values[z][y][x]); //Top layer, top left corner
    vertices_values.push_back(pixel_values[z][y][x1]); //Top layer, top right corner
    vertices_values.push_back(pixel_values[z][y1][x1]); //Top layer, bottom right corner
    vertices_values.push_back(pixel_values[z][y1][x]); //Top layer, bottom left corner
    vertices_values.push_back(pixel_values[z1][y][x]); //Bottom layer, top left corner
    vertices_values.push_back(pixel_values[z1][y][x1]); //Bottom layer, top right corner
    vertices_values.push_back(pixel_values[z1][y1][x1]); //Bottom layer, bottom right corner
    vertices_values.push_back(pixel_values[z1][y1][x]); //Bottom layer, bottom left corner

    /*
    if (x == 3 && y == 3 && z == 3) {
        cout << "Vertices Coordinates (x,y,z) for single cube:" << endl;
        //Write_2D_Matrix_to_Screen<int>(vertices_coordinates); //Generates linker error
        cout << "\nVertices Values for single cube" << endl;
        //Write_List_to_Screen(vertices_values); //Generates linker error
    }*/
}

bool CCalculatingCurvature::Check_Triangle(vector<double> vertex_1, vector<double> vertex_2, vector<double> vertex_3) {
    //Returns false if any of the vertices are the same; true if the all vertices are unique
    //Note: Triangle could technically still have a zero area if the three vertices fell on one straight line, but this shouldn't be the case in marching cubes

    //Check for 3 coordinates in each of the vertices
    if (vertex_1.size() != 3 || vertex_2.size() != 3 || vertex_3.size() != 3) {
        cout << "Lists passed to Check_Triangle() are not all 3 elements long! List size(s) is/are:" << endl;
        if (vertex_1.size() != 3)
            cout << "Vertex_1 size: " << vertex_1.size() << "\t";
        if (vertex_2.size() != 3)
            cout << "Vertex_2 size: " << vertex_2.size() << "\t";
        if (vertex_3.size() != 3)
            cout << "Vertex_3 size: " << vertex_3.size() << "\t";
        cout << endl;
        exit(0);
    }

    //Check vertex_1 and vertex_2
    if (vertex_1[0] == vertex_2[0]) {
        if (vertex_1[1] == vertex_2[1]) {
            if (vertex_1[2] == vertex_2[2])
                return false;
        }
    }
    //Check vertex_1 and vertex_3
    if (vertex_1[0] == vertex_3[0]) {
        if (vertex_1[1] == vertex_3[1]) {
            if (vertex_1[2] == vertex_3[2])
                return false;
        }
    }
    //Check vertex_2 and vertex_3
    if (vertex_2[0] == vertex_3[0]) {
        if (vertex_2[1] == vertex_3[1]) {
            if (vertex_2[2] == vertex_3[2])
                return false;
        }
    }

    //None of the vertices are the same
    return true;
}

bool CCalculatingCurvature::Check_Isosurface_Coordinates(vector<double> coordinates, vector<int> dataset_size) {
    //Returns false if the point has already been mapped; true if the point is new to the isosurface
    //Note: Coordinates are provided as (x, y, z)

    /*How many cubes have got the isosurface in on this / the previous layer(z) has not been calculated, so don't know how many boxes to go back.
    Instead, check over all cubes so far within this layer, and all cubes in the previous layer up to this cube - 1 in each direction
    This can be improved to iterate over fewer cubes in the future*/
    int num_voxels_in_layer = (dataset_size[1] - 1) * (dataset_size[2] - 1); //-1 because there are 1 fewer cubes than there are elements for a row or a column
    int num_voxels_to_iterate_over = num_voxels_in_layer + dataset_size[2]; //A layer + a row before + a column before (second half is actually: ... +  (dataset_size[2] - 1) + 1)

    for (int i = 0; i < num_voxels_to_iterate_over && i < isosurface_coordinates.size(); i++) { //Iterate up to the number of voxels required or the beginning of the datset
        for (int j = 0; j < 12; j++) { //For each edge
            //If each of the coordinates is equal
            if (isosurface_coordinates[(isosurface_coordinates.size() - 1) - i][j].empty())
                continue;
            else if (isosurface_coordinates[(isosurface_coordinates.size() - 1) - i][j][0] == coordinates[0]) { ///Change to outer vector
                if (isosurface_coordinates[(isosurface_coordinates.size() - 1) - i][j][1] == coordinates[1]) {
                    if (isosurface_coordinates[(isosurface_coordinates.size() - 1) - i][j][2] == coordinates[2])
                        return false;
                }
            }
        }
    }

    return true;
}

int CCalculatingCurvature::Calculate_Cube_Index(vector<int> vertices_values, double isolevel) {
    //Determines the index into the edge table which tells us which vertices are inside of the surface
    int cube_index = 0;

    //Check vertices_values length
    if (vertices_values.size() != 8) {
        cout << "Length of the vertices_values list is incorrect - it should be 8 but the length is actually " << vertices_values.size() << "!" << endl;
        exit(0);
    }

    /*
    |= is equivalent to += for binary, but uses bitwise or.
    Example: a = 1101 (13), b = 1011 (11). a|b = 1111 (15), a|=b means a = a|b so sets a = 1111 (15)
    */
    if (vertices_values[0] < isolevel) cube_index |= 1; ///Explains binary flags
    if (vertices_values[1] < isolevel) cube_index |= 2;
    if (vertices_values[2] < isolevel) cube_index |= 4;
    if (vertices_values[3] < isolevel) cube_index |= 8;
    if (vertices_values[4] < isolevel) cube_index |= 16;
    if (vertices_values[5] < isolevel) cube_index |= 32;
    if (vertices_values[6] < isolevel) cube_index |= 64;
    if (vertices_values[7] < isolevel) cube_index |= 128;

    return cube_index;
}

vector<double> CCalculatingCurvature::Linear_Interpolation(double isolevel, double error_margin, vector<double> pos_1, vector<double> pos_2, int val_1, int val_2) {
    vector<double> interpolated_isosurface_coorinates;

    //If isolevel is very close to a point of the points are very close to each other then skip interpolating and return an input position
    if (abs(val_1 - val_2) < error_margin) {
        interpolated_isosurface_coorinates.insert(begin(interpolated_isosurface_coorinates), begin(pos_1), end(pos_1));
        return interpolated_isosurface_coorinates;
    }
    else if (abs(isolevel - val_1) < error_margin) {
        interpolated_isosurface_coorinates.insert(begin(interpolated_isosurface_coorinates), begin(pos_1), end(pos_1));
        return interpolated_isosurface_coorinates;
    }
    else if (abs(isolevel - val_2) < error_margin) {
        interpolated_isosurface_coorinates.insert(begin(interpolated_isosurface_coorinates), begin(pos_2), end(pos_2));
        return interpolated_isosurface_coorinates;
    }
    //Perform linear interpolation
    else {
        double scale_factor;
        scale_factor = (isolevel - val_1) / (val_2 - val_1);
        interpolated_isosurface_coorinates.push_back(pos_1[0] + (scale_factor * (pos_2[0] - pos_1[0])));
        interpolated_isosurface_coorinates.push_back(pos_1[1] + (scale_factor * (pos_2[1] - pos_1[1])));
        interpolated_isosurface_coorinates.push_back(pos_1[2] + (scale_factor * (pos_2[2] - pos_1[2])));
    }
    return interpolated_isosurface_coorinates;
}

int CCalculatingCurvature::Marching_Cubes_for_Single_Cube(double isolevel, int single_cube_index) {
    //Determine which vertices are inside the surface
    int cube_index;
    cube_index = Calculate_Cube_Index(vertices_values, isolevel);

    //Find which edges are intersected by the surface
    if (EdgeTable[cube_index] == 0) //Cube is entirely in/out of the surface
        return(0);
    else { //Find the point of intersection of the isosurface with each edge
        //Resize isosurface_coordinates
        //cout << "Cube Index: " << cube_index << endl;
        if (single_cube_index + 1 >= isosurface_coordinates.size()) {
            isosurface_coordinates.resize(single_cube_index + 1);
            isosurface_coordinates[single_cube_index].resize(12);
        }

        //Find points of intersection using bitwise operator &
        ///Explain marching cubes further or reference paper
        vector<double> interpolated_isosurface_coordinates;
        if (EdgeTable[cube_index] & 1) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[0], vertices_coordinates[1], vertices_values[0], vertices_values[1]);
            isosurface_coordinates[single_cube_index][0].insert(begin(isosurface_coordinates[single_cube_index][0]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 2) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[1], vertices_coordinates[2], vertices_values[1], vertices_values[2]);
            isosurface_coordinates[single_cube_index][1].insert(begin(isosurface_coordinates[single_cube_index][1]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 4) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[2], vertices_coordinates[3], vertices_values[2], vertices_values[3]);
            isosurface_coordinates[single_cube_index][2].insert(begin(isosurface_coordinates[single_cube_index][2]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 8) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[3], vertices_coordinates[0], vertices_values[3], vertices_values[0]);
            isosurface_coordinates[single_cube_index][3].insert(begin(isosurface_coordinates[single_cube_index][3]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 16) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[4], vertices_coordinates[5], vertices_values[4], vertices_values[5]);
            isosurface_coordinates[single_cube_index][4].insert(begin(isosurface_coordinates[single_cube_index][4]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 32) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[5], vertices_coordinates[6], vertices_values[5], vertices_values[6]);
            isosurface_coordinates[single_cube_index][5].insert(begin(isosurface_coordinates[single_cube_index][5]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 64) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[6], vertices_coordinates[7], vertices_values[6], vertices_values[7]);
            isosurface_coordinates[single_cube_index][6].insert(begin(isosurface_coordinates[single_cube_index][6]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 128) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[7], vertices_coordinates[4], vertices_values[7], vertices_values[4]);
            isosurface_coordinates[single_cube_index][7].insert(begin(isosurface_coordinates[single_cube_index][7]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 256) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[0], vertices_coordinates[4], vertices_values[0], vertices_values[4]);
            isosurface_coordinates[single_cube_index][8].insert(begin(isosurface_coordinates[single_cube_index][8]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 512) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[1], vertices_coordinates[5], vertices_values[1], vertices_values[5]);
            isosurface_coordinates[single_cube_index][9].insert(begin(isosurface_coordinates[single_cube_index][9]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 1024) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[2], vertices_coordinates[6], vertices_values[2], vertices_values[6]);
            isosurface_coordinates[single_cube_index][10].insert(begin(isosurface_coordinates[single_cube_index][10]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 2048) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[3], vertices_coordinates[7], vertices_values[3], vertices_values[7]);
            isosurface_coordinates[single_cube_index][11].insert(begin(isosurface_coordinates[single_cube_index][11]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
    }

    //Create the triangle(s)
    //Resize outer dimension of triangles
    if (single_cube_index + 1 > triangles.size())
        triangles.resize(single_cube_index + 1);
    int num_triangles = 0;
    for (int i = 0; TriTable[cube_index][i] != -1; i += 3) {
        //Resize inner dimensions of triangles
        triangles[single_cube_index].resize(num_triangles + 1);
        triangles[single_cube_index][num_triangles].resize(3);
        //Allocate coordinates for each point in the triangle
        triangles[single_cube_index][num_triangles][0].insert(begin(triangles[single_cube_index][num_triangles][0]), begin(isosurface_coordinates[single_cube_index][TriTable[cube_index][i]]), end(isosurface_coordinates[single_cube_index][TriTable[cube_index][i]]));
        triangles[single_cube_index][num_triangles][1].insert(begin(triangles[single_cube_index][num_triangles][1]), begin(isosurface_coordinates[single_cube_index][TriTable[cube_index][i + 1]]), end(isosurface_coordinates[single_cube_index][TriTable[cube_index][i + 1]]));
        triangles[single_cube_index][num_triangles][2].insert(begin(triangles[single_cube_index][num_triangles][2]), begin(isosurface_coordinates[single_cube_index][TriTable[cube_index][i + 2]]), end(isosurface_coordinates[single_cube_index][TriTable[cube_index][i + 2]]));
        num_triangles++;
    }

    return num_triangles;
}

int CCalculatingCurvature::Marching_Cubes_for_Single_Cube_Test(double isolevel, int single_cube_index, vector<int> dataset_size, bool check_isosurface, bool check_triangles) {
    //Determine which vertices are inside the surface
    int cube_index;
    cube_index = Calculate_Cube_Index(vertices_values, isolevel);

    //Find which edges are intersected by the surface
    if (EdgeTable[cube_index] == 0) //Cube is entirely in/out of the surface
        return(0);
    else { //Find the point of intersection of the isosurface with each edge
        //Resize isosurface_coordinates
        isosurface_coordinates.resize(isosurface_coordinates.size() + 1);
        isosurface_coordinates[isosurface_coordinates.size() - 1].resize(12);

        //Find points of intersection using bitwise operator &
        ///Explain marching cubes bitwise operator usage or reference paper (and tables)
        vector<double> interpolated_isosurface_coordinates;
        bool new_coordinates;
        if (EdgeTable[cube_index] & 1) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[0], vertices_coordinates[1], vertices_values[0], vertices_values[1]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][0].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][0]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][0].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][0]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 2) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[1], vertices_coordinates[2], vertices_values[1], vertices_values[2]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][1].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][1]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][1].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][1]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 4) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[2], vertices_coordinates[3], vertices_values[2], vertices_values[3]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][2].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][2]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][2].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][2]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 8) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[3], vertices_coordinates[0], vertices_values[3], vertices_values[0]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][3].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][3]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][3].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][3]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 16) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[4], vertices_coordinates[5], vertices_values[4], vertices_values[5]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][4].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][4]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][4].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][4]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 32) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[5], vertices_coordinates[6], vertices_values[5], vertices_values[6]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][5].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][5]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][5].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][5]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 64) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[6], vertices_coordinates[7], vertices_values[6], vertices_values[7]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][6].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][6]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][6].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][6]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 128) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[7], vertices_coordinates[4], vertices_values[7], vertices_values[4]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][7].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][7]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][7].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][7]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 256) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[0], vertices_coordinates[4], vertices_values[0], vertices_values[4]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][8].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][8]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][8].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][8]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 512) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[1], vertices_coordinates[5], vertices_values[1], vertices_values[5]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][9].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][9]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][9].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][9]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 1024) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[2], vertices_coordinates[6], vertices_values[2], vertices_values[6]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][10].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][10]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][10].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][10]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
        if (EdgeTable[cube_index] & 2048) {
            interpolated_isosurface_coordinates = Linear_Interpolation(isolevel, 0.00001, vertices_coordinates[3], vertices_coordinates[7], vertices_values[3], vertices_values[7]);
            if (check_isosurface) {
                new_coordinates = Check_Isosurface_Coordinates(interpolated_isosurface_coordinates, dataset_size);
                if (new_coordinates)
                    isosurface_coordinates[isosurface_coordinates.size() - 1][11].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][11]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
            }
            else
                isosurface_coordinates[isosurface_coordinates.size() - 1][11].insert(begin(isosurface_coordinates[isosurface_coordinates.size() - 1][11]), begin(interpolated_isosurface_coordinates), end(interpolated_isosurface_coordinates));
        }
    }

    //Create the triangle(s)
    //Resize outer dimension of triangles
    triangles.resize(triangles.size() + 1);
    int num_triangles = 0;
    for (int i = 0; TriTable[cube_index][i] != -1; i += 3) {
        //Check the triangle has 3 unique vertices
        bool unique_vertices = true;
        if (check_triangles)
            unique_vertices = Check_Triangle(isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i]], isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i + 1]], isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i + 2]]);
        if (unique_vertices) {
            //Resize inner dimensions of triangles
            triangles[triangles.size() - 1].resize(num_triangles + 1);
            triangles[triangles.size() - 1][num_triangles].resize(3);
            //Allocate coordinates for each point in the triangle
            triangles[triangles.size() - 1][num_triangles][0].insert(begin(triangles[triangles.size() - 1][num_triangles][0]), begin(isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i]]), end(isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i]]));
            triangles[triangles.size() - 1][num_triangles][1].insert(begin(triangles[triangles.size() - 1][num_triangles][1]), begin(isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i + 1]]), end(isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i + 1]]));
            triangles[triangles.size() - 1][num_triangles][2].insert(begin(triangles[triangles.size() - 1][num_triangles][2]), begin(isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i + 2]]), end(isosurface_coordinates[isosurface_coordinates.size() - 1][TriTable[cube_index][i + 2]]));
            num_triangles++;
        }
    }

    return num_triangles;
}

void CCalculatingCurvature::Marching_Cubes(vector<int> dataset_size, double isolevel, bool output_cube_index_to_screen, int cube_index_output_frequency, bool check_isosurface, bool check_triangles) {
    cout << "Begin marching cubes..." << endl;
    if (output_cube_index_to_screen)
        cout << "Number of cubes to be marched: " << (dataset_size[0] - 1) * (dataset_size[1] - 1) * (dataset_size[2] - 1) << endl;
    int single_cube_index = 0;
    //Indices capped at dataset_size[n] - 1 to prevent parsing a cube at the edges of the dataset where no more data exits
    for (int i = 0; i < dataset_size[0] - 1; i++) { ///For each
        for (int j = 0; j < dataset_size[1] - 1; j++) { ///For each
            for (int k = 0; k < dataset_size[2] - 1; k++) { ///For each
                Parse_Voxel_Data_into_Cube(k, j, i, dataset_size); //Given backwards as arguments are in order (x, y, z)
                int num_triangles;
                single_cube_index = (i * (dataset_size[1] - 1) * (dataset_size[2] - 1)) + (j * (dataset_size[2] - 1)) + k; ///Calculate cube index
                if (output_cube_index_to_screen) { //For keeping track of larger datasets
                    if ((single_cube_index % cube_index_output_frequency) == 0)
                        cout << single_cube_index << "\t";
                }
                //num_triangles = Marching_Cubes_for_Single_Cube(isolevel, single_cube_index);
                num_triangles = Marching_Cubes_for_Single_Cube_Test(isolevel, single_cube_index, dataset_size, check_isosurface, check_triangles);
                //single_cube_index++; ///Include for efficieny if not first call?
            }
        }
    }

    if (output_cube_index_to_screen)
        cout << "Final cube index: " << single_cube_index << endl;

    cout << "All cubes have been marched!" << endl << endl;
}

void CCalculatingCurvature::Convert_Triangles_to_Faces(string isosurface_file, string triangles_file, bool output_triangle_number_to_screen, int triangle_number_output_frequency) {
    cout << "Begin converting triangles to faces..." << endl;
    //Declare input filestreams for reading in data and string for reading in data
    ifstream isosurface_data, triangles_data;
    string line_in;

    //Check files have opened correctly
    ///Use preliminary file opening check from data handling?
    isosurface_data.open(isosurface_file, fstream::in);
    triangles_data.open(triangles_file, fstream::in);
    if (isosurface_data.fail()) {
        cout << "Error opening isosurface file for reading" << endl << endl;
        exit(0);
    }
    else if (triangles_data.fail()) {
        cout << "Error opening triangles file for reading" << endl << endl;
        exit(0);
    }
    else
        cout << "Files opened for reading successfully..." << endl;

    //Read in isosurface_coords
    vector<vector<double>> isosurface_coords;
    //Read in each directory
    while (getline(isosurface_data, line_in)) {				//While there are lines to read in from the file, store them in line_in
        istringstream input_string_stream(line_in);			//Copy line_in to input_string_stream
        string word;										//Create a string that stores each word
        vector<double> temp_vector;                         //Create a temporary vector to store the doubles for each value for each line (as opposed to the strings for each word for each line)
        while (getline(input_string_stream, word, '\t')) {  //Split line_in (which is in the input_string_stream) into words (seperated by a tab delimiter)
            double element_value;	    					//Create a temporary variable to store the words (which are strings) as doubles
            element_value = stod(word);				       	//Convert the word into a double
            temp_vector.push_back(element_value);		    //Store the double in temp_vector
        }
        isosurface_coords.push_back(temp_vector);
    }
    cout << "Isosurface data read in from file successfully..." << endl;

    //Read in triangles
    vector<vector<vector<double>>> triangles_matrix;
    vector<vector<double>> triangle;
    //Read in each directory
    while (getline(triangles_data, line_in)) {			    	//While there are lines to read in from the file, store them in line_in
        if (line_in.empty()) {
            if (!triangle.empty()) {
                triangles_matrix.push_back(triangle);
                triangle.clear();
            }
        }
        else {
            istringstream input_string_stream(line_in);			//Copy line_in to input_string_stream
            string word;										//Create a string that stores each word
            vector<double> vertex;                              //Create a vector (vertex) to store the doubles (coordinates) for each value for each line (as opposed to the strings for each word for each line)
            while (getline(input_string_stream, word, '\t')) {  //Split line_in (which is in the input_string_stream) into words (seperated by a tab delimiter)
                double vertex_coordinate;	    				//Create a temporary variable to store the words (which are strings) as doubles
                vertex_coordinate = stod(word);				    //Convert the word into a double
                vertex.push_back(vertex_coordinate);	       	//Store the coordinate in vertex
            }
            triangle.push_back(vertex);
        }
    }
    //Flush last directory
    ///Incorporate into loop? Do while loop?
    if (!triangle.empty()) {
        triangles_matrix.push_back(triangle);
        triangle.clear();
    }
    cout << "Triangles data read in from file successfully..." << endl;

    //Convert triangles to faces
    cout << "Number of triangles to convert: " << triangles_matrix.size() << endl;
    faces.resize(triangles_matrix.size());
    for (int i = 0; i < triangles_matrix.size(); i++) { //For each triangle
        if (output_triangle_number_to_screen) {
            if ((i % triangle_number_output_frequency) == 0)
                cout << i << "\t";
        }
        for (int j = 0; j < triangles_matrix[i].size(); j++) { //For each row of the triangles
            if (j >= 3) cout << j << endl;
            for (int p = 0; p < isosurface_coords.size(); p++) { //For each row of the isosurface_coords
                //Check if the triangle's vertex matches the isosurface coordinate
                if (triangles_matrix[i][j] == isosurface_coords[p]) {
                    faces[i].push_back(p + 1); //Indexing starts at 1 not 0 for faces
                    continue;
                }
            }
        }
    }
    if (output_triangle_number_to_screen)
        cout << endl;
    else
        cout << "Number of faces: " << faces.size() << endl;
    cout << "Triangles converted to faces successfully." << endl;
}

void CCalculatingCurvature::Convert_Isosurface_Coordinates_to_Vertices(void) {
    cout << "Begin converting isosurface coordinates to vertices..." << endl;
    ///Better explanation here
    for (int i = 0; i < isosurface_coordinates.size(); i++) {
        if (isosurface_coordinates[i].empty()) //Shouldn't be needed if isosurface_coordinates have been checked
            continue;
        else {
            for (int j = 0; j < isosurface_coordinates[i].size(); j++) {
                if (isosurface_coordinates[i][j].empty()) //Shouldn't be needed if isosurface_coordinates have been checked
                    continue;
                else {
                    pre_smoothing_vertices.resize(pre_smoothing_vertices.size() + 1);
                    pre_smoothing_vertices[pre_smoothing_vertices.size() - 1].insert(begin(pre_smoothing_vertices[pre_smoothing_vertices.size() - 1]), begin(isosurface_coordinates[i][j]), end(isosurface_coordinates[i][j]));
                }
            }
        }
    }

    cout << "Number of vertices: " << pre_smoothing_vertices.size() << endl;
    cout << "Isosurface coordinates converted to vertices successfully." << endl;
}

void CCalculatingCurvature::Convert_Isosurface_Coordinates_to_Vertices(string isosurface_file) {
    cout << "Begin converting isosurface coordinates to vertices..." << endl;

    //Declare input filestreams for reading in data and string for reading in data
    ifstream isosurface_data;
    string line_in;

    //Check files have opened correctly
    ///Use preliminary file opening check
    isosurface_data.open(isosurface_file, fstream::in);
    if (isosurface_data.fail()) {
        cout << "Error opening isosurface file for reading" << endl << endl;
        exit(0);
    }
    else
        cout << "Files opened for reading successfully..." << endl;

    //Read in isosurface_coords
    vector<vector<double>> isosurface_coords;
    //Read in each directory
    while (getline(isosurface_data, line_in)) {				//While there are lines to read in from the file, store them in line_in
        istringstream input_string_stream(line_in);			//Copy line_in to input_string_stream
        string word;										//Create a string that stores each word
        vector<double> temp_vector;                         //Create a temporary vector to store the doubles for each value for each line (as opposed to the strings for each word for each line)
        while (getline(input_string_stream, word, '\t')) {  //Split line_in (which is in the input_string_stream) into words (seperated by a tab delimiter)
            double element_value;	    					//Create a temporary variable to store the words (which are strings) as doubles
            element_value = stod(word);				       	//Convert the word into a double
            temp_vector.push_back(element_value);		    //Store the double in temp_vector
        }
        isosurface_coords.push_back(temp_vector);
    }
    cout << "Isosurface data read in from file successfully..." << endl;

    ///Explain
    for (int i = 0; i < isosurface_coords.size(); i++) {
        pre_smoothing_vertices.resize(pre_smoothing_vertices.size() + 1);
        pre_smoothing_vertices[pre_smoothing_vertices.size() - 1].insert(begin(pre_smoothing_vertices[pre_smoothing_vertices.size() - 1]), begin(isosurface_coords[i]), end(isosurface_coords[i]));
    }

    cout << "Number of vertices: " << pre_smoothing_vertices.size() << endl;
    cout << "Isosurface coordinates converted to vertices successfully." << endl << endl;
}

void CCalculatingCurvature::Correct_Layering_for_Catenoids_Post_Marching_Cubes(string input_isosurface_file, string input_triangles_file, string output_isosurface_file, string output_triangles_file, int z_value, double z_step) {
    cout << "Begin correcting layers for catenoids post marching cubes..." << endl;
    //Declare input filestreams for reading in data and string for reading in data
    ifstream isosurface_data, triangles_data;
    string line_in;

    //Check files have opened correctly
    isosurface_data.open(input_isosurface_file, fstream::in);
    triangles_data.open(input_triangles_file, fstream::in);
    if (isosurface_data.fail()) {
        cout << "Error opening isosurface file for reading" << endl << endl;
        exit(0);
    }
    else if (triangles_data.fail()) {
        cout << "Error opening triangles file for reading" << endl << endl;
        exit(0);
    }
    else
        cout << "Files opened for reading successfully..." << endl;

    //Read in isosurface_coords
    vector<vector<double>> isosurface_coords;
    //Read in each directory
    while (getline(isosurface_data, line_in)) {				//While there are lines to read in from the file, store them in line_in
        istringstream input_string_stream(line_in);			//Copy line_in to input_string_stream
        string word;										//Create a string that stores each word
        vector<double> temp_vector;                         //Create a temporary vector to store the doubles for each value for each line (as opposed to the strings for each word for each line)
        while (getline(input_string_stream, word, '\t')) {  //Split line_in (which is in the input_string_stream) into words (seperated by a tab delimiter)
            double element_value;	    					//Create a temporary variable to store the words (which are strings) as doubles
            element_value = stod(word);				       	//Convert the word into a double
            temp_vector.push_back(element_value);		    //Store the double in temp_vector
        }
        isosurface_coords.push_back(temp_vector);
    }
    cout << "Isosurface data read in from file successfully..." << endl;
    isosurface_data.close();

    //Read in triangles
    vector<vector<vector<double>>> triangles_matrix;
    vector<vector<double>> triangle;
    //Read in each directory
    while (getline(triangles_data, line_in)) {			    	//While there are lines to read in from the file, store them in line_in
        if (line_in.empty()) {
            if (!triangle.empty()) {
                triangles_matrix.push_back(triangle);
                triangle.clear();
            }
        }
        else {
            istringstream input_string_stream(line_in);			//Copy line_in to input_string_stream
            string word;										//Create a string that stores each word
            vector<double> vertex;                              //Create a vector (vertex) to store the doubles (coordinates) for each value for each line (as opposed to the strings for each word for each line)
            while (getline(input_string_stream, word, '\t')) {  //Split line_in (which is in the input_string_stream) into words (seperated by a tab delimiter)
                double vertex_coordinate;	    				//Create a temporary variable to store the words (which are strings) as doubles
                vertex_coordinate = stod(word);				    //Convert the word into a double
                vertex.push_back(vertex_coordinate);	       	//Store the coordinate in vertex
            }
            triangle.push_back(vertex);
        }
    }
    //Flush last directory
    if (!triangle.empty()) {
        triangles_matrix.push_back(triangle);
        triangle.clear();
    }
    cout << "Triangles data read in from file successfully..." << endl;
    triangles_data.close();

    //Calculate top layer
    int num_layers = ((z_value / z_step) * 2) + 1 + 2;
    int top_limit = num_layers - 2; //-1 to start indexing layers from 0, and -1 to ensure that the 2nd from top isn't used (so correct coordinates to bottom of this layer)
    //Correct isosurface_coords
    for (int i = 0; i < isosurface_coords.size(); i++) {
        if (isosurface_coords[i][2] == 0.5)
            isosurface_coords[i][2] = 1.0;
        //Check to ensure calculations are fine (for current method of catenoid generation)
        else if (double(isosurface_coords[i][2]) - double(top_limit) > 0.5) {
            cout << "Isosurface z-coordinate for point " << i << ": " << isosurface_coords[i][2] << " but top limit is " << top_limit << " with correction margin of 0.5" << endl;
            isosurface_coords[i][2] = top_limit; //Still perform corrections
        }
        else if (isosurface_coords[i][2] > top_limit)
            isosurface_coords[i][2] = top_limit;
    }

    //Correct triangles
    for (int i = 0; i < triangles_matrix.size(); i++) {
        for (int j = 0; j < triangles_matrix[i].size(); j++) {
            if (triangles_matrix[i][j][2] == 0.5)
                triangles_matrix[i][j][2] = 1.0;
            //Check to ensure calculations are fine (for current method of catenoid generation)
            else if (double(triangles_matrix[i][j][2]) - double(top_limit) > 0.5) {
                cout << "Triangle z-coordinate for point " << i << ": " << triangles_matrix[i][j][2] << " but top limit is " << top_limit << " with correction margin of 0.5" << endl;
                triangles_matrix[i][j][2] = top_limit; //Still perform corrections
            }
            else if (triangles_matrix[i][j][2] > top_limit)
                triangles_matrix[i][j][2] = top_limit;
        }
    }

    //Output Corrected Results
    Write_2D_Matrix_to_File(isosurface_coords, "Corrected Isosurface Coordinates", output_isosurface_file);
    Write_3D_Matrix_to_File(triangles_matrix, "Corrected Triangles", output_triangles_file, false, true);

    //Output Closing
    cout << "Post marching cubes catenoid correction completed successfully." << endl;
}

void CCalculatingCurvature::Modify_Pixel_Values(int old_value, int new_value) {
    cout << "Begin modifying pixel values..." << endl;

    int modified_counter = 0, left_alone_counter = 0;
    for (int i = 0; i < pixel_values.size(); i++) {
        for (int j = 0; j < pixel_values[i].size(); j++) {
            for (int k = 0; k < pixel_values[i][j].size(); k++) {
                if (pixel_values[i][j][k] == old_value) {
                    pixel_values[i][j][k] = new_value;
                    modified_counter++;
                }
                else
                    left_alone_counter++;
            }
        }
    }

    cout << "There were " << left_alone_counter + modified_counter << " pixel values of which " << modified_counter << " were modified and " << left_alone_counter << " were left alone." << endl;
}

void CCalculatingCurvature::Swap_Pixel_Values(int pixel_value_1, int pixel_value_2) {
    cout << "Begin swaping pixel values..." << endl;

    int modified_counter = 0, left_alone_counter = 0;
    for (int i = 0; i < pixel_values.size(); i++) {
        for (int j = 0; j < pixel_values[i].size(); j++) {
            for (int k = 0; k < pixel_values[i][j].size(); k++) {
                if (pixel_values[i][j][k] == pixel_value_1) {
                    pixel_values[i][j][k] = pixel_value_2;
                    modified_counter++;
                }
                else if (pixel_values[i][j][k] == pixel_value_2) {
                    pixel_values[i][j][k] = pixel_value_1;
                    modified_counter++;
                }
                else
                    left_alone_counter++;
            }
        }
    }

    cout << "There were " << left_alone_counter + modified_counter << " pixel values of which " << modified_counter << " were swapped and " << left_alone_counter << " were left alone." << endl;
}

void CCalculatingCurvature::Find_Common_Interface(string interface_1_faces_file, string interface_1_vertices_file, string interface_2_faces_file, string interface_2_vertices_file) {
    //Output opening statement
    cout << "Begin finding common interface..." << endl;

    //Declare variables
    vector<vector<double>> interface_1_vertices, interface_2_vertices;
    vector<vector<int>> interface_1_faces, interface_2_faces;

    //Read in data
    interface_1_faces = Read_2D_Integer_Matrix_from_Text_File(interface_1_faces_file, "Interface 1 Faces");
    interface_1_vertices = Read_2D_Double_Matrix_from_Text_File(interface_1_vertices_file, "Interface 1 Vertices");
    interface_2_faces = Read_2D_Integer_Matrix_from_Text_File(interface_2_faces_file, "Interface 2 Faces");
    interface_2_vertices = Read_2D_Double_Matrix_from_Text_File(interface_2_vertices_file, "Interface 2 Vertices");

    //Find vertices that are in both interfaces
    for (int i = 0; i < interface_1_vertices.size(); i++) {
        for (int j = 0; j < interface_2_vertices.size(); j++) {
            /*
            if (interface_1_vertices[i].size() == interface_2_vertices[j].size() && equal(interface_1_vertices[i].begin(), interface_1_vertices[i].end(), interface_2_vertices[j].begin())) {
                vertices.resize(vertices.size() + 1);
                vertices[vertices.size() - 1].insert(begin(vertices[vertices.size() - 1]), begin(interface_1_vertices[i]), end(interface_1_vertices[i]));
                break;
            }*/
            if (interface_1_vertices[i][0] == interface_2_vertices[j][0]) {
                if (interface_1_vertices[i][1] == interface_2_vertices[j][1]) {
                    if (interface_1_vertices[i][2] == interface_2_vertices[j][2] - 1) {
                        pre_smoothing_vertices.resize(pre_smoothing_vertices.size() + 1);
                        pre_smoothing_vertices[pre_smoothing_vertices.size() - 1].insert(begin(pre_smoothing_vertices[pre_smoothing_vertices.size() - 1]), begin(interface_1_vertices[i]), end(interface_1_vertices[i]));
                        break;
                    }
                }
            }
        }
    }
    cout << "Interface 1 vertices: " << interface_1_vertices.size() << "\tInterface 2 vertices: " << interface_2_vertices.size() << "\tCommon interface vertices: " << pre_smoothing_vertices.size() << endl;


    //Create faces using indexing from interface 1
    for (int i = 0; i < interface_1_faces.size(); i++) {
        bool vertex_1_included = false, vertex_2_included = false, vertex_3_included = false;
        vector<int> triangle;
        //Check if all the vertices in the face are in the new list of vertices
        for (int j = 0; j < pre_smoothing_vertices.size(); j++) {
            if (pre_smoothing_vertices[j] == interface_1_vertices[interface_1_faces[i][0] - 1]) {
                vertex_1_included = true;
                triangle.push_back(j + 1);
            }
            else if (pre_smoothing_vertices[j] == interface_1_vertices[interface_1_faces[i][1] - 1]) {
                vertex_2_included = true;
                triangle.push_back(j + 1);
            }
            else if (pre_smoothing_vertices[j] == interface_1_vertices[interface_1_faces[i][2] - 1]) {
                vertex_3_included = true;
                triangle.push_back(j + 1);
            }
            if (vertex_1_included && vertex_2_included && vertex_3_included)
                break;
        }
        //If all the vertices are included, the face is included
        if (vertex_1_included && vertex_2_included && vertex_3_included) {
            faces.resize(faces.size() + 1);
            faces[faces.size() - 1].insert(begin(faces[faces.size() - 1]), begin(triangle), end(triangle));
        }
    }
    cout << "Interface 1 faces: " << interface_1_faces.size() << "\t\tInterface 2 faces: " << interface_2_faces.size() << "\t\tCommon interface faces: " << faces.size() << endl;

    //Remove verticies that are not used in faces over the common interface vertices
    //Store old vertices (used for creating new faces later)
    vector<vector<double>> old_vertices;
    old_vertices.resize(pre_smoothing_vertices.size());
    for (int i = 0; i < old_vertices.size(); i++)
        old_vertices[i].insert(begin(old_vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));

    //Remove additional vertices
    vector<int> indices_of_removed_vertices; //For reference/error checking, not required for functionality
    pre_smoothing_vertices.clear();
    for (int i = 0; i < old_vertices.size(); i++) {
        bool vertex_used = false;
        for (int j = 0; j < faces.size(); j++) {
            for (int k = 0; k < faces[j].size(); k++) {
                if (i == faces[j][k] - 1) {
                    vertex_used = true;
                    break;
                }
            }
            if (vertex_used)
                break;
        }
        if (!vertex_used) {
            indices_of_removed_vertices.push_back(i + 1);
        }
        else
            pre_smoothing_vertices.push_back(old_vertices[i]);

    }
    //Write_List_to_Screen(indices_of_removed_vertices);
    cout << "Number of common interface's vertices (old): " << old_vertices.size() << "\tNumber of common included interface's vertices: " << pre_smoothing_vertices.size() << endl;


    //Loop to rewrite faces with new indices
    int old_faces_size = faces.size();
    for (int i = 0; i < faces.size(); i++) {
        vector<int> new_face;
        for (int j = 0; j < faces[i].size(); j++) {
            for (int k = 0; k < indices_of_removed_vertices.size(); k++) {
                if (faces[i][j] == indices_of_removed_vertices[k]) { //Should never be triggered - no correction because indices both given starting from 1
                    cout << "Face [" << i << "][" << j << "] is trying to use deleted vertex " << k << endl;
                    exit(0);
                }
            }
            for (int k = 0; k < pre_smoothing_vertices.size(); k++) {
                if (old_vertices[faces[i][j] - 1] == pre_smoothing_vertices[k]) {
                    new_face.push_back(k + 1);
                    break;
                }
            }
        }
        if (new_face.size() != 3) { //Output error - should not be triggered
            Write_List_to_Screen(faces[i]);
            if (new_face.size() == 1)
                cout << "[" << new_face[0] << "]" << endl << endl;
            else if (new_face.size() == 0)
                cout << "No vertices for this face" << endl << endl;
            else
                Write_List_to_Screen(new_face);
        }
        faces[i].clear();
        faces[i].insert(begin(faces[i]), begin(new_face), end(new_face));
    }

    cout << "All faces have had vertex indices updated." << endl;
    cout << "Finished finding common interface." << endl;
}