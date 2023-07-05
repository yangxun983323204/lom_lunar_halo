#include "pch.h"
#include "LoginProcess.h"
#include "../DirectXTK-main/Inc/Keyboard.h"
#include "../mir_lib/MirConst.h"
#include "ProcessManager.h"
#include "Game.h"
#include "GUI/LayoutLoader.h"
#include "StringCodec.hpp"
#include "RmlUiAdapter.h"

using DirectX::Keyboard;

void LoginProcess::StartEnter()
{
	_game->SetWindowAsLogin();
	_testUI = YX::GUI::LayoutLoader::Parse(_W(_game->GetSetting()->GetUILayoutDir() + "login.xml"));
	string rmlPath = _game->GetSetting()->GetUILayoutDir() + "hello_world.rml";
	_game->GetRmlUiAdapter()->GetContext()->LoadDocument(rmlPath)->Show();
	_state = IProcess::State::Entered;
}

void LoginProcess::StartExit()
{
	_testUI.reset();
	_state = IProcess::State::Exited;
}

void LoginProcess::Update(DX::StepTimer const& timer)
{
	if (gKBTracker.IsKeyPressed(Keyboard::Keys::Enter))
	{
		_mgr->StartEnter(_mgr->GetSelectProcess());
	}
}

void LoginProcess::GetWindowSize(int& w, int& h)
{
	w = Mir::LoginLayoutW;
	h = Mir::LoginLayoutH;
}
