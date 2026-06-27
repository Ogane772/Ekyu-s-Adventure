#include "DirectionalLight.h"
#include "../Renderer/Renderer.h"

CDirectionalLight::CDirectionalLight()
{
	m_Light.Direction = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	m_Light.Diffuse = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.Ambient = COLOR(0.3f, 0.3f, 0.3f, 1.0f);
	CRenderer::SetLight(m_Light);
	m_DefaultLight = m_Light;
}

void CDirectionalLight::SetDirection(const XMFLOAT4& Direction)
{
	m_Light.Direction = Direction;
	CRenderer::SetLight(m_Light);
}

void CDirectionalLight::SetDiffuse(const COLOR& Diffuse)
{
	m_Light.Diffuse = Diffuse;
	CRenderer::SetLight(m_Light);
}

void CDirectionalLight::SetAmbient(const COLOR& Ambient)
{
	m_Light.Ambient = Ambient;
	CRenderer::SetLight(m_Light);
}
