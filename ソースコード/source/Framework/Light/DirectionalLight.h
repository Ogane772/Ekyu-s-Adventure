#pragma once
//ディレクショナルライトクラス
#include "../Singleton/Singleton.h"
#include "../Renderer/ShaderStruct.h"

class CDirectionalLight : public CSingleton<CDirectionalLight>
{
private:
	LIGHT m_Light;
	LIGHT m_DefaultLight;//リセット用のライト初期値

public:
	CDirectionalLight();
	LIGHT GetLight() const { return m_Light; }
	LIGHT GetDefaultLight() const { return m_DefaultLight; }
	void SetDirection(const XMFLOAT4& Direction);
	void SetDiffuse(const COLOR& Diffuse);
	void SetAmbient(const COLOR& Ambient);
	void ResetDirection() { m_Light.Direction = m_DefaultLight.Direction; }
	void ResetDiffuse() { m_Light.Diffuse = m_DefaultLight.Diffuse; }
	void ResetAmbient() { m_Light.Ambient = m_DefaultLight.Ambient; }
};