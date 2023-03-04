#include "CubemapCommon.hlsli"

[maxvertexcount(18)] // why 18?
void main(triangle float3 input[3] : POSITION, inout TriangleStream<GsOutGenCubemap> stream)
{
    for (uint f = 0; f < 6; ++f)
    {
        GsOutGenCubemap output;
        output.targetIndex = f;
        
        for (uint v = 0; v < 3; ++v)
        {
            output.pos = mul(CubeFaceViewProjs[f], float4(input[v], 1.0));
            output.worldPos = input[v];
            stream.Append(output);
        }

        stream.RestartStrip();
    }
}