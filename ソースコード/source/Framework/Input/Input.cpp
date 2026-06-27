#include "Input.h"
constexpr int MAX_KEY = 256;// キーの最大数

CInput::CInput()
{
	memset(m_OldKeyState, 0, MAX_KEY);
	memset(m_KeyState, 0, MAX_KEY);
}

void CInput::Update()
{
	memcpy( m_OldKeyState, m_KeyState, MAX_KEY );

	BYTE aKeyState[MAX_KEY];
	// デバイスからデータを取得
	if (GetKeyboardState(aKeyState))
	{
		for (int nCnKey = 0; nCnKey < MAX_KEY; nCnKey++)
		{
			// キートリガー・リリース情報を生成
			m_ReleaseKeyState[nCnKey] = (m_KeyState[nCnKey] ^ aKeyState[nCnKey]) & m_KeyState[nCnKey];

			// キープレス情報を保存
			m_KeyState[nCnKey] = aKeyState[nCnKey];
		}
	}
}

bool CInput::GetKeyPress(const BYTE KeyCode) const
{
	return (m_KeyState[KeyCode] & 0x80);
}

bool CInput::GetKeyTrigger(const BYTE KeyCode) const
{
	return ((m_KeyState[KeyCode] & 0x80) && !(m_OldKeyState[KeyCode] & 0x80));
}

bool CInput::GetKeyRelease(const BYTE KeyCode) const
{
	return (m_ReleaseKeyState[KeyCode] & 0x80) != 0;
}