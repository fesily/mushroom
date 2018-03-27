#pragma once
#include <mushroom\mockutils\Destructible.hpp>
#include <mushroom\mockutils\VirtualTable.hpp>
namespace fakeit {

struct InvocationHandlerCollection {
	static const unsigned int VT_COOKIE_INDEX = 0;

	virtual Destructible *getInvocatoinHandlerPtrById(unsigned int index) = 0;
	virtual ~InvocationHandlerCollection() = default;

	static InvocationHandlerCollection *getInvocationHandlerCollection(void *instance) {
		VirtualTableBase &vt = VirtualTableBase::getVTable(instance);
		InvocationHandlerCollection *invocationHandlerCollection = (InvocationHandlerCollection *)vt.getCookie(
			InvocationHandlerCollection::VT_COOKIE_INDEX);
		return invocationHandlerCollection;
	}
};
}