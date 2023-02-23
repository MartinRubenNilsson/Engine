struct GsOutCubemap
{
	float4 pos : SV_POSITION;
    uint index : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)] // 18 vertices = 6 faces * 3 vertices
void main(triangle float4 anInput[3] : SV_POSITION, inout TriangleStream<GsOutCubemap> aStream)
{
	for (uint f = 0; f < 6; ++f)
	{
        GsOutCubemap output;
        output.index = f;
        for (uint v = 0; v < 3; ++v)
        {
            output.pos = anInput[v];
            aStream.Append(output);
        }
        aStream.RestartStrip();
    }
}