#pragma once
#include <cassert>
#include <type_traits>

template <class Derived>
class Singleton
{
public:
	Singleton();
	~Singleton();

	static Derived& Get();

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	static Derived* ourInstance;
};

template <class Derived>
Derived* Singleton<Derived>::ourInstance = nullptr;

template<class Derived>
inline Singleton<Derived>::Singleton()
{
	static_assert(std::is_base_of_v<Singleton<Derived>, Derived>);
	assert(!ourInstance);
	ourInstance = static_cast<Derived*>(this);
}

template<class Derived>
inline Singleton<Derived>::~Singleton()
{
	ourInstance = nullptr;
}

template<class Derived>
inline Derived& Singleton<Derived>::Get()
{
	assert(ourInstance);
	return *ourInstance;
}
