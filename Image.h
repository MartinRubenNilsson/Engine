#pragma once

class Image
{
public:
	Image();
	Image(const fs::path& aPath, unsigned aDesiredChannels = 0);

	unsigned GetWidth() const			{ return myWidth; }
	unsigned GetHeight() const			{ return myHeight; }
	unsigned GetChannelsInFile() const	{ return myChannelsInFile; }
	unsigned GetChannels() const		{ return myChannels; }

	const void* Data() const { return myData.get(); }

	explicit operator bool() const { return myData.operator bool(); }

private:
	using Element = unsigned char;
	using Deleter = void(void*);

	unsigned myWidth{}, myHeight{}, myChannelsInFile{}, myChannels{};
	std::unique_ptr<Element, Deleter*> myData;
};

