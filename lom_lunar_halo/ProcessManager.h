#pragma once
#include "IProcess.h"
#include "LoginProcess.h"
#include "SelectProcess.h"
#include "GameProcess.h"
#include "Game.h"

class ProcessManager
{
public:
	ProcessManager(Game* game);
	void StartEnter(IProcess* process);
	IProcess* GetCurrentProcess();
	bool IsSwitching();
	IProcess* GetNextProcess();
	void Update(DX::StepTimer const& timer);

	inline IProcess* GetLoginProcess() { return _loginProc.get(); }
	inline IProcess* GetSelectProcess() { return _selectProc.get(); }
	inline IProcess* GetGameProcess() { return _gameProc.get(); }
private:
	IProcess* _curr;
	IProcess* _next;

	std::unique_ptr<LoginProcess> _loginProc;
	std::unique_ptr<SelectProcess> _selectProc;
	std::unique_ptr<GameProcess> _gameProc;
};

