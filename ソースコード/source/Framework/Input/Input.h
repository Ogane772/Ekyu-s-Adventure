#pragma once
#include <Windows.h>

class CInput
{
private:
	BYTE m_OldKeyState[256];
	BYTE m_KeyState[256];
	BYTE m_ReleaseKeyState[256];
public:
	CInput();

	void Update();

	bool GetKeyPress(const BYTE KeyCode ) const;
	bool GetKeyTrigger(const BYTE KeyCode ) const;
	bool GetKeyRelease(const BYTE KeyCode ) const;
};
