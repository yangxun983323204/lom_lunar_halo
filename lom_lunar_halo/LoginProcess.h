#pragma once
#include "IProcess.h"

class Game;
namespace YX {
	namespace GUI {
		class Canvas;
	}
}

class LoginProcess :public IProcess
{
public:
	LoginProcess(Game* game) :IProcess(game) {}
	// 通过 IProcess 继承
	virtual void StartEnter() override;
	virtual void StartExit() override;
	virtual void Update(DX::StepTimer const& timer) override;
	virtual void GetWindowSize(int& w, int& h) override;

private:
	std::shared_ptr<YX::GUI::Canvas> _testUI;
};

