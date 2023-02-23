struct GsOut
{
    float4 pos : SV_POSITION;
    uint id : SV_RenderTargetArrayIndex;
};

float4 main(GsOut anInput) : SV_TARGET
{
    if (anInput.pos.x > anInput.pos.y)
	    return float4(1.0f, 1.0f, 1.0f, 1.0f);
    else
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
}