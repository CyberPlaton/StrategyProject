#pragma once

#include "NetCommon.h"

#include <CherrySoda/Util/Json.h>
#include <CherrySoda/Util/XML.h>
#include <CherrySoda/Util/String.h>
#include <CherrySoda/Entity.h>
#include <CherrySoda/Scene.h>



namespace cherrysoda
{
	class Factory
	{
	public:
		STATIC_GET_DEL(Factory, g_Factory);
		

		Factory& Begin(Scene* scene)
		{
			m_constructingEntity = new cherrysoda::Entity();
			m_constructingEntity->AddTag(Factory::g_NextEntityId++);
			
			m_scene = scene;
			// Return self for concatening calls.
			return *this;
		}

		Entity* End()
		{
			m_scene->Add(m_constructingEntity);

			auto e = m_constructingEntity;

			m_constructingEntity = nullptr;
			m_scene = nullptr;

			return e;
		}

		template < class... ARGS >
		Factory& Add(ARGS... args)
		{
			m_constructingEntity->Add(args...);
			return *this;
		}

	private:
		static Factory* g_Factory;
		static size_t g_NextEntityId;

		Entity* m_constructingEntity = nullptr;
		Scene* m_scene = nullptr;


	private:
		Factory() = default;
		~Factory() = default;
	};
}