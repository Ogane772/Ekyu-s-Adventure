#include "../Framework/ModelView/ViewModel.h"
#include "../Framework/Scene/SceneManager.h"
#include "../Framework/Input/InputInterface.h"
#include "../Framework/Time/Time.h"
#include "../Framework/Sound/SoundManager.h"
#include "../Framework/Renderer/Renderer.h"
#include "../Framework/TextureLoader/TextureLoader.h"
#include "../Framework/Scene/ModelViewMap.h"
#include "../Framework/System/SystemContext.h"
#include "../Framework/Fade/Fade.h"
#include "../Framework/Model/StaticMesh.h"
#include "../Framework/SaveAndLoad/SaveAndLoad.h"
#include "resource.h"
const char* CLASS_NAME = "DX11AppClass";
const char* WINDOW_NAME = "エキューの冒険";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//システムクラスとセーブデータ作成
	CSaveAndLoad* save = CSingleton<CSaveAndLoad>::GetInstance();
	CSystemContext* systemContext = CSingleton<CSystemContext>::GetInstance();
	if (!systemContext)
	{
		return -1;
	}

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		nullptr,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		CLASS_NAME,
		nullptr
	};
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウサイズの計算
	const int windowWidth = static_cast<int>(systemContext->GetDefaultScreenWidth()) + GetSystemMetrics(SM_CXDLGFRAME) * 2;
	const int windowHeight = static_cast<int>(systemContext->GetDefaultScreenHeight()) + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

	// モニター解像度を取得
	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// ウィンドウを画面中央に配置するための座標を計算
	const int windowX = static_cast<int>((screenWidth - windowWidth) * 0.5f);
	const int windowY = static_cast<int>((screenHeight - windowHeight) * 0.5f);

	// ウィンドウの作成
	HWND window = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		windowX,      // 画面中央のX座標
		windowY,      // 画面中央のY座標
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	DragAcceptFiles(window, TRUE);

	systemContext->SetWindow(window);
	//音声初期化
	CSoundManager* soundManager = CSingleton<CSoundManager>::GetInstance();
	if (!soundManager)
	{
		return -1;
	}
	// インプット初期化
	CInputInterface* inputInterface = CSingleton<CInputInterface>::GetInstance();
	if (!inputInterface)
	{
		return -1;
	}
	// テクスチャ読み込みクラス初期化
	CTextureLoader* textureLoader = CSingleton<CTextureLoader>::GetInstance();
	if (!textureLoader)
	{
		return -1;
	}
	// シーン初期化処理
	CSceneManager* sceneManager = CSingleton<CSceneManager>::GetInstance();
	if (!sceneManager)
	{
		return -1;
	}
	sceneManager->Init();

	// ウインドウの表示(初期化処理の後に行う)
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	CTime* time = CTime::GetInstance();
	if (!time)
	{
		return -1;
	}
	time->Initialize(30);
	time->SetTargetFPS(60);

	// メッセージループ
	MSG msg;
	while (1)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (time->Update())
			{
				// 更新処理
				const float deltaTime = time->GetDeltaTime();
				if (inputInterface)
				{
					inputInterface->Update(deltaTime);
				}
				sceneManager->Update(deltaTime);

				// 描画処理
				sceneManager->Draw();

				if (soundManager)
				{
					soundManager->Update();
				}
			}
		}
	}
	//ロード済みモデルを全解放
	CStaticMesh::AllLoadModelDelete();
	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	SingletonFinalizer::Finalize();

	return (int)msg.wParam;
}
//=============================================================================
// ウインドウプロシージャ
//=============================================================================
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	switch(uMsg)
	{
	case WM_SIZE:
	{
		CSystemContext* systemContext = CSystemContext::GetInstance();
		if (!systemContext)
		{
			break;
		}
		systemContext->SetCurrentScreenWidth(LOWORD(lParam));
		systemContext->SetCurrentScreenHeight(HIWORD(lParam));
		// 最小化時はリサイズ処理をスキップ
		if (wParam == SIZE_MINIMIZED)
			break;

		// スワップチェーンが初期化済みならリサイズ処理
		if (CRenderer::GetSwapChain())
		{
			// 既存のレンダーターゲットを破棄してからResizeBuffersを再作成
			CRenderer::CleanupRenderTarget();
			CRenderer::ResizeBuffers(lParam);
			CRenderer::CreateRenderTarget();

			// ImGuiのデバイスオブジェクトを再作成（フォントテクスチャなど）
			// CSceneManager::ResizeWindow はシーン内の ImGui オブジェクトを再初期化する
			if (CSceneManager* sceneManager = CSingleton<CSceneManager>::GetInstance())
			{
				sceneManager->ResizeWindow();
			}
		}
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_DROPFILES:
	{
		if (CSceneManager* sceneManager = CSingleton<CSceneManager>::GetInstance())
		{
			//モデルビューモードの時のみドラッグドロップされたモデルを表示
			std::weak_ptr<CScene> scene = sceneManager->GetScene();
			if (CModelViewMap* modelViewScene = dynamic_cast<CModelViewMap*>(scene.lock().get()))
			{
				HDROP drop = (HDROP)wParam;
				char fileName[MAX_PATH];
				DragQueryFileA(drop, 0, fileName, MAX_PATH);
				if (CViewModel* viewModel = modelViewScene->GetViewModel())
				{
					viewModel->LoadModel(fileName);
				}
			}
		}
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

