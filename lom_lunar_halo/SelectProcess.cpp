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
	_game->SetWindowAsLogin();
	//_testUI = YX::GUI::LayoutLoader::Parse(_W(_game->GetSetting()->GetUILayoutDir() + "select.xml"));
	string rmlPath = _game->GetSetting()->GetUILayoutDir() + "select.rml";
	_game->GetRmlUiAdapter()->GetContext()->LoadDocument(rmlPath)->Show();
	_state = IProcess::State::Entered;
}

void SelectProcess::StartExit()
{
	//_testUI = nullptr;
	_game->GetRmlUiAdapter()->GetContext()->UnloadAllDocuments();
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
