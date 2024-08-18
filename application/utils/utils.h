#pragma once

#include <string>
#include <functional>

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/* Before exit the function */
class ScopeExit
{
public:
	template< typename Callback >
	ScopeExit( const Callback& callback ): m_callback(callback){}

	ScopeExit(const ScopeExit&) = delete;
	ScopeExit& operator =(const ScopeExit&) = delete;
	~ScopeExit(){ m_callback(); }
private:

	std::function<void(void)> m_callback;
};


