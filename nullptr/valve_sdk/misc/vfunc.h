#pragma once

template<typename FuncType>
__forceinline static FuncType call_vfunction(void* ppClass, int index)
{
    int* pVTable = *(int**)ppClass;
    int dwAddress = pVTable[index];
    return (FuncType)(dwAddress);
}

template<unsigned int IIdx, typename TRet, typename ... TArgs>
static auto call_vfunc(void* thisptr, TArgs ... argList) -> TRet
{
    using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
    return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
}

template<typename T>
__forceinline static T vfunc(void* base, int index)
{
	DWORD* vTabella = *(DWORD**)base;
	return (T)vTabella[index];
}

template<typename T, typename ...Args>
constexpr auto call_virtual_method(void* classBase, int index, Args... args) noexcept
{
    return ((*reinterpret_cast<T(__thiscall***)(void*, Args...)>(classBase))[index])(classBase, args...);
}
