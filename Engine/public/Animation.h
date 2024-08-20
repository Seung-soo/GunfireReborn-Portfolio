#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
public:
	void				Reserve(_uint iNumChannels) {	// ���� �Ҵ� �Լ�
		m_Channels.reserve(iNumChannels);
		m_iNumChannels = iNumChannels;
	}

	const vector<class CChannel*>* Get_Channels() const { return &m_Channels; }

	void				Set_PlayAcc() { m_PlayTimeAcc = 0.0; }
	void				Set_PlayAcc(_float fPlayTimeAcc) { m_PlayTimeAcc = fPlayTimeAcc; }
	void				Set_EndPlayAcc();
	_double				Get_PlayAcc() { return m_PlayTimeAcc; }

	_bool				Get_EndAnimation() { return m_PlayTimeAcc == m_Duration; }

	void				Set_Finished(_bool bFinished) { m_isFinished = bFinished; }

	//					�� �߰� �Լ�
	void				Add_Channel(class CChannel* pChannel) { m_Channels.push_back(pChannel); }
public:
	HRESULT				NativeConstruct(const char* pName, _double Duration, _double TickPerSecond);
	HRESULT				Update_TransformMatrices(_uint iType, _double TimeDelta);	// ������ Ʈ�������� �����ϴ� �Լ�
private:
	char				m_szName[MAX_PATH] = "";// �ִϸ��̼� �̸�
	_double				m_Duration = 0.0;		// ��ü ��� �ð�
	_double				m_TickPerSecond = 0.0;	// �ð��� �ִϸ��̼� ��� �ӵ�
	_double				m_PlayTimeAcc = 0.0;	// �ִϸ��̼� ���� �ð�

	_bool				m_isFinished = false;
private:
	_uint							m_iNumChannels = 0;	// �ִϸ��̼��� ����ϴµ� ����ϴ� ���� ����
	vector<class CChannel*>			m_Channels;			// �ִϸ��̼� ����ϴµ� �ʿ��� ������ �����ϴ� �����̳�
	typedef	vector<class CChannel*> CHANNELS;
public:
	static CAnimation*	Create(const char* pName, _double Duration, _double TickPerSecond);
	virtual void		Free() override;
};

END