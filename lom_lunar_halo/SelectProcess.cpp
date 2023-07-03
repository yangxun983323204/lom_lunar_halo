#include "pch.h"
#include "SelectProcess.h"
#include "Game.h"
#include "../DirectXTK-main/Inc/Keyboard.h"
#include "../mir_lib/MirConst.h"
#include "ProcessManager.h"
#include "GUI/LayoutLoader.h"
#include "StringCodec.hpp"

using DirectX::Keyboard;

void SelectProcess::StartEnter()
{
	_game->SetWindowSize(Mir::LoginLayoutW, Mir::LoginLayoutH);
	_testUI = YX::GUI::LayoutLoader::Parse(_W(_game->GetSetting()->GetUILayoutDir() + "select.xml"));
	_state = IProcess::State::Entered;
}

void SelectProcess::StartExit()
{
	_testUI = nullptr;
	_state = IProcess::State::Exited;
}

void SelectProcess::Update(DX::StepTimer const& timer)
{
	if (gKBTracker.IsKeyPressed(Keyboard::Keys::Enter))
	{
		_mgr->StartEnter(_mgr->GetGameProcess());
	}
}

void SelectProcess::GetWindowSize(int& w, int& h)
{
	w = Mir::LoginLayoutW;
	h = Mir::LoginLayoutH;
}
