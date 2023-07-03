#pragma once
#include "StepTimer.h"

class Game;
class ProcessManager;
class IProcess
{
public:
	enum class State {
		Entering,
		Entered,
		Exiting,
		Exited
	};

	IProcess(Game* game):
		_game{ game }, _state{ State::Exited }, _mgr{}
	{}
	State GetStatus() { return _state; }

	virtual void StartEnter() = 0;
	virtual void StartExit() = 0;
	virtual void Update(DX::StepTimer const& timer) = 0;
	virtual void GetWindowSize(int& w, int& h) = 0;

	virtual void OnAppActivated(){}
	virtual void OnAppDeactivated(){}
	virtual void OnDeviceLost(){}
	virtual void OnDeviceRestored(){}
	virtual void OnDisplayChange(){}
	virtual void OnAppResuming(){}
	virtual void OnAppSuspending(){}
	virtual void OnWindowMoved(){}
	virtual void OnWindowSizeChanged(int,int) {}

protected:
	friend class ProcessManager;
	Game* _game;
	State _state;
	ProcessManager* _mgr;
};

