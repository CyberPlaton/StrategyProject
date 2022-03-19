#pragma once

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Pool.h>
#include <CherrySoda/Util/String.h>
#include <CherrySoda/Util/STL.h>

#define CONCAT_STRING(a, b) a##b
#define DECLARE_GET_SET(returntype, name, member) \
returntype CONCAT_STRING(Get, name)() \
{ \
	return member; \
} \
void CONCAT_STRING(Set, name)(returntype value) \
{ \
	member = value; \
} \

#define DECLARE_QUERY(name, member) \
bool CONCAT_STRING(Is, name)()\
{ \
	return member; \
} \