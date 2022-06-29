#pragma once

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Pool.h>
#include <CherrySoda/Util/String.h>
#include <CherrySoda/Util/STL.h>

#include <map>
#include <vector>


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


#define DECLARE_VIRTUAL_GET_SET(returntype, name) \
virtual returntype CONCAT_STRING(Get, name)()  = 0; \
virtual void CONCAT_STRING(Set, name)(returntype value) = 0 \