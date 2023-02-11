#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#pragma warning( push )
#pragma warning( disable : 4244 )
#include <stb/stb_image.h>
#pragma warning( pop )
#include "Image.h"

Image::Image()
	: myData{ nullptr, stbi_image_free }
{
}

Image::Image(const fs::path& aPath, unsigned aDesiredChannels)
	: Image()
{
	int width, height, channelsInFile;

	myData.reset(stbi_load(
		aPath.string().c_str(),
		&width, &height, &channelsInFile,
		static_cast<int>(aDesiredChannels)
	));
	
	if (!myData)
		return;

	myWidth = static_cast<unsigned>(width);
	myHeight = static_cast<unsigned>(height);
	myChannelsInFile = static_cast<unsigned>(channelsInFile);
	myChannels = aDesiredChannels;
}
