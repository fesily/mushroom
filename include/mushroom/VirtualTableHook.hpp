#pragma once
#include <cstdint>
#include <memory>
#include "mockutils/union_cast.hpp"
#include "mockutils/VTUtils.hpp"

namespace mushroom
{
using namespace fakeit;
namespace VirtualTableHook
{
template<typename T>
struct Hook
{
	intptr_t* originalVtPtr;
	intptr_t* hookVtPtr;
	size_t dectorIndex;
	size_t vtSize;
	std::shared_ptr<void> data;

	intptr_t Destructor(int flag)
	{
		T* objPtr = reinterpret_cast<T*>(this);
		Hook* hook = GetOrCreate(objPtr);
		return hook->DestructorImpl(objPtr, flag);
	}
	intptr_t DestructorImpl(T* objPtr, int flag)
	{
		const auto originalPtr = originalVtPtr;
		auto hookPtr = hookVtPtr;
		auto dectorFn = fakeit::union_cast<int(T::*)(int)>(originalPtr[dectorIndex]);
		delete this;
		hookPtr--;
		delete[] hookPtr;
		// reset
		GetVtPtr(objPtr) = originalPtr;
		return (objPtr->*dectorFn)(flag);
	}

	static_assert(std::is_polymorphic_v<T>,"");
	static intptr_t*& GetVtPtr(T* ptr)
	{
		return *reinterpret_cast<intptr_t**>(ptr);
	}
	static Hook* GetOrCreate(T* ptr)
	{
		intptr_t*& vtPtr = GetVtPtr(ptr);
		const size_t dectorIndex = VTUtils::getDestructorOffset<T>();
		const size_t vtSize = VTUtils::getVTSize<T>();
		if (static_cast<intptr_t>(vtPtr[dectorIndex]) != fakeit::union_cast<intptr_t>(&Hook::Destructor))
		{
			auto realSize = vtSize + 1;
			auto copy = new intptr_t[realSize + 1];
			auto newVt = copy + 1;
			std::memcpy(copy, vtPtr - 1, sizeof(intptr_t) * realSize);
			newVt[dectorIndex] = fakeit::union_cast<intptr_t>(&Hook::Destructor);
			newVt[vtSize] = reinterpret_cast<intptr_t>(new Hook<T>{ vtPtr, newVt, dectorIndex, vtSize, nullptr });
			vtPtr = newVt;
			return reinterpret_cast<Hook<T>*>(newVt[vtSize]);
		}
		return reinterpret_cast<Hook<T>*>(vtPtr[vtSize]);
	}
};
template<typename T>
Hook<T>* HookVT(T* ptr)
{
	return Hook<T>::GetOrCreate(ptr);
}
}
}

