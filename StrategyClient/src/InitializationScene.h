#pragma once

#include <CherrySoda/Scene.h>

#include "StateMachine.h"
#include "GamerService.h"


extern class App;
struct SceneImpl;

class InitializationScene : public State
{
public:
	InitializationScene(const cherrysoda::String& name, StateMachine* state_machine, App* app) : State(name)
	{
		state_machine->AddState(this);

		m_impl = new SceneImpl();
		m_impl->m_application = app;
		m_impl->m_initializationComplete = false;
		m_impl->m_stateMachine = state_machine;
	}

	void OnUpdate() override final { m_impl->Update(); }
	void OnBegin() override final { m_impl->Begin(); }
	void OnEnd() override final { m_impl->m_application = nullptr; m_impl->m_initializationComplete = false; }
	cherrysoda::Scene* AsScene() override final { return m_impl; }

	struct SceneImpl : public cherrysoda::Scene
	{
		void Update() override;
		void Begin() override;
		void End() override;

		App* m_application = nullptr;
		bool m_initializationComplete;
		StateMachine* m_stateMachine = nullptr;
	};
private:

	SceneImpl* m_impl = nullptr;
};