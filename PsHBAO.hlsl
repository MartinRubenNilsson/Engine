#include "ShaderCommon.hlsli"
// https://www.derschmale.com/2013/12/20/an-alternative-implementation-for-hbao-2/

//static const float2 sampleDirections[32];
//static const float ditherScale; // the ratio between the render target size and the dither texture size. Normally: renderTargetResolution / 4

static const uint numRays = 8;
static const uint maxStepsPerRay = 5;
static const float strengthPerRay = 0.1875; // strength / numRays, strength = 1.5
static const float halfSampleRadius = 0.25; // sampleRadius / 2, sampleRadius = 0.5
static const float fallOff = 2.0; // the maximum distance to count samples
static const float bias = 0.03; // minimum factor to start counting occluders

float2 CreateUnitVector(float angle)
{
    float s, c;
    sincos(angle, s, c);
    return float2(c, s);
}

float2x2 CreateRotationMatrix(float angle)
{
    float s, c;
    sincos(angle, s, c);
    return float2x2(c, -s, s, c);
}

//// Unproject a value from the depth buffer to the Z value in view space.
//// Multiply the result with an interpolated frustum vector to get the actual view-space coordinates
//float DepthToViewZ(float depthValue)
//{
//    return Proj[3][2] / (depthValue - Proj[2][2]);
//}

// snaps a uv coord to the nearest texel centre
float2 SnapToTexel(float2 uv, float2 maxScreenCoords)
{
    return round(uv * maxScreenCoords) * rcp(maxScreenCoords);
}

//// rotates a sample direction according to the row-vectors of the rotation matrix
//float2 Rotate(float2 vec, float2 rotationX, float2 rotationY)
//{
//    float2 rotated;
//	// just so we can use dot product
//    float3 expanded = float3(vec, 0.0);
//    rotated.x = dot(expanded.xyz, rotationX.xyy);
//    rotated.y = dot(expanded.xyz, rotationY.xyy);
//    return rotated;
//}

//// Gets the view position for a uv coord
//float3 GetViewPosition(float2 uv, float2 frustumDiff)
//{
//    float depth = depthTexture.SampleLevel(sourceSampler, uv, 0);
//    float3 frustumVector = float3(viewFrustumVectors[3].xy + uv * frustumDiff, 1.0);
//    return frustumVector * DepthToViewZ(depth);
//}

// Retrieves the occlusion factor for a particular sample
// uv: the centre coordinate of the kernel
// centerViewPos: The view space position of the centre point
// centerNormal: The normal of the centre point
// tangent: The tangent vector in the sampling direction at the centre point
// topOcclusion: The maximum cos(angle) found so far, will be updated when a new occlusion segment has been found
float GetSampleOcclusion(float2 uv, float3 centerViewPos, float3 centerNormal, float3 tangent, inout float topOcclusion)
{
	// reconstruct sample's view space position based on depth buffer and camera proj
    float sampleDepth = GBufferNormalDepth.Sample(NormalDepthSampler, uv).w;
    float3 sampleViewPos = UVDepthToViewPos(uv, sampleDepth);

	// get occlusion factor based on candidate horizon elevation
    float3 horizonVector = sampleViewPos - centerViewPos;
    float horizonVectorLength = length(horizonVector);
	
    float occlusion;

	// If the horizon vector points away from the tangent, make an estimate
    if (dot(tangent, horizonVector) < 0.0)
        return 0.5;
    else
        occlusion = dot(centerNormal, horizonVector) / horizonVectorLength;
	
	// this adds occlusion only if angle of the horizon vector is higher than the previous highest one without branching
    float diff = max(occlusion - topOcclusion, 0.0);
    topOcclusion = max(occlusion, topOcclusion);

	// attenuate occlusion contribution using distance function 1 - (d/f)^2
    float distanceFactor = saturate(horizonVectorLength / fallOff);
    distanceFactor = 1.0 - distanceFactor * distanceFactor;
    return diff * distanceFactor;
}

// Retrieves the occlusion for a given ray
// origin: The uv coordinates of the ray origin (the AO kernel centre)
// direction: The direction of the ray
// jitter: The random jitter factor by which to offset the start position
// maxScreenCoords: The maximum screen position (the texel that corresponds with uv = 1)
// projectedRadii: The sample radius in uv space
// numStepsPerRay: The amount of samples to take along the ray
// centerViewPos: The view space position of the centre point
// centerNormal: The normal of the centre point
float GetRayOcclusion(float2 renderTargetResolution, float2 origin, float2 direction, float jitter, float2 maxScreenCoords, float2 projectedRadii, uint numStepsPerRay, float3 centerViewPos, float3 centerNormal)
{
	// calculate the nearest neighbour sample along the direction vector
    float2 texelSizedStep = direction * rcp(renderTargetResolution);
    direction *= projectedRadii;
	
	// gets the tangent for the current ray, this will be used to handle opposing horizon vectors
	// Tangent is corrected with respect to per-pixel normal by projecting it onto the tangent plane defined by the normal
   
    
    float depth = GBufferNormalDepth.Sample(NormalDepthSampler, origin + texelSizedStep).w;
    float3 tangent = UVDepthToViewPos(origin + texelSizedStep, depth) - centerViewPos;
    tangent -= dot(centerNormal, tangent) * centerNormal;
	
	// calculate uv increments per marching step, snapped to texel centres to avoid depth discontinuity artefacts
    float2 stepUV = SnapToTexel(direction.xy / (numStepsPerRay - 1), maxScreenCoords);
	
	// jitter the starting position for ray marching between the nearest neighbour and the sample step size
    float2 jitteredOffset = lerp(texelSizedStep, stepUV, jitter);
    float2 uv = SnapToTexel(origin + jitteredOffset, maxScreenCoords);

	// top occlusion keeps track of the occlusion contribution of the last found occluder.
	// set to bias value to avoid near-occluders
    float topOcclusion = bias;
    float occlusion = 0.0;
    
    const uint steps = min(maxStepsPerRay, numStepsPerRay);
    
    for (uint step = 0; step < steps; ++step)
    {
        occlusion += GetSampleOcclusion(uv, centerViewPos, centerNormal, tangent, topOcclusion);
        uv += stepUV;
    }

    return occlusion;
}

float main(VsOutFullscreen input) : SV_TARGET
{
    const float4 centerNormalDepth = GBufferNormalDepth.Sample(NormalDepthSampler, input.uv);
    const float centerDepth = centerNormalDepth.w;
    if (centerDepth == FAR_Z)
        return 1.0; // at far plane -> no occlusion -> full access
    
    const float3 centerViewPos = UVDepthToViewPos(input.uv, centerDepth);
    const float3 centerWorldNormal = normalize(UnpackNormal(centerNormalDepth.xyz));
    const float3 centerViewNormal = mul((float3x3)InvView, centerWorldNormal);
    
    // The maximum screen position (the texel that corresponds with uv = 1), used to snap to texels
	// (normally, this would be passed in as a constant)
    uint2 dim;
    OcclusionMap.GetDimensions(dim.x, dim.y);
    float2 renderTargetResolution = dim;
    float2 maxScreenCoords = renderTargetResolution - 1.0;
	
	// Get the random factors and construct the row vectors for the 2D matrix from cos(a) and -sin(a) to rotate the sample directions
    float2 randomFactors = GetUniformReal(input.pos.xy);
	
	// scale the sample radius perspectively according to the given view depth (becomes ellipse)
    //float w = centerViewPos.z * Proj[2][3] + Proj[3][3];
    //float2 projectedRadii = halfSampleRadius * float2(Proj[1][1], Proj[2][2]) / w; // half radius because projection ([-1, 1]) -> uv ([0, 1])
    //float screenRadius = projectedRadii.x * renderTargetResolution.x;
    
    //float w = centerViewPos.z * Proj[2][1] + Proj[2][2];
    float w = centerViewPos.z;
    float2 projectedRadii = halfSampleRadius * float2(Proj[0][0], Proj[1][1]) / w; // half radius because projection ([-1, 1]) -> uv ([0, 1])
    float screenRadius = projectedRadii.x * renderTargetResolution.x;
    
    if (screenRadius < 0.5)
        return 0.5;
    
	// bail out if there's nothing to march
    if (screenRadius < 1.0)
        return 1.0;

	// do not take more steps than there are pixels		
    uint numStepsPerRay = min(maxStepsPerRay, screenRadius);
	
    float totalOcclusion = 0.0;

    for (uint i = 0; i < numRays; ++i)
    {
        float2 sampleDir = CreateUnitVector(PI2 * i / numRays);
        sampleDir = mul(CreateRotationMatrix(PI2 * randomFactors.x), sampleDir);
        totalOcclusion += GetRayOcclusion(renderTargetResolution, input.uv, sampleDir, randomFactors.y, maxScreenCoords, projectedRadii, numStepsPerRay, centerViewPos, centerViewNormal);
    }
    
    return 1.0 - saturate(strengthPerRay * totalOcclusion);
}
