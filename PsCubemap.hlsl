Texture2D EquirectMap : register(t0);
SamplerState SamplerDefault : register(s0);

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    EquirectMap.GetDimensions(dim.x, dim.y);
    
    return EquirectMap.Sample(SamplerDefault, pos.xy / dim);
}