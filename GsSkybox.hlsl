#include "ShaderCommon.hlsli"

[maxvertexcount(18)] // why 18?
void main(triangle float3 input[3] : POSITION, inout TriangleStream<GsOutSkybox> stream)
{
    for (uint v = 0; v < 3; ++v)
    {
        // By setting z=w=1, we ensure that the skybox always lie on the far plane.
        
        GsOutSkybox output;
        output.pos = mul(CameraViewProj, float4(input[v], 0.f)).xyww;
        output.worldPos = input[v];
        stream.Append(output);
    }
}