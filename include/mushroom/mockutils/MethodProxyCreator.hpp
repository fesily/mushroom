#pragma once
#include <utility>

#include <mushroom\mockutils\VirtualTable.hpp>
#include <mushroom\mockutils\InvocationHandlerCollection.hpp>
#include <mushroom\mockutils\MethodInvocationHandler.hpp>

namespace fakeit {
template<typename C, typename ... baseclasses>
class DynamicProxy;

    template<typename R, typename ... arglist>
    class MethodProxyCreator {
    public:

        template<unsigned int id>
        void* createMethodProxy() {
            return union_cast<void *>(&MethodProxyCreator::template methodProxy < id >);
        }
    protected:
		template<uint32_t id>
		R methodProxy(arglist... args)
		{//当函数内this实指FakeObject
			auto proxy = InvocationHandlerCollection::getInvocationHandlerCollection((void*)this);
			const auto ptr = proxy->getInvocatoinHandlerPtrById(id);
			if (ptr == nullptr)
				throw std::invalid_argument("");
			auto fn = static_cast<MethodInvocationHandler<R, arglist...>*>(ptr);
			return fn->Invoker(std::forward<arglist>(args)...);
		}
    };
}
