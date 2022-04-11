// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Direct3D와 관련있는 헤더 파일들을 추가한다.
#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

//#include <dxgidebug.h>
#include <DXGIDebug.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

// Import 라이브러리를 링커 속성에서 입력하지 않아도 되도록 추가한다.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

#define FRAME_BUFFER_WIDTH 800
#define FRAME_BUFFER_HEIGHT 600
// 다음을 정의하면 응용 프로그램을 실행할 때 처음부터 전체화면 모드로 실행된다. 
// 그렇지 않으면 실행할 때 윈도우 모드로 실행되고 ‘F9’를 누르면 전체화면 모드로 전환된다.
#define _WITH_SWAPCHAIN_FULLSCREEN_STATE
