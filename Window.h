#pragma once

class Window : public Singleton<Window>
{
public:
	Window();
	Window(WNDPROC);

	void SetIcon(const fs::path&);
	void SetTitle(std::wstring_view);
	std::wstring GetTitle() const;

	RECT GetClientRect() const;

	operator HWND() const;
	explicit operator bool() const;

private:
	using Element = std::remove_pointer_t<HWND>;
	using Deleter = BOOL(HWND);

	std::unique_ptr<Element, Deleter*> myHandle;
};

