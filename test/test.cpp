#include <mushroom\mushroom.h>

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
int main()
{
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
	bind_method<0>(proxy, &ITest::t3, fn1);
	proxy.GetInstance().t5();
	proxy.GetInstance().t3();
    return 0;
}
