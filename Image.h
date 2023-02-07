#pragma once

class Image
{
public:
	Image(const fs::path& aPath, unsigned aDesiredChannels = 0);

	const void* GetData() const	{ return myData.get(); }

	unsigned GetWidth() const			{ return (unsigned)myWidth; }
	unsigned GetHeight() const			{ return (unsigned)myHeight; }
	unsigned GetChannelsInFile() const	{ return (unsigned)myChannelsInFile; }
	unsigned GetChannels() const		{ return myChannels; }

	operator bool() const { return myData.operator bool(); }

private:
	using Element = unsigned char;
	using Deleter = void(void*);

	int myWidth, myHeight, myChannelsInFile;
	std::unique_ptr<Element, Deleter*> myData;
	unsigned myChannels;
};

