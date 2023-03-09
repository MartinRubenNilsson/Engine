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

struct VsOutFullscreen
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

struct PsOutGBuffer
{
    float4 normalDepth  : SV_Target0;
    float4 albedo       : SV_Target1;
    float4 metalRoughAo : SV_Target2;
    uint entity         : SV_Target3;
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

cbuffer ImmutableBuffer : register(b0)
{
    float4x4 CubeFaceViewProjs[6];
}

cbuffer CameraBuffer : register(b1)
{
    float4x4 ViewProj;
    float4x4 InvViewProj;
    float4x4 InvTransView;
    float4 CameraPosition;
}

cbuffer MeshBuffer : register(b2)
{
    float4x4 MeshMatrix;
    float4x4 MeshMatrixInvTrans;
    uint MeshEntity[4];
}

cbuffer LightBuffer : register(b3)
{
    float4 LightColor;      // (r, g, b, [unused])
    float4 LightPosition;   // (x, y, z, 1)
    float4 LightDirection;  // (x, y, z, 0)
    float4 LightParams;     // (range, constant attn, linear attn, quadratic attn)
    float4 LightConeAngles; // (inner, outer, [unused], [unused])
};

/*
* Textures
*/

Texture2D GBufferNormalDepth    : register(t0);
Texture2D GBufferAlbedo         : register(t1);
Texture2D GBufferMetalRoughAo   : register(t2);
Texture2D<uint> GBufferEntity   : register(t3);

Texture2D<float> SSAOTexture : register(t4);
Texture2D LightningTexture   : register(t5);

Texture2D MaterialAlbedo    : register(t10);
Texture2D MaterialNormal    : register(t11);
Texture2D MaterialMetallic  : register(t12);
Texture2D MaterialRoughness : register(t13);
Texture2D MaterialOcclusion : register(t14);

TextureCube EnvironmentMap : register(t20); // Skybox
TextureCube IrradianceMap  : register(t21);  // Diffuse IBL
TextureCube PrefilteredMap : register(t22); // Specular IBL

Texture2D GaussianMap : register(t30);

/*
* Samplers
*/

SamplerState PointSampler     : register(s0);
SamplerState TrilinearSampler : register(s1);
SamplerState GaussianSampler  : register(s2);

/*
* Functions
*/

float3 PackNormal(float3 N)
{
    return N * 0.5 + 0.5;
}

float3 UnpackNormal(float3 N)
{
    return N * 2.0 - 1.0;
}

// To be multiplied by outgoing radiance.
float DistanceAttenuation(float aDistanceToLight)
{
    return 1.f / max(0.01, LightParams.y + aDistanceToLight * (LightParams.z + aDistanceToLight * LightParams.w));
}

float3 ClipToWorld(float3 clipPos)
{
    float4 worldPos = mul(InvViewProj, float4(clipPos, 1.0));
    return worldPos.xyz / worldPos.w;
}

float3 UVDepthToWorld(float2 uv, float depth)
{
    float3 clipPos = { uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, depth };
    return ClipToWorld(clipPos);
}
