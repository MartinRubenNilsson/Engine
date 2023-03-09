#include "ShaderCommon.hlsli"

static const VsOutFullscreen FullscreenVertices[3] =
{
   -1.0, -1.0, 0.0, 1.0, 0.0, +1.0,
   -1.0, +3.0, 0.0, 1.0, 0.0, -1.0,
   +3.0, -1.0, 0.0, 1.0, 2.0, +1.0 ,
};

VsOutFullscreen main(uint id : SV_VertexID)
{
    return FullscreenVertices[id];
}