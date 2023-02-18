float4 main(uint id : SV_VertexID) : SV_POSITION
{
    const float4 vertices[3] =
    {
        { -1, -1, 0, 1 },
        { -1, 3, 0, 1 },
        { 3, -1, 0, 1 },
    };
    
    return vertices[id];
}