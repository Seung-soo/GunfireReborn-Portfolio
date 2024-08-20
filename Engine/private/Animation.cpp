#include "..\public\Animation.h"
#include "Channel.h"


CAnimation::CAnimation()
{
}

void CAnimation::Set_EndPlayAcc()
{
	m_PlayTimeAcc = m_Duration;
	for (auto& pChannel : m_Channels)
		pChannel->Set_EndKeyFrame();
}

HRESULT CAnimation::NativeConstruct(const char* pName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_szName, pName);

	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;
	return S_OK;
}

HRESULT CAnimation::Update_TransformMatrices(_uint iType, _double TimeDelta)
{
	// 애니메이션 누적 시간을 갱신
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta;

	if (ANIM_ROOF == iType)
	{
		if (m_PlayTimeAcc >= m_Duration)	// 반복되는 애니메이션일 경우 누적시간이 한 애니메이션의 총 시간 길이보다 커지면
		{
			m_PlayTimeAcc = 0.0;
			m_isFinished = true;
		}
		else								// 아니라면 false로
			m_isFinished = false;
	}
	else if (ANIM_ONETIME == iType)
	{
		if (m_PlayTimeAcc >= m_Duration)	// 바로 끝나는 애니메이션일 경우 바로 끝냄
		{
			m_PlayTimeAcc = m_Duration;
			//m_isFinished = true;
		}
		else								// 아니라면 false로
			m_isFinished = false;
	}

	_vector		vScale, vRotation, vPosition;

	// 현재 내 애니메이션 상태에서 재생시간에 따른 모든 뼈의 상태를 갱신한다.
	for (_uint i = 0; i < m_iNumChannels; i++)	// 뼈의 갯수만큼 루프
	{
		if (true == m_isFinished)
			m_Channels[i]->Set_CurrentKeyFrame(0);	// 애니메이션이 끝나면 현재 키 프레임을 0으로 초기화해준다.

		// 각각의 뼈들이 시간값에 따른 상태값을 표현한 키프레임들을 가져온다.
		const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
		if (nullptr == pKeyFrames)
		{
			MSGBOX("pKeyFrames is nullptr in CAnimation::Update_TransformMatrices");
			return E_FAIL;
		}

		_uint iNumKeyFrameSize = (_uint)pKeyFrames->size();	// 키 프레임의 사이즈를 받아옴

		_uint iCurrentKeyFrameIndex = m_Channels[i]->Get_CurrentKeyFrame();	// 현재 키프레임을 받아옴

		if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrameSize - 1]->Time)	// 누적된 시간이 마지막 키 프레임의 시작 시간을 넘어가면
		{
			vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vScale);		// 마지막 스케일로 고정
			vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vRotation);	// 마지막 회전값으로 고정
			vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vPosition);	// 마지막 포지션으로 고정
		}
		else		// 누적된 시간이 특정 키 프레임 두개 사이에 존재할 때
		{
			while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)	// 애니메이션 플레이 누적 시간이 다음 키 프레임 시작 시간을 넘어갈 때
				m_Channels[i]->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);		// 다음 키 프레임으로 이동 시켜줌

			// 현재 키 프레임에서 몇 퍼센트 진행됐는지 퍼센트수치.
			_double	Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time) / ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

			// 현재와 다음 키프레임들의 트랜스폼 정보들
			_vector	vSourScale, vDestScale;
			_vector	vSourRotation, vDestRotation;
			_vector	vSourPosition, vDestPosition;

			vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
			vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
			vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

			vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
			vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
			vDestPosition= XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

			// 보간 함수. 두 개의 벡터값을 세 번째 인자만큼의 퍼센트 수치로 보간해서 리턴해줌.
			vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
		}

		vPosition = XMVectorSetW(vPosition, 1.f);
		
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale
			, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		m_Channels[i]->Set_TransformationMatrix(TransformationMatrix);
	}
	return S_OK;
}

CAnimation * CAnimation::Create(const char* pName, _double Duration, _double TickPerSecond)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pName, Duration, TickPerSecond)))
	{
		MSGBOX("pInstance->NativeConstruct returned E_FAIL in CAnimation::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto* pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}