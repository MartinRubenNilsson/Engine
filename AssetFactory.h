#pragma once
#include "Singleton.h"
#include <filesystem>
#include <unordered_map>

template <class T>
class AssetFactory : public Singleton<AssetFactory<T>>
{
public:
	using Path = std::filesystem::path;
	using Ptr = std::shared_ptr<const T>;

	template <class... Args>
	Ptr GetAsset(const Path& aPath, Args&&... someArgs)
	{
		auto itr = myAssets.find(aPath);
		if (itr != myAssets.end())
			return itr->second;
		
		Ptr asset = std::make_shared<T>(aPath, std::forward<Args>(someArgs)...);
		if (!asset->operator bool())
			return nullptr; // todo: add warning printout?

		myAssets.emplace(aPath, asset);
		return asset;
	}

	const auto& GetAssets() const { return myAssets; }

private:
	std::unordered_map<Path, Ptr> myAssets{};
};

