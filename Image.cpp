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
	bool hdr{ stbi_is_hdr(aPath.string().c_str()) != 0 };
	int x, y, comp, req_comp{ static_cast<int>(aDesiredChannels) };

	if (hdr)
		myData.reset(stbi_loadf(aPath.string().c_str(), &x, &y, &comp, req_comp));
	else
		myData.reset(stbi_load(aPath.string().c_str(), &x, &y, &comp, req_comp));
	
	if (!myData)
		return;

	myWidth = static_cast<unsigned>(x);
	myHeight = static_cast<unsigned>(y);
	myChannelsInFile = static_cast<unsigned>(comp);
	myChannels = aDesiredChannels ? aDesiredChannels : myChannelsInFile;
	myHdr = hdr;
}

unsigned Image::GetRowPitch() const
{
	return myWidth * myChannels * (myHdr ? sizeof(float) : sizeof(stbi_uc));
}

const void* Image::Data() const
{
	return myData.get();
}

Image::operator bool() const
{
	return myData.operator bool();
}
