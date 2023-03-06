#include "Constants.hlsli"

/*
* Shader input/output
*/

struct VsInBasic
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT; 
    float3 bitangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct VsOutBasic
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct PsOutGBuffer
{
    float depth : SV_Target0;
    float4 vertexNormal : SV_Target1;
    float4 pixelNormal : SV_Target2;
    float4 albedo : SV_Target3;
    float4 metalRoughAo : SV_Target4;
    uint entity : SV_Target5;
};


struct GsOutGenCubemap
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    uint targetIndex : SV_RenderTargetArrayIndex;
};

struct GsOutSkybox
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
};

/*
* Constant buffers
*/

cbuffer CameraBuffer : register(b0)
{
    float4x4 CameraViewProj;
    float4x4 CameraInvViewProj;
    float4 CameraPosition;
}

cbuffer MeshBuffer : register(b1)
{
    float4x4 MeshMatrix;
    float4x4 MeshMatrixInvTrans;
    uint MeshEntity[4];
}

cbuffer LightBuffer : register(b2)
{
    float4 LightColor;      // (r, g, b, [unused])
    float4 LightPosition;   // (x, y, z, 1)
    float4 LightDirection;  // (x, y, z, 0)
    float4 LightParams; // (range, constant attn, linear attn, quadratic attn)
    float4 LightConeAngles; // (inner, outer, [unused], [unused])
};

cbuffer CubemapBuffer : register(b3)
{
    float4x4 CubeFaceViewProjs[6];
}

/*
* Textures
*/

Texture2D<float> GBufferDepth   : register(t0);
Texture2D GBufferVertexNormal   : register(t1);
Texture2D GBufferPixelNormal    : register(t2);
Texture2D GBufferAlbedo         : register(t3);
Texture2D GBufferMetalRoughAo   : register(t4);
Texture2D<uint> GBufferEntity   : register(t5);

Texture2D MaterialAlbedo    : register(t10);
Texture2D MaterialNormal    : register(t11);
Texture2D MaterialMetallic  : register(t12);
Texture2D MaterialRoughness : register(t13);
Texture2D MaterialOcclusion : register(t14);

TextureCube EnvironmentMap : register(t20); // Skybox
TextureCube IrradianceMap : register(t21);  // Diffuse IBL
TextureCube PrefilteredMap : register(t22); // Specular IBL

/*
* Samplers
*/

SamplerState PointSampler : register(s0);
SamplerState TrilinearSampler : register(s1);

/*
* Functions
*/

// To be multiplied by outgoing radiance.
float DistanceAttenuation(float aDistanceToLight)
{
    return 1.f / max(0.01, LightParams.y + aDistanceToLight * (LightParams.z + aDistanceToLight * LightParams.w));
}

float3 ClipToWorld(float3 clipPos)
{
    float4 worldPos = mul(CameraInvViewProj, float4(clipPos, 1.0));
    return worldPos / worldPos.w;
}

float3 UVDepthToWorld(float2 uv, float depth)
{
    float3 clipPos = { uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, depth };
    return ClipToWorld(clipPos);
}
