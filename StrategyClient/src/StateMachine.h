#pragma once

#include <CherrySoda/Util/String.h>
#include <CherrySoda/Util/STL.h>
#include <CherrySoda/Scene.h>


extern class App;


class StateMachine;
class State
{
	friend class StateMachine;
public:
	State(const cherrysoda::String& name) : m_name(name){}
	virtual ~State(){}

	// To be overriden by implementing class.
	virtual void OnUpdate(){}
	virtual void OnBegin(){}
	virtual void OnEnd(){}
	virtual cherrysoda::Scene* AsScene() { return nullptr; }

	void AddTransition(const cherrysoda::String& name) { m_transitions.push_back(name); }
	cherrysoda::String GetStateName() { return m_name; }


protected:

	cherrysoda::String m_name;
	cherrysoda::STL::Vector<cherrysoda::String> m_transitions;
};




class StateMachine
{
public:
	StateMachine(App* app) : m_currentState(nullptr), m_previousState(nullptr), m_application(app){}
	
	void Transit(const cherrysoda::String& name);
	void AddState(State* state) { m_states.try_emplace(state->GetStateName(), state); }
	void SetInitialState(State* state) { m_currentState = state; }

private:

	App* m_application;

	State* m_currentState;
	State* m_previousState;
	bool m_sceneChanged = false;

	cherrysoda::STL::HashMap<cherrysoda::String, State*> m_states;
};