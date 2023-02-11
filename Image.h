#pragma once

class Image
{
public:
	Image(const fs::path& aPath, unsigned aDesiredChannels = 0);

	unsigned GetWidth() const			{ return (unsigned)myWidth; }
	unsigned GetHeight() const			{ return (unsigned)myHeight; }
	unsigned GetChannelsInFile() const	{ return (unsigned)myChannelsInFile; }
	unsigned GetChannels() const		{ return myChannels; }

	const void* Data() const { return myData.get(); }

	explicit operator bool() const { return myData.operator bool(); }

private:
	using Element = unsigned char;
	using Deleter = void(void*);

	int myWidth, myHeight, myChannelsInFile;
	std::unique_ptr<Element, Deleter*> myData;
	unsigned myChannels;
};

