#include "ShaderCommon.hlsli"

[maxvertexcount(18)] // why 18?
void main(triangle float3 input[3] : POSITION, inout TriangleStream<GsOutSkybox> stream)
{
    for (uint v = 0; v < 3; ++v)
    {
        // Set w=z after transformation to ensure that the skybox always lie on the far plane.
        
        GsOutSkybox output;
        output.pos = mul(ViewProj, float4(input[v] + CameraPosition.xyz, 1.f)).xyww;
        output.worldPos = input[v];
#if USE_REVERSE_Z
        output.pos.z = 0.0;
#endif
        stream.Append(output);
    }
}