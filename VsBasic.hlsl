cbuffer CameraBuffer : register(b0)
{
    float4x4 WorldToClipMatrix;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    return mul(WorldToClipMatrix, pos);
}