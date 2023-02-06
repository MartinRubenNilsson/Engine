// https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip

// This triangle strip is front-facing only when viewed from INSIDE the cube.
// If you want to view the cube from the outside, flip the sign of one of the axes.
// The z-axis has been inverted compared to the answer from stackoverflow, since DX11 is left-handed

float3 CubeStrip[] =
{
    -1,  1, -1, // Front-top-left
     1,  1, -1, // Front-top-right
    -1, -1, -1, // Front-bottom-left
     1, -1, -1, // Front-bottom-right
     1, -1,  1, // Back-bottom-right
     1,  1, -1, // Front-top-right
     1,  1,  1, // Back-top-right
    -1,  1, -1, // Front-top-left
    -1,  1,  1, // Back-top-left
    -1, -1, -1, // Front-bottom-left
    -1, -1,  1, // Back-bottom-left
     1, -1,  1, // Back-bottom-right
    -1,  1,  1, // Back-top-left
     1,  1,  1  // Back-top-right
};