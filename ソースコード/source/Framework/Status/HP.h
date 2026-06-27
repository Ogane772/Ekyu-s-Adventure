#pragma once

class HP
{
private:
	int m_MaxHP = 0;     // 最大HP
	int m_CurrentHP = 0;// 現在のHP

public:
	int GetMaxHP() const { return m_MaxHP; }
	int GetCurrentHP() const { return m_CurrentHP; }
	void AddDamage(int value) {	m_CurrentHP -= value; NormalizeHP(); }
	void RecoverDamage(int value) { m_CurrentHP += value; NormalizeHP(); }
	void SetMaxHP(int value) { m_MaxHP = value; NormalizeHP(); }
	void SetHP(int value) { m_CurrentHP = value; NormalizeHP(); }
	void NormalizeHP() { if (m_CurrentHP < 0) { m_CurrentHP = 0; } else if (m_CurrentHP > m_MaxHP) { m_CurrentHP = m_MaxHP; } }
	float GetPercent() const {return (float)m_CurrentHP / (float)m_MaxHP;}
	// HPオーバー判定
	bool IsHpPercentOver(float check_percent) const
	{
		float percent = (float)m_CurrentHP / (float)m_MaxHP;

		if (percent * 100.0f >= check_percent)
		{
			return true;
		}

		return false;
	}

};
