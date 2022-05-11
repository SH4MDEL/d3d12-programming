#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"

CScene::CScene(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

CScene::~CScene()
{
}

void CScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	CCubeMesh* pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);

	for (m_RailObjects = 0; m_RailObjects < 13; ++m_RailObjects) {
		XMFLOAT3 Coord(RD::GetRandomfloat(-5.0f, 5.0f), RD::GetRandomfloat(-5.0f, 5.0f), (float)m_RailObjects * 10.0f - 200.0f);
		m_dRailCoordinate.push_back(Coord);
	}

	pRailMesh = new CRailMesh(5.0);

	for (int i = 0; i < 10; ++i) {
		m_dRailManager.push_back(new CRailObject(m_dRailCoordinate[i], m_dRailCoordinate[i + 1], 
			m_dRailCoordinate[i + 2], m_dRailCoordinate[i + 3]));
		m_dRailManager.back()->SetMesh(pRailMesh);
		m_dRailManager.back()->SetColor(RGB(128, 0, 255));
	}
	m_dRailManagerIter = m_dRailManager.begin();
	for (int i = 0; i < 5; i++) {
		++m_dRailManagerIter;
	}
	(*m_dRailManagerIter)->EnableRailObject();

#ifdef _WITH_DRAW_AXIS
	m_pWorldAxis = new CGameObject();
	CAxisMesh* pAxisMesh = new CAxisMesh(0.5f, 0.5f, 0.5f);
	m_pWorldAxis->SetMesh(pAxisMesh);
#endif
}

void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();


#ifdef _WITH_DRAW_AXIS
	if (m_pWorldAxis) delete m_pWorldAxis;
#endif
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':

			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;

	return(pNearestObject);
}

void CScene::Animate(float fElapsedTime)
{
	if ((*m_dRailManagerIter)->isEnableRailObject()) {
		m_pPlayer->SetPosition((*m_dRailManagerIter)->UpdatePlayerPosition(fElapsedTime));
		m_pPlayer->Rotate(((*m_dRailManagerIter)->GetPlayerDirection().y, (*m_dRailManagerIter)->GetPlayerDirection().x, 0.0f));
	}
	else {
		XMFLOAT3 Coord(RD::GetRandomfloat(-5.0f, 5.0f), RD::GetRandomfloat(-5.0f, 5.0f), (float)m_RailObjects++ * 10.0f - 200.0f);
		m_dRailCoordinate.push_back(Coord);
		m_dRailCoordinate.pop_front();

		auto i = m_dRailCoordinate.rbegin();
		m_dRailManager.push_back(new CRailObject(*(i + 3), *(i + 2), *(i + 1), *i));
		m_dRailManager.back()->SetMesh(pRailMesh);
		m_dRailManager.back()->SetColor(RGB(128, 0, 255));
		m_dRailManager.pop_front();
		m_dRailManagerIter = m_dRailManager.begin();
		for (int i = 0; i < 5; i++) {
			++m_dRailManagerIter;
		}

		(*m_dRailManagerIter)->EnableRailObject();
		m_pPlayer->SetPosition((*m_dRailManagerIter)->UpdatePlayerPosition(fElapsedTime));
	}
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);

	for (const auto& elem : m_dRailManager) elem->Render(hDCFrameBuffer, pCamera);

	if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);

//UI
#ifdef _WITH_DRAW_AXIS
	CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
	m_pWorldAxis->SetRotationTransform(&m_pPlayer->m_xmf4x4World);
	m_pWorldAxis->Render(hDCFrameBuffer, pCamera);
#endif
}
