#include <mushroom\mushroom.h>
#include "mushroom/VirtualTableHook.hpp"

struct ITest
{
	virtual ~ITest() = 0;
	virtual void t1() = 0;
	virtual void t2() = 0;
	virtual void t3() = 0;
	virtual void t4() = 0;
	virtual void t5() = 0;
};
template<typename R, typename ... Args>
struct StlFunctionMethodInvocationHandler : mushroom::MethodInvocationHandler<R, Args...>
{
	std::function<R(Args...)> fn;
	R Invoker(Args... args) override
	{
		return fn(std::forward<Args>(args)...);
	}
};
#include <iostream>
void HookVirtualTableTest();
int main()
{
	HookVirtualTableTest();
	using namespace mushroom;
	auto proxy = make_dynamic_proxy<ITest>();
	auto fn = std::make_shared<StlFunctionMethodInvocationHandler<void>>();
	fn->fn = []()
	{
		std::cout << "t5";
	};
	auto fn1 = std::make_shared<StlFunctionMethodInvocationHandler<void>>();
	fn1->fn = []()
	{
		std::cout << "t3";
	};
	bind_method<0>(proxy, &ITest::t5, fn);
	bind_method<1>(proxy, &ITest::t3, fn1);
	auto& intance = proxy.GetInstance();
	intance.t5();
	intance.t3();
	auto& id1 = typeid(ITest);
	auto& id2 = typeid(intance);
	id2.name();
    return 0;
}
struct VT
{
	virtual void t() {}
	virtual ~VT() = default;
};
struct VT1
{
	virtual ~VT1() = default;
	virtual void t1() {}
};
struct VTD : VT, VT1
{
	void t() override {}
};


void HookVirtualTableTest()
{
	int a = 0;
	for (size_t i = 0; i < 999999; i++)
	{
		{
			// this will memory leak ,because  t.~VT() not call virtual table
			VT t;
			auto ptr = &t;
			mushroom::VirtualTableHook::HookVT(ptr);
		}
		VT* t = new VTD;
		mushroom::VirtualTableHook::HookVT(t);
		delete t;
	}
	a = 999;
}