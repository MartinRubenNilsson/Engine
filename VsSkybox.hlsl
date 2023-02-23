#include "ShaderCommon.hlsli"
#include "Skybox.hlsli"

SkyboxPixel main(uint id : SV_VertexID)
{
    // https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
    // The z-axis has been inverted compared to the answer from stackoverflow, since DX11 is left-handed

    static const float3 skyboxVertices[] =
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
    
    // By setting z=w=1 in pixelPosition, we ensure that the skybox always lie on the far plane 
    
    SkyboxPixel pixel;
    pixel.localPosition = skyboxVertices[id];
    pixel.pixelPosition = mul(CameraViewProjMatrix, float4(pixel.localPosition, 0.f)).xyww;
    
    return pixel;
}