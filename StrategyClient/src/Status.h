#ifndef _STATUS_H_
#define _STATUS_H_
#pragma once

#include "ComponentCommon.h"
#include "NetCommon.h"
#include "BTInterface.h"

// Forward Decl.
class sakura::BehaviorTree;

// Forward decl.
struct net::SStatusEffectData;

namespace cherrysoda
{
	/// @brief 
	class IEntityStatusEffect
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


		virtual bool Initialize(const net::SStatusEffectData&, Entity*, sakura::BehaviorTree*) = 0;



		virtual void Terminate() = 0;



		/// @brief Perform an update.
		/// @return True, if this effect stays to the next update; False if it can be removed and deleted.
		virtual bool OnUpdate() = 0;

		/// @brief Retrieve the Status Effect Name.
		/// @return Name.
		virtual String Name() = 0;
	};




	/// @brief Manages the execution and deletion of Status Effects on an Entity.
	// Central query interface for an entities Status Effects and their data.
	// The execution is Turn Based and performed on Turn End.
	// How are duplicate Status Effects handled? -> By Design Decision, they are NOT handled. Meaning duplication and stacking of Status Effects
	// is tolerated, and if we flag to remove a SE which is duplicate, we remove the first one we find, the other one stays unless flagged as to be removed again.
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

		/// @brief Add a Status Effect Object to the Entity.
		/// @param effect Pointer to Status Effect Object.
		void Add(IEntityStatusEffect* effect)
		{
			m_statusEffectsVec.push_back(effect);
		}


		/// @brief Flag (!) a Status Effect to be removed. This may not lead to an actual removal however,
		/// especially if the Status Effect is permanent.
		/// @param effect_name The name of the Status Effect to be removed.
		void Remove(const String& effect_name)
		{
			m_statusEffectsFlaggedToBeRemovedVec.push_back(effect_name);
		}


		/// @brief Whether a SE is flagged to be removed. Meaning it will be checked and if the conditions are met, he will be deleted.
		/// @param effect_name The name of the SE.
		/// @return True, if the given SE if flagged to be removed.
		bool StatusEffectFlaggedToBeRemoved(const String& effect_name)
		{
			return std::find(m_statusEffectsFlaggedToBeRemovedVec.begin(), m_statusEffectsFlaggedToBeRemovedVec.end(), effect_name) != m_statusEffectsFlaggedToBeRemovedVec.end();
		}


		/// @brief Execute all status effects update function on Turn End. 
		/// Removes those that are not needed anymore and remain those that should stay.
		void Update()
		{
			std::vector< IEntityStatusEffect* > remaining_effects;
			std::vector< IEntityStatusEffect* > removing_effects;

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
				_deleteStatusEffect(removing_effects[0]);
				
				removing_effects.erase(removing_effects.begin());
			}
		}



	private:
		static std::vector< CEntityStatusEffectMngr* > g_CEntityStatusEffectMngrVec;
		static size_t g_nextManagerId;

	private:
		size_t m_managerId = 0;
		std::vector< IEntityStatusEffect* > m_statusEffectsVec;

		std::vector< String > m_statusEffectsFlaggedToBeRemovedVec;

	private:
		CEntityStatusEffectMngr(bool active, bool visible) : base(active, visible) {}


		/// @brief Remove SE from Storages and Free its memory.
		/// @param effect The SE Object to be deleted.
		void _deleteStatusEffect(IEntityStatusEffect* effect)
		{
			// Remove SE from storage.
			std::string effect_name = effect->Name();

			int i = 0;
			for (auto& e : m_statusEffectsVec)
			{
				if (effect->Name().compare(e->Name()) == 0)
				{
					m_statusEffectsVec.erase(m_statusEffectsVec.begin() + i);
					break;
				}

				i++;
			}

			// Remove "to be removed" flag.
			auto it = std::find(m_statusEffectsFlaggedToBeRemovedVec.begin(), m_statusEffectsFlaggedToBeRemovedVec.end(), effect_name);
			if(it != m_statusEffectsFlaggedToBeRemovedVec.end())
			{
				m_statusEffectsFlaggedToBeRemovedVec.erase(it);
			}

			// Free SE memory.
			delete effect;
			effect = nullptr;
		}
	};


	__declspec(selectany) std::vector< CEntityStatusEffectMngr* > CEntityStatusEffectMngr::g_CEntityStatusEffectMngrVec;
	__declspec(selectany) size_t CEntityStatusEffectMngr::g_nextManagerId = 1;


}

#endif