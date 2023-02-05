float4 main(uint id : SV_VertexID) : SV_POSITION
{
    // https://wallisc.github.io/rendering/2021/04/18/Fullscreen-Pass.html
    float2 uv = float2((id << 1) & 2, id & 2);
    return float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
}