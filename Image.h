#pragma once

class Image
{
public:
	Image(const fs::path& aPath, int aDesiredChannels = 0);

	const void* GetData() const { return myData.get(); }
	int GetWidth() const { return myWidth; }
	int GetHeight() const { return myHeight; }
	int GetChannelsInFile() const { return myChannelsInFile; }
	int GetChannels() const { return myChannels; }

	operator bool() const { return myData.operator bool(); }

private:
	using Element = unsigned char;
	using Deleter = decltype(stbi_image_free);

	int myWidth, myHeight;
	int myChannelsInFile, myChannels;
	std::unique_ptr<Element, Deleter*> myData;
};

