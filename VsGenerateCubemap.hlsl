float3 main(uint id : SV_VertexID) : POSITION
{
    static const float3 vertices[] =
    {
        -1.0,  1.0, -1.0, // Front-top-left
         1.0,  1.0, -1.0, // Front-top-right
        -1.0, -1.0, -1.0, // Front-bottom-left
         1.0, -1.0, -1.0, // Front-bottom-right
         1.0, -1.0,  1.0, // Back-bottom-right
         1.0,  1.0, -1.0, // Front-top-right
         1.0,  1.0,  1.0, // Back-top-right
        -1.0,  1.0, -1.0, // Front-top-left
        -1.0,  1.0,  1.0, // Back-top-left
        -1.0, -1.0, -1.0, // Front-bottom-left
        -1.0, -1.0,  1.0, // Back-bottom-left
         1.0, -1.0,  1.0, // Back-bottom-right
        -1.0,  1.0,  1.0, // Back-top-left
         1.0,  1.0,  1.0, // Back-top-right
    };
    
    return vertices[id];
}