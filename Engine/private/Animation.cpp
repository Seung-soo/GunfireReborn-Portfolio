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
	// �ִϸ��̼� ���� �ð��� ����
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta;

	if (ANIM_ROOF == iType)
	{
		if (m_PlayTimeAcc >= m_Duration)	// �ݺ��Ǵ� �ִϸ��̼��� ��� �����ð��� �� �ִϸ��̼��� �� �ð� ���̺��� Ŀ����
		{
			m_PlayTimeAcc = 0.0;
			m_isFinished = true;
		}
		else								// �ƴ϶�� false��
			m_isFinished = false;
	}
	else if (ANIM_ONETIME == iType)
	{
		if (m_PlayTimeAcc >= m_Duration)	// �ٷ� ������ �ִϸ��̼��� ��� �ٷ� ����
		{
			m_PlayTimeAcc = m_Duration;
			//m_isFinished = true;
		}
		else								// �ƴ϶�� false��
			m_isFinished = false;
	}

	_vector		vScale, vRotation, vPosition;

	// ���� �� �ִϸ��̼� ���¿��� ����ð��� ���� ��� ���� ���¸� �����Ѵ�.
	for (_uint i = 0; i < m_iNumChannels; i++)	// ���� ������ŭ ����
	{
		if (true == m_isFinished)
			m_Channels[i]->Set_CurrentKeyFrame(0);	// �ִϸ��̼��� ������ ���� Ű �������� 0���� �ʱ�ȭ���ش�.

		// ������ ������ �ð����� ���� ���°��� ǥ���� Ű�����ӵ��� �����´�.
		const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
		if (nullptr == pKeyFrames)
		{
			MSGBOX("pKeyFrames is nullptr in CAnimation::Update_TransformMatrices");
			return E_FAIL;
		}

		_uint iNumKeyFrameSize = (_uint)pKeyFrames->size();	// Ű �������� ����� �޾ƿ�

		_uint iCurrentKeyFrameIndex = m_Channels[i]->Get_CurrentKeyFrame();	// ���� Ű�������� �޾ƿ�

		if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrameSize - 1]->Time)	// ������ �ð��� ������ Ű �������� ���� �ð��� �Ѿ��
		{
			vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vScale);		// ������ �����Ϸ� ����
			vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vRotation);	// ������ ȸ�������� ����
			vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vPosition);	// ������ ���������� ����
		}
		else		// ������ �ð��� Ư�� Ű ������ �ΰ� ���̿� ������ ��
		{
			while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)	// �ִϸ��̼� �÷��� ���� �ð��� ���� Ű ������ ���� �ð��� �Ѿ ��
				m_Channels[i]->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);		// ���� Ű ���������� �̵� ������

			// ���� Ű �����ӿ��� �� �ۼ�Ʈ ����ƴ��� �ۼ�Ʈ��ġ.
			_double	Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time) / ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

			// ����� ���� Ű�����ӵ��� Ʈ������ ������
			_vector	vSourScale, vDestScale;
			_vector	vSourRotation, vDestRotation;
			_vector	vSourPosition, vDestPosition;

			vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
			vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
			vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

			vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
			vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
			vDestPosition= XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

			// ���� �Լ�. �� ���� ���Ͱ��� �� ��° ���ڸ�ŭ�� �ۼ�Ʈ ��ġ�� �����ؼ� ��������.
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