#include "pch.h"
#include "ProcessManager.h"

ProcessManager::ProcessManager(Game* game):
	_curr{}, _next{}
{
	_loginProc = std::make_unique<LoginProcess>(game);
	_selectProc = std::make_unique<SelectProcess>(game);
	_gameProc = std::make_unique<GameProcess>(game);
}

void ProcessManager::StartEnter(IProcess* process)
{
	if (!_curr)
	{
		_curr = process;
		_next = nullptr;
		_curr->_mgr = this;
		_curr->StartEnter();
		return;
	}

	_curr->StartExit();
	_next = process;
}

IProcess* ProcessManager::GetCurrentProcess()
{
	return _curr;
}

bool ProcessManager::IsSwitching()
{
	if (_curr == nullptr)
		return false;

	auto state = _curr->GetStatus();
	return state == IProcess::State::Entering || state == IProcess::State::Exiting;
}

IProcess* ProcessManager::GetNextProcess()
{
	return _next;
}

void ProcessManager::Update(DX::StepTimer const& timer)
{
	if (_curr == nullptr)
		return;

	if (_curr->GetStatus() == IProcess::State::Exited) {
		_curr = nullptr;
		StartEnter(_next);
	}

	_curr->Update(timer);
}
