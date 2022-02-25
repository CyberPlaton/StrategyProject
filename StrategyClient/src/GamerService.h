#pragma once


#define STATIC_GET_DEL(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS();} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}



class GamerService
{
public:
	// Forward Declaration of Implementation class.
	// The Implementation depends on the platform, but the 
	// functionality is the same everywhere.
	struct Impl;

public:
	STATIC_GET_DEL(GamerService, g_gamerService);

	bool Initialize();
	void Terminate();

private:

	static GamerService* g_gamerService;

	Impl* m_impl = nullptr;


private:
	GamerService() = default;
	GamerService(const GamerService&) = delete;
	GamerService& operator=(const GamerService&) = delete;
	~GamerService() = default;

};