#include "ShaderCommon.hlsli"

float4 main(uint id : SV_VertexID) : SV_POSITION
{
    const float4 vertices[3] =
    {
        { -1.0, -1.0, 0.0, 1.0 },
        { -1.0,  3.0, 0.0, 1.0 },
        {  3.0, -1.0, 0.0, 1.0 },
    };
    
    return vertices[id];
}