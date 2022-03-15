#include "Factory.h"

namespace cherrysoda
{
	Factory* Factory::g_Factory = nullptr;
	size_t Factory::g_NextEntityId = 1;

	SceneGraphFactory* SceneGraphFactory::g_SceneGraphFactory = nullptr;
}