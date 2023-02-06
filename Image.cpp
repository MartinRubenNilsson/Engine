#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#pragma warning( push )
#pragma warning( disable : 4244 )
#include <stb/stb_image.h>
#pragma warning( pop )
#include "Image.h"

Image::Image(const fs::path& aPath, int aDesiredChannels)
	: myWidth{}, myHeight{}
	, myChannelsInFile{}, myChannels{ aDesiredChannels }
	, myData{ stbi_load(
		aPath.string().c_str(),
		&myWidth, &myHeight,
		&myChannelsInFile,
		aDesiredChannels), stbi_image_free }
{
	assert(0 <= aDesiredChannels && aDesiredChannels <= 4);
}
