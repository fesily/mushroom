#pragma once
#ifndef MUSHROOM_H
#define MUSHROOM_H
#include <mushroom\mockutils\DynamicProxy.hpp>

namespace mushroom
{
using namespace fakeit;
	template<typename Object>
	DynamicProxy<Object> make_dynamic_proxy()
	{
		return {};
	}

	template<uint32_t id,typename C, typename R,typename... Args, typename FnObj>
	bool bind_method(DynamicProxy<C>& proxy, R (C::*fn)(Args...), FnObj&& obj)
	{
		return proxy.BindMethod<id>(fn, std::static_pointer_cast<MethodInvocationHandler<R, Args...>>(obj));
	}
}
#endif // MUSHROOM_H
