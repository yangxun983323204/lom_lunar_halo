#pragma once
#include "IProcess.h"
#include "RmlUi/Core.h"

class Game;
class GameProcess :public IProcess
{
public:
	GameProcess(Game* game) :IProcess(game) {}
	// 通过 IProcess 继承
	virtual void StartEnter() override;
	virtual void StartExit() override;
	virtual void Update(DX::StepTimer const& timer) override;
	virtual void GetWindowSize(int& w, int& h) override;

private:
	Rml::ElementDocument* _ui;
};

