#include "../../includes.h"
#include "../../main/ui/vars.h"


bool IPT::Setup(LONG_PTR Hook)
{
	D3DDEVICE_CREATION_PARAMETERS creationParameters = { };

	while (FAILED(TInterfaces::DirectDevice->GetCreationParameters(&creationParameters)))
		TWinApi::Ke32SleepThread(500);

	hWindow = creationParameters.hFocusWindow;

	if (hWindow == nullptr)
		return false;

	pOldWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hWindow, GWLP_WNDPROC, Hook));

	if (pOldWndProc == nullptr)
		return false;

	return true;
}

void IPT::Restore()
{
	if (pOldWndProc != nullptr)
	{
		SetWindowLongPtrW(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pOldWndProc));
		pOldWndProc = nullptr;
	}

	TInterfaces::InputSystem->EnableInput(true);
}

bool IPT::Process(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (TVariables::bMenuOpen && wParam != TVariables::iMenuKey)
		return false;

	int nKey = 0;
	EKeyState state = EKeyState::NONE;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256U)
		{
			nKey = wParam;
			state = EKeyState::DOWN;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256U)
		{
			nKey = wParam;
			state = EKeyState::UP;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		nKey = VK_LBUTTON;
		state = uMsg == WM_LBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		nKey = VK_RBUTTON;
		state = uMsg == WM_RBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		nKey = VK_MBUTTON;
		state = uMsg == WM_MBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		nKey = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		state = uMsg == WM_XBUTTONUP ? EKeyState::UP : EKeyState::DOWN;
		break;
	default:
		return false;
	}

	if (state == EKeyState::UP && arrKeyState.at(nKey) == EKeyState::DOWN) 
		arrKeyState.at(nKey) = EKeyState::RELEASED;
	else
		arrKeyState.at(nKey) = state;

	return true;
}
