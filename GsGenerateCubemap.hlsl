struct GsOut
{
	float4 posClip : SV_POSITION;
    float3 posWorld : POSITION;
    uint index : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)] // 18 vertices = 6 faces * 3 vertices
void main(triangle float3 input[3] : POSITION, uint id : SV_PrimitiveID, inout TriangleStream<GsOut> stream)
{
    static const float4 vertices[] = 
    {
        -1.0, -1.0, 0.0, 1.0,
        -1.0,  3.0, 0.0, 1.0,
         3.0, -1.0, 0.0, 1.0,
    };
    
    static const uint arrayIndices[] =
    {
        5, 5, 3, 0, 0, 2, 2, 1, 1, 3, 4, 4
    };
    
    GsOut output;
    output.index = arrayIndices[id];
    
    for (uint v = 0; v < 3; ++v)
    {
        output.posClip = vertices[v];
        output.posWorld = input[v];
        stream.Append(output);
    }
}