#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
public:
	void				Reserve(_uint iNumChannels) {	// 공간 할당 함수
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

	//					뼈 추가 함수
	void				Add_Channel(class CChannel* pChannel) { m_Channels.push_back(pChannel); }
public:
	HRESULT				NativeConstruct(const char* pName, _double Duration, _double TickPerSecond);
	HRESULT				Update_TransformMatrices(_uint iType, _double TimeDelta);	// 뼈들의 트랜스폼을 갱신하는 함수
private:
	char				m_szName[MAX_PATH] = "";// 애니메이션 이름
	_double				m_Duration = 0.0;		// 전체 재생 시간
	_double				m_TickPerSecond = 0.0;	// 시간당 애니메이션 재생 속도
	_double				m_PlayTimeAcc = 0.0;	// 애니메이션 누적 시간

	_bool				m_isFinished = false;
private:
	_uint							m_iNumChannels = 0;	// 애니메이션을 재생하는데 사용하는 뼈의 갯수
	vector<class CChannel*>			m_Channels;			// 애니메이션 재생하는데 필요한 뼈들을 저장하는 컨테이너
	typedef	vector<class CChannel*> CHANNELS;
public:
	static CAnimation*	Create(const char* pName, _double Duration, _double TickPerSecond);
	virtual void		Free() override;
};

END