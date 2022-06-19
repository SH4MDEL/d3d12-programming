//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	// Player's Light
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);

	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	CMissileObject::PrepareExplosion(pd3dDevice, pd3dCommandList);
	CHellicopterObject::PrepareMovePosition();

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.2f, 0.2f, 0.2f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
	//지형을 하나의 격자 메쉬(257x257)로 생성한다. 
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif

	m_pTerrain->SetPosition(0.0f, 0.0f, 0.0f);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();
	
	CGameObject* pGunshipModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Gunship.bin");
	pGunshipModel->SetBoundingBox(pGunshipModel->m_xmOOBB, pGunshipModel);
	pGunshipModel->SetScale(5.0f, 5.0f, 5.0f);
	CGunshipObject* pGunShipObject = nullptr;
	
	CGameObject* pSuperCobraModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SuperCobra.bin");
	pSuperCobraModel->SetBoundingBox(pSuperCobraModel->m_xmOOBB, pSuperCobraModel);
	pSuperCobraModel->SetScale(5.0f, 5.0f, 5.0f);
	CSuperCobraObject* pSuperCobraObject = nullptr;

	CGameObject* pMi24Model = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Mi24.bin");
	pMi24Model->SetBoundingBox(pMi24Model->m_xmOOBB, pMi24Model);
	pMi24Model->SetScale(5.0f, 5.0f, 5.0f);
	CMi24Object* pMi24Object = nullptr;

	int nSelectedObject = 0;

	for (int i = 0; i < 1; ++i) {
		nSelectedObject = RD::GetRandomint(0,2);

		switch (nSelectedObject) {
			case 0:
				pGunShipObject = new CGunshipObject();
				pGunShipObject->SetChild(pGunshipModel, true);
				pGunShipObject->OnInitialize();
				pGunShipObject->SetBoundingBox(pGunShipObject->m_xmOOBB, pGunshipModel);
				pGunShipObject->SetPosition(CHellicopterObject::m_vxmf3MovePosition.front());
				m_lpGameObjects.push_back(pGunShipObject);
				break;
			case 1:
				pSuperCobraObject = new CSuperCobraObject();
				pSuperCobraObject->SetChild(pSuperCobraModel, true);
				pSuperCobraObject->OnInitialize();
				pSuperCobraObject->SetBoundingBox(pSuperCobraObject->m_xmOOBB, pSuperCobraModel);
				pSuperCobraObject->SetPosition(CHellicopterObject::m_vxmf3MovePosition.front());
				m_lpGameObjects.push_back(pSuperCobraObject);
				break;
			case 2:
				pMi24Object = new CMi24Object();
				pMi24Object->SetChild(pMi24Model, true);
				pMi24Object->OnInitialize();
				pMi24Object->SetBoundingBox(pMi24Object->m_xmOOBB, pMi24Model);
				pMi24Object->SetPosition(CHellicopterObject::m_vxmf3MovePosition.front());
				m_lpGameObjects.push_back(pMi24Object);
				break;
		}
		
	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	while (m_lpGameObjects.size()) {
		m_lpGameObjects.back()->Release();
		m_lpGameObjects.pop_back();
	}

	if (m_pTerrain) delete m_pTerrain;

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (const auto& elm : m_lpGameObjects) elm->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (const auto& elm : m_lpGameObjects) {
		elm->Animate(fTimeElapsed, NULL);
	}


	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();

	}

	// 추가적인 처리 함수
	//CheckObjectByObjectCollisions();
	CheckMissileByObjectCollisions();
	CheckObjectArriveEndline();
	CheckPlayerByObjectCollisions();
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
	for (const auto& elm : m_lpGameObjects) {
		elm->UpdateTransform(NULL);
		elm->Render(pd3dCommandList, pCamera);
	}
	if (m_pTerrain) {
		m_pTerrain->UpdateTransform(NULL);
		m_pTerrain->Render(pd3dCommandList, pCamera);
	}
}

void CScene::CheckObjectByObjectCollisions()
{
	for (const auto& elm1 : m_lpGameObjects) {
		CHellicopterObject* Car1 = (CHellicopterObject*)elm1;

		for (const auto& elm2 : m_lpGameObjects) {
			CHellicopterObject* Car2 = (CHellicopterObject*)elm2;
			if (Car1 != Car2) {
				float fDistZ = Car1->GetPosition().z - Car2->GetPosition().z;
				fDistZ = sqrt(fDistZ * fDistZ);
				float fDistX = Car1->GetPosition().x - Car2->GetPosition().x;
				fDistX = sqrt(fDistX * fDistX);

				if (Car1->m_xmOOBB.Intersects(Car2->m_xmOOBB)){
					Car1->m_bIsCollide = true;
					Car2->m_bIsCollide = true;

					float fDistance = Car1->GetPosition().x - Car2->GetPosition().x;
					float fLength = sqrt(fDistance * fDistance);

					// 차량이 좌우 비중을 두고 박았는지, 정면으로 박았는지 확인
					// fLength 가 일정값보다 크면 좌우로 튕겨나감
					if (fLength >= 15) {
						// 우측에서 충돌 -> 차량은 오른쪽으로 튕겨나감
						if (fDistance > 0) {
							Car1->m_xmCollidedShift = XMFLOAT3(0.02f, 0.0f, -0.01f);
							Car2->m_xmCollidedShift = XMFLOAT3(-0.02f, 0.0f, 0.01f);
						}
						// 좌측에서 충돌 -> 차량은 왼쪽으로 튕겨나감
						else {
							Car1->m_xmCollidedShift = XMFLOAT3(-0.02f, 0.0f, -0.01f);
							Car2->m_xmCollidedShift = XMFLOAT3(0.02f, 0.0f, 0.01f);
						}
					}
					// 정면으로 충돌 -> 차량은 뒤로만 튕겨나감
					else if(Car1->GetPosition().z < Car2->GetPosition().z) {
						Car1->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, -0.06f);
						Car2->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, 0.1f);
					}
					else {
						Car1->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, 0.1f);
						Car2->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, -0.06f);
					}

				}
			}
		}
	}
}

void CScene::CheckMissileByObjectCollisions()
{
	CAirplanePlayer* Player = (CAirplanePlayer*)m_pPlayer;

	for (int i = 0; i < MAX_LAUNCH_MISSILE; ++i) {
		if (Player->m_pMissileObject[i].m_bIsShooted && !Player->m_pMissileObject[i].m_bBlowingUp) {
			for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
				CHellicopterObject* Hellicopter = (CHellicopterObject*)*iter;
				if (Hellicopter->m_xmOOBB.Intersects(Player->m_pMissileObject[i].m_xmOOBB)) {
					Player->m_pMissileObject[i].ExploseMissile();
					m_lpGameObjects.erase(iter);
					break;
				}
			}
		}
	}
	
}

void CScene::CheckObjectArriveEndline()
{
	for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
		CHellicopterObject* Hellicopter = (CHellicopterObject*)*iter;
		if (Hellicopter->m_iPosition + 1 == Hellicopter->m_vxmf3MovePosition.size()) {
			m_lpGameObjects.erase(iter);
			break;
		}
	}
}

void CScene::CheckPlayerByObjectCollisions()
{
	CAirplanePlayer* Player = (CAirplanePlayer*)m_pPlayer;
	for (const auto& elm : m_lpGameObjects) {
		CHellicopterObject* Hellicopter = (CHellicopterObject*)elm;
		if (Hellicopter->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB)) {
			//Hellicopter->ResetPosition();
			//Player->ResetPosition();
			cout << "collide" << endl;
		}
		cout << " " << endl;
	}
}

//void CScene::CheckPlayerByObjectCollisions()
//{
//	
//	for (const auto& elm : m_lpGameObjects) {
//		CHellicopterObject* Car = (CHellicopterObject*)elm;
//		
//		if (Car->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB)) {
//			m_pPlayer->m_bIsCollide = true;
//			Car->m_bIsCollide = true;
//			
//			XMFLOAT3 xmPlayerVelocity{ m_pPlayer->GetVelocity() };
//			m_pPlayer->SetVelocity(XMFLOAT3(0.0f, xmPlayerVelocity.y, 200.0f));
//				
//			float fDistance = m_pPlayer->m_xmOOBB.Center.x - Car->m_xmOOBB.Center.x;
//			float fLength = sqrt(fDistance * fDistance);
//
//			// 차량이 좌우 비중을 두고 박았는지, 정면으로 박았는지 확인
//			// fLength 가 일정값보다 크면 좌우로 튕겨나감
//			if (fLength >= 15) {
//				// 우측에서 충돌 -> 플레이어는 오른쪽으로 튕겨나감
//				if (fDistance > 0) {
//					m_pPlayer->m_xmCollidedShift = XMFLOAT3(0.05f, 0.0f, -0.1f);
//					Car->m_xmCollidedShift = XMFLOAT3(-0.02f, 0.0f, 0.01f);
//				}
//				// 좌측에서 충돌 -> 플레이어는 왼쪽으로 튕겨나감
//				else {
//					m_pPlayer->m_xmCollidedShift = XMFLOAT3(-0.05f, 0.0f, -0.1f);
//					Car->m_xmCollidedShift = XMFLOAT3(0.02f, 0.0f, 0.01f);
//				}
//			}
//			// 정면으로 충돌 -> 플레이어는 뒤로만 튕겨나감
//			else if(m_pPlayer->m_xmOOBB.Center.z < Car->m_xmOOBB.Center.z) {
//				m_pPlayer->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, -0.1f);
//				Car->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, 0.01f);
//			}		
//			// 후면에서 충돌 -> 플레이어는 앞으로 튕겨나감
//			else {
//				m_pPlayer->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, 0.3f);
//				Car->m_xmCollidedShift = XMFLOAT3(0.0f, 0.0f, -0.01f);
//			}
//		}
//
//	}
//}