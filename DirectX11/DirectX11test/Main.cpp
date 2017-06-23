#include "CommonDirectX.h"
#include "resource.h"

HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;

//--------------------------------------------------------------------------------------
// 前方宣言
//--------------------------------------------------------------------------------------
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//--------------------------------------------------------------------------------------
// プログラムのエントリポイント。全てを初期化し、メッセージの処理に移行する。
// ループを行い、アイドル時間はシーンをレンダリングするために使用。
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//DirectXインスタンスを作成
	CommonDirectX* directX = new CommonDirectX();

	int width = 640;	// 画面横幅
	int height = 480;	// 画面縦幅
	// DirectXを初期化 できなければ解放
	if (FAILED(directX->Init(hInstance, nCmdShow, WndProc, width, height)))
	{
		directX->Release(); // できなければ解放
		return 0;
	}
	DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 25.0f, -15.0f, 0.0f);	// 視点位置
	DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);	// 視点注視位置
	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	// 視点上方向ベクトル
	directX->SetCameraPosition(Eye, At, Up, DirectX::XM_PIDIV2, width / (FLOAT)height, 0.01f, 50.0f); // カメラ位置を設定
	
	// モデルファイルパス
	const char* filename = "NINJA_Slime.fbx";
	// モデルテクスチャパス
	//const char* texturename = "Robert.dds";
	// シェーダパス
	const char* shadername = "CommonShader.fx";
	// モデルインスタンスを作成
	std::shared_ptr<CommonModel> model(directX->LoadModel(filename, shadername));
	std::shared_ptr<CommonModel> floor(directX->LoadModel("floor.fbx", "CommonShader.fx"));
	std::vector<Vertex> vertex = model->GetVertex(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	std::shared_ptr<Line> line(directX->LoadLine());
	//float rad = 0;
	// メインメッセージループ
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			directX->DrawStart();	// 描画のはじめに実行
			// 一つ目のモデルを設置
			//float radd = DirectX::XM_PI / 180.0f * rad;
			directX->DrawModel(model.get(), DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(10.0f, 5.0f, 0.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(-10.0f, 5.0f, 0.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(0.0f, 5.0f, 10.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(0.0f, 5.0f, -10.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(10.0f, 5.0f, 10.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(10.0f, 5.0f, -10.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(-10.0f, 5.0f, 10.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			//directX->DrawModel(model.get(), DirectX::XMFLOAT3(-10.0f, 5.0f, -10.0f), DirectX::XMFLOAT3(DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));


			directX->DrawModel(floor.get(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(3 * DirectX::XM_PI / 2, 0.0f, 0.0f), DirectX::XMFLOAT3(25.0f, 25.0f, 25.0f));
			line.get()->SetLineColor(1.0f, 0.0f, 0.0f, 1.0f);
			directX->DrawLine(line.get(), DirectX::XMFLOAT3(-10.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(10.0f, 0.0f, 0.0f));
			line.get()->SetLineColor(0.0f, 1.0f, 0.0f, 1.0f);
			directX->DrawLine(line.get(), DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f));
			line.get()->SetLineColor(0.0f, 0.0f, 1.0f, 1.0f);
			directX->DrawLine(line.get(), DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));

			directX->DrawEnd();		// 描画の終わりに実行　ここで画面上に描画される
		}
		//rad += 0.1;
		//if (rad > 360) rad = 0;
	}

	model.get()->Release();

	floor.get()->Release();

	line.get()->Release();

	delete directX;

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// アプリケーションがメッセージを受信するたびに呼び出される。
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}