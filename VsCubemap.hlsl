// https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
// When drawing, set primitive topology to triangle strip and call ID3D11DeviceContext::Draw(14, 0).

static const float3 CubemapVertices[] =
{
    -1.0, +1.0, -1.0, // Front-top-left
    +1.0, +1.0, -1.0, // Front-top-right
    -1.0, -1.0, -1.0, // Front-bottom-left
    +1.0, -1.0, -1.0, // Front-bottom-right
    +1.0, -1.0, +1.0, // Back-bottom-right
    +1.0, +1.0, -1.0, // Front-top-right
    +1.0, +1.0, +1.0, // Back-top-right
    -1.0, +1.0, -1.0, // Front-top-left
    -1.0, +1.0, +1.0, // Back-top-left
    -1.0, -1.0, -1.0, // Front-bottom-left
    -1.0, -1.0, +1.0, // Back-bottom-left
    +1.0, -1.0, +1.0, // Back-bottom-right
    -1.0, +1.0, +1.0, // Back-top-left
    +1.0, +1.0, +1.0, // Back-top-right
};

float3 main(uint id : SV_VertexID) : POSITION
{
    return CubemapVertices[id];
}