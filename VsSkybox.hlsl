#include "ShaderCommon.hlsli"
#include "CubemapCommon.hlsli"

// https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip

// The primitive topology for this cube is a triangle strip.
static const float3 CubeStrip[] =
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

VsOutSkybox main(uint id : SV_VertexID)
{
    // By setting z=w=1 in pixelPosition, we ensure that the skybox always lie on the far plane.
    
    VsOutSkybox output;
    output.localPosition = CubeStrip[id];
    output.pixelPosition = mul(CameraViewProj, float4(output.localPosition, 0.f)).xyww;
    
    return output;
}