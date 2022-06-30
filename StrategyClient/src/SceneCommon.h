#pragma once

#include <CherrySoda/Scene.h>
#include "StateMachine.h"


extern class App;
struct SceneImpl;


#define DECLARE_SCENE_COMMON(CLASS) \
CLASS(const cherrysoda::String& name, CStateMachine* state_machine, App* app) : State(name) \
{ \
	state_machine->AddState(this); \
	m_impl = new SceneImpl(this);	   \
	m_impl->m_application = app;   \
	m_impl->m_initializationComplete = false; \
	m_impl->m_stateMachine = state_machine; \
} \
CLASS(const char* name, CStateMachine* state_machine, App* app) : State(name) \
{ \
	state_machine->AddState(this); \
	m_impl = new SceneImpl(this);	   \
	m_impl->m_application = app;   \
	m_impl->m_initializationComplete = false; \
	m_impl->m_stateMachine = state_machine; \
} \
void OnUpdate() override final { m_impl->Update(); } \
void OnBegin() override final { m_impl->Begin(); } \
void OnEnd() override final { m_impl->m_application = nullptr; m_impl->m_initializationComplete = false; } \
cherrysoda::Scene* AsScene() override final { return m_impl; } \
struct SceneImpl : public cherrysoda::Scene \
{ \
	SceneImpl(State* s) : m_state(s) {}; \
	void Update() override; \
	void Begin() override; \
	void End() override; \
	State* GetState() { return m_state; } \
\
	App* m_application = nullptr; \
	bool m_initializationComplete; \
	CStateMachine* m_stateMachine = nullptr; \
	State* m_state = nullptr; \
}; \
private: \
	SceneImpl* m_impl = nullptr; \
