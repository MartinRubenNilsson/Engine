float4 main(float4 aPosition : SV_POSITION) : SV_TARGET
{
    if (aPosition.x > aPosition.y)
	    return float4(1.0f, 1.0f, 1.0f, 1.0f);
    else
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
}