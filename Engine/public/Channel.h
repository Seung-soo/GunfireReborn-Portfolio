#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;
public:
	const vector<KEYFRAME*>*	Get_KeyFrames() const { return &m_KeyFrames; }	// Ű�����ӵ��� ��ȯ
	_uint						Get_CurrentKeyFrame() const { return m_iCurrentKeyFrame; }	// ���� Ű�������� ��ȯ
	KEYFRAME*					Get_CurrentKeyFrameInfo() const { return m_KeyFrames[m_iCurrentKeyFrame]; }
	KEYFRAME*					Get_IndexKeyFrameInfo(_uint iIndex) const { return m_KeyFrames[iIndex]; }
	const char*					Get_Name() const { return m_szName; }

	void						Set_CurrentKeyFrame(_uint iKeyFrameIndex) { m_iCurrentKeyFrame = iKeyFrameIndex; }	// Ű �������� ���� �޾ƿ�
	void						Set_EndKeyFrame() { m_iCurrentKeyFrame = m_iNumKeyFrames - 1;  }	// Ű �������� ���� �޾ƿ�
	void						Set_TransformationMatrix(_fmatrix TransformationMatrix);	// 
	void						Set_HierarchyNodePtr(class CHierarchyNode* pNode);
public:
	void				Reserve(_uint iNumKeyFrames) {	// Ű ������ ����ŭ ���� ���� �Ҵ�
		m_KeyFrames.reserve(iNumKeyFrames);
		m_iNumKeyFrames = iNumKeyFrames;
	}
						// Ű ������ �߰�
	void				Add_KeyFrame(KEYFRAME* pKeyFrame) { m_KeyFrames.push_back(pKeyFrame); }
public:
	HRESULT				NativeConstruct(const char* pName);
private:
	char						m_szName[MAX_PATH] = "";// ���� �̸�

	_uint						m_iCurrentKeyFrame = 0;	// ���� Ű ������
	_uint						m_iNumKeyFrames = 0;	// Ű ������ ��
	vector<KEYFRAME*>			m_KeyFrames;			// �ϳ��� ���� ������ �ִ� Ű �������� ������ ���� �����̳�
	typedef vector<KEYFRAME*>	KEYFRAMES;

	class CHierarchyNode*		m_pNode = nullptr;		// �� ���� ��带 ����
public:
	static CChannel*	Create(const char* pName);
	virtual void		Free() override;
};

END