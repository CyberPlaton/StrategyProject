#pragma once

#include "ComponentCommon.h"


namespace cherrysoda
{
	// Forward decl.
	struct SStatusEffectData;


	/// @brief 
	class EntityStatusEffect
	{
	public:
		enum ETimerType
		{
			TT_TURN,			// Status Effect is applied for a number of turns. TimerValue defines for how many turns the SE stays applied.
			
			TT_PERSISTENT,		// Status Effect is persistent until it is removed. TimerValue is ignored.

			TT_REOCCURRING,		// Status Effect is persistent until it is removed, but after the removal condition is not more valid it is re-applied. 
								// TimerValue defines how many turns the condition has to be invalid, in order to re-apply the SE again.
		};
	public:
		/// @brief Pointer to the entity this effect is applied to.
		/// @return Entity Pointer.
		virtual Entity* Self() = 0;

		/// @brief Perform an update.
		/// @return True, if this effect stays to the next update; False if it can be removed and deleted.
		virtual bool OnUpdate() = 0;

		/// @brief Set the data for the status effect.
		/// @param The status effect data from SStatusEffectData.
		virtual void Data(SStatusEffectData*) = 0;

		/// @brief Retrieve the Status Effect Name.
		/// @return Name.
		virtual String Name() = 0;
	};




	/// @brief Manages the execution and deletion of Status Effects on an Entity.
	// The execution is Turn Based and performed on Turn End.
	class CEntityStatusEffectMngr : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(CEntityStatusEffectMngr, Component);

		CEntityStatusEffectMngr() : CEntityStatusEffectMngr(true, false)
		{
			// Try to add self in a free location in static storage.
			for(int i = 0; i < g_CEntityStatusEffectMngrVec.size(); i++)
			{
				if(g_CEntityStatusEffectMngrVec[i] == nullptr)
				{
					g_CEntityStatusEffectMngrVec[i] = this;
					m_managerId = i;
					return;
				}
			}

			// Add self to next location in static storage.
			g_CEntityStatusEffectMngrVec.push_back(this);
			m_managerId = g_nextManagerId++;
		}

		~CEntityStatusEffectMngr()
		{
			// Remove self from static storage.
			g_CEntityStatusEffectMngrVec.erase(g_CEntityStatusEffectMngrVec.begin() + m_managerId);
			g_CEntityStatusEffectMngrVec[m_managerId] = nullptr;
		}

		static void OnTurnEnd()
		{
			for(auto& mngr: g_CEntityStatusEffectMngrVec)
			{
				mngr->Update();
			}
		}

		void Add(EntityStatusEffect* effect)
		{
			m_statusEffectsVec.push_back(effect);
		}
		void Remove(EntityStatusEffect* effect)
		{
			int i = 0;
			for(auto& e: m_statusEffectsVec)
			{
				if(effect->Name().compare(e->Name()) == 0)
				{
					m_statusEffectsVec.erase(m_statusEffectsVec.begin() + i);
					return;
				}

				i++;
			}
		}

		/// @brief Execute all status effects update function on Turn End. 
		/// Removes those that are not needed anymore and remain those that should stay.
		void Update()
		{
			std::vector< EntityStatusEffect* > remaining_effects;
			std::vector< EntityStatusEffect* > removing_effects;

			// Execute and store which should remain and which should be removed.
			for(int i = 0; i < m_statusEffectsVec.size(); i++)
			{
				auto& effect = m_statusEffectsVec[i];

				if(effect->OnUpdate() == true)
				{
					remaining_effects.push_back(m_statusEffectsVec[i]); // Remain.
				}
				else
				{
					removing_effects.push_back(m_statusEffectsVec[i]); // Remove.
				}
			}

			// Remove all Status Effects (But do not delete them!) and re-append remaining.
			m_statusEffectsVec.clear();
			for(int i = 0; i < remaining_effects.size(); i++)
			{
				m_statusEffectsVec.push_back(remaining_effects[i]);
			}

			// Delete the unneeded effects.
			while(removing_effects.size() > 0)
			{
				auto effect = removing_effects[0];
				delete effect; effect = nullptr;
				removing_effects.erase(removing_effects.begin());
			}
		}



	private:
		static std::vector< CEntityStatusEffectMngr* > g_CEntityStatusEffectMngrVec;
		static size_t g_nextManagerId;

	private:
		size_t m_managerId = 0;
		std::vector< EntityStatusEffect* > m_statusEffectsVec;

	private:
		CEntityStatusEffectMngr(bool active, bool visible) : base(active, visible) {}
	};


	__declspec(selectany) std::vector< CEntityStatusEffectMngr* > CEntityStatusEffectMngr::g_CEntityStatusEffectMngrVec;
	__declspec(selectany) size_t CEntityStatusEffectMngr::g_nextManagerId = 1;


#define ENTITY_ADD_STATUS_EFFECT(effect, entity) \
auto mngr = entity->Get< CEntityStatusEffectMngr >(); \
mngr->Add(effect) \

#define ENTITY_REMOVE_STATUS_EFFECT() \
auto mngr = entity->Get< CEntityStatusEffectMngr >(); \
mngr->Remove(effect) \


}