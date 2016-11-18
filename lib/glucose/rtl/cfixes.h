#pragma once

//This macro adds implementation that allows using C++ 11 enum classes 
//naturally as flags, e.g., you can use: Flags flgs = Flags::OneFlag | Flags::NextFlag, 
//instead of basic:  Flags flgs = (Flags)(static_cast<int>(Flags::OneFlag) | static_cast<int>(Flags::NextFlag))
#define ENABLE_ENUMCLASS_FLAGS(T_ENUMCLASS) \
using T_ENUMTYPE##T_ENUMCLASS = std::underlying_type<T_ENUMCLASS>::type;\
inline T_ENUMCLASS operator | (T_ENUMCLASS lhs, T_ENUMCLASS rhs)\
{\
	return (T_ENUMCLASS)(static_cast<T_ENUMTYPE##T_ENUMCLASS>(lhs) | static_cast<T_ENUMTYPE##T_ENUMCLASS>(rhs));\
}\
inline T_ENUMCLASS& operator |= (T_ENUMCLASS& lhs, T_ENUMCLASS rhs)\
{\
	lhs = (T_ENUMCLASS)(static_cast<T_ENUMTYPE##T_ENUMCLASS>(lhs) | static_cast<T_ENUMTYPE##T_ENUMCLASS>(rhs));\
	return lhs;\
}\
inline T_ENUMCLASS operator & (T_ENUMCLASS lhs, T_ENUMCLASS rhs)\
{\
	return (T_ENUMCLASS)(static_cast<T_ENUMTYPE##T_ENUMCLASS>(lhs) & static_cast<T_ENUMTYPE##T_ENUMCLASS>(rhs));\
}\
inline T_ENUMCLASS& operator &= (T_ENUMCLASS& lhs, T_ENUMCLASS rhs)\
{\
	lhs = (T_ENUMCLASS)(static_cast<T_ENUMTYPE##T_ENUMCLASS>(lhs) & static_cast<T_ENUMTYPE##T_ENUMCLASS>(rhs));\
	return lhs;\
}\
inline T_ENUMCLASS operator ^ (T_ENUMCLASS lhs, T_ENUMCLASS rhs)\
{\
	return (T_ENUMCLASS)(static_cast<T_ENUMTYPE##T_ENUMCLASS>(lhs) ^ static_cast<T_ENUMTYPE##T_ENUMCLASS>(rhs));\
}\
inline T_ENUMCLASS& operator ^= (T_ENUMCLASS& lhs, T_ENUMCLASS rhs)\
{\
	lhs = (T_ENUMCLASS)(static_cast<T_ENUMTYPE##T_ENUMCLASS>(lhs) ^ static_cast<T_ENUMTYPE##T_ENUMCLASS>(rhs));\
	return lhs;\
}\
inline T_ENUMCLASS operator ~ (T_ENUMCLASS lhs)\
{\
	return (T_ENUMCLASS)(~static_cast<T_ENUMTYPE##T_ENUMCLASS>(lhs));\
}

template<class... T> void unused(T&&...)
{ }
