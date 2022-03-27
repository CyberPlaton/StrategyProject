#pragma once

#define STATIC_GET_DEL(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS();} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}


#define STATIC_GET_DEL_WITH_INIT_AND_SHUTDOWN(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS(); if(!STATIC_MEMBER->Initialize()){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){STATIC_MEMBER->Terminate(); delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}
