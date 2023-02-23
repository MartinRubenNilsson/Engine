struct GsOut
{
	float4 pos : SV_POSITION;
    uint index : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)] // 6 * 3
void main(triangle float4 anInput[3] : SV_POSITION, inout TriangleStream<GsOut> aStream)
{
	for (uint f = 0; f < 6; ++f)
	{
        GsOut output;
        output.index = f;
        for (uint v = 0; v < 3; ++v)
        {
            output.pos = anInput[v];
            aStream.Append(output);
        }
        aStream.RestartStrip();
    }
}