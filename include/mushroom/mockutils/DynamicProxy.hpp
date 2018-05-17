/*
 * Copyright (c) 2014 Eran Pe'er.
 *
 * This program is made available under the terms of the MIT License.
 *
 * Created on Mar 10, 2014
 */
#pragma once

#include <functional>
#include <type_traits>
#include <vector>
#include <array>
#include <memory>
#include <map>

#include <mushroom\mockutils\MethodInvocationHandler.hpp>
#include <mushroom\mockutils\VTUtils.hpp>
#include <mushroom\mockutils\FakeObject.hpp>
#include <mushroom\mockutils\MethodProxyCreator.hpp>
#include <mushroom\mockutils\Destructible.hpp>

namespace fakeit {
	struct NullMethod
	{
		void throw_error_no_method()
		{
			throw std::runtime_error("don't init method!");
		}
	};

template<typename C, typename ...baseclasses>
class DynamicProxy : InvocationHandlerCollection
{
	static_assert(std::is_polymorphic<C>::value, "DynamicProxy requires a polymorphic type");
	static_assert(sizeof(C) == sizeof(fakeit::FakeObject<C, baseclasses...>), "This is a problem");
public:
	using fake_t = fakeit::FakeObject<C, baseclasses...>;
private:
	C * object_;
	std::vector<std::shared_ptr<fakeit::Destructible>> methods_;
	std::map<uint32_t,uint32_t> offsets_;
public:
	DynamicProxy()
		:object_(reinterpret_cast<C*>(new fake_t)), methods_(fakeit::VTUtils::getVTSize<C>())
	{
		GetFake().getVirtualTable().
		  setCookie(VT_COOKIE_INDEX, static_cast<InvocationHandlerCollection*>(this));
		GetFake().getVirtualTable().initAll(union_cast<void*>(&NullMethod::throw_error_no_method));
	}
	virtual ~DynamicProxy()
	{
		auto f = std::addressof(GetFake());
		if (f != nullptr)
			delete f;
	}
	fakeit::Destructible* getInvocatoinHandlerPtrById(uint32_t index) override
	{
		const auto iter = offsets_.find(index);
		if (iter == offsets_.end())
			return nullptr;
		return methods_[iter->second].get();
	}
	fake_t& GetFake()const
	{
		return *reinterpret_cast<fake_t*>(object_);
	}
	C& GetInstance() const
	{
		return *object_;
	}
	template<uint32_t id, typename R, typename ... Args>
	bool BindMethod(R(C::* method)(Args...), std::shared_ptr<MethodInvocationHandler<R, Args...>> handler)
	{
		auto ID = id + 1;
		if (offsets_.find(ID) != offsets_.end())
			return false;
		auto offset = fakeit::VTUtils::getOffset(method);
		MethodProxyCreator<R, Args...> creator;
		GetFake().setMethod(offset, creator.template createMethodProxy<id + 1>());
		if (methods_[offset] != nullptr) return false;
		methods_[offset] = std::static_pointer_cast<fakeit::Destructible>(handler);
		offsets_[ID] = offset;
		return true;
	}
	bool BindMethod(std::shared_ptr<MethodInvocationHandler<void>> handler)
	{
		auto offset = fakeit::VTUtils::getDestructorOffset<C>();
		MethodProxyCreator<void> creator;
		GetFake().setDtor(offset, creator.createMethodProxy<0>());
		if (methods_[offset] != nullptr) return false;
		methods_[offset] = std::static_pointer_cast<fakeit::Destructible>(handler);
		offsets_[0] = offset;
		return true;
	}
};
}
