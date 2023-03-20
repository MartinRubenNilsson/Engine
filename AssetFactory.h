#pragma once
#include "Singleton.h"
#include "Debug.h"
#include <filesystem>
#include <unordered_map>

template <class Asset>
class AssetFactory : public Singleton<AssetFactory<Asset>>
{
public:
	using Path = std::filesystem::path;
	using Ptr = std::shared_ptr<const Asset>;

	template <class... Args>
	Ptr GetAsset(Path aPath, Args&&... someArgs)
	{
		if (aPath.is_absolute())
			aPath = fs::relative(aPath);

		auto itr = myAssets.find(aPath);
		if (itr != myAssets.end())
			return itr->second;
		
		Ptr asset = std::make_shared<Asset>(aPath, std::forward<Args>(someArgs)...);
		if (!asset->operator bool())
		{
			Debug::Println(std::format("Error: Failed to load asset: {}", aPath.string()));
			return nullptr;
		}

		myAssets.emplace(aPath, asset);
		return asset;
	}

	const auto& GetAssets() const { return myAssets; }

private:
	std::unordered_map<Path, Ptr> myAssets{};
};

