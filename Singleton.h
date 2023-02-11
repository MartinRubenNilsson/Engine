#pragma once
#include <cassert>
#include <type_traits>

template <class Derived>
class Singleton
{
public:
	static Derived& Get()
	{
		assert(ourInstance);
		return *ourInstance;
	}

protected:
	Singleton()
	{
		static_assert(std::is_base_of_v<Singleton, Derived>);
		assert(!ourInstance);
		ourInstance = static_cast<Derived*>(this);
	}

	~Singleton()
	{
		ourInstance = nullptr;
	}

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	static Derived* ourInstance;
};

template <class Derived>
Derived* Singleton<Derived>::ourInstance = nullptr;

