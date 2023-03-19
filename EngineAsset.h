#pragma once

enum class EngineAsset
{
	// Textures
	DefaultAlbedo,
	DefaultNormal,
	DefaultMetallic,
	DefaultRoughness,
	DefaultOcclusion,

	// Scenes
	ShapesScene,
};

fs::path GetPath(EngineAsset);

