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
	unsigned GetRowPitch() const;

	bool IsHdr() const { return myHdr; }

	const void* Data() const;

	explicit operator bool() const;

private:
	std::unique_ptr<void, void(*)(void*)> myData;
	unsigned myWidth{}, myHeight{}, myChannelsInFile{}, myChannels{};
	bool myHdr{};
};

