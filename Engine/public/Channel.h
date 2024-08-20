#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;
public:
	const vector<KEYFRAME*>*	Get_KeyFrames() const { return &m_KeyFrames; }	// 키프레임들을 반환
	_uint						Get_CurrentKeyFrame() const { return m_iCurrentKeyFrame; }	// 현재 키프레임을 반환
	KEYFRAME*					Get_CurrentKeyFrameInfo() const { return m_KeyFrames[m_iCurrentKeyFrame]; }
	KEYFRAME*					Get_IndexKeyFrameInfo(_uint iIndex) const { return m_KeyFrames[iIndex]; }
	const char*					Get_Name() const { return m_szName; }

	void						Set_CurrentKeyFrame(_uint iKeyFrameIndex) { m_iCurrentKeyFrame = iKeyFrameIndex; }	// 키 프레임을 새로 받아옴
	void						Set_EndKeyFrame() { m_iCurrentKeyFrame = m_iNumKeyFrames - 1;  }	// 키 프레임을 새로 받아옴
	void						Set_TransformationMatrix(_fmatrix TransformationMatrix);	// 
	void						Set_HierarchyNodePtr(class CHierarchyNode* pNode);
public:
	void				Reserve(_uint iNumKeyFrames) {	// 키 프레임 수만큼 저장 공간 할당
		m_KeyFrames.reserve(iNumKeyFrames);
		m_iNumKeyFrames = iNumKeyFrames;
	}
						// 키 프레임 추가
	void				Add_KeyFrame(KEYFRAME* pKeyFrame) { m_KeyFrames.push_back(pKeyFrame); }
public:
	HRESULT				NativeConstruct(const char* pName);
private:
	char						m_szName[MAX_PATH] = "";// 뼈의 이름

	_uint						m_iCurrentKeyFrame = 0;	// 현재 키 프레임
	_uint						m_iNumKeyFrames = 0;	// 키 프레임 수
	vector<KEYFRAME*>			m_KeyFrames;			// 하나의 뼈가 가지고 있는 키 프레임을 저장한 벡터 컨테이너
	typedef vector<KEYFRAME*>	KEYFRAMES;

	class CHierarchyNode*		m_pNode = nullptr;		// 이 뼈의 노드를 저장
public:
	static CChannel*	Create(const char* pName);
	virtual void		Free() override;
};

END