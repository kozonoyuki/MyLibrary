#include "CommonDirectX.h"
#include "resource.h"

HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;

//--------------------------------------------------------------------------------------
// �O���錾
//--------------------------------------------------------------------------------------
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//--------------------------------------------------------------------------------------
// �v���O�����̃G���g���|�C���g�B�S�Ă����������A���b�Z�[�W�̏����Ɉڍs����B
// ���[�v���s���A�A�C�h�����Ԃ̓V�[���������_�����O���邽�߂Ɏg�p�B
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//DirectX�C���X�^���X���쐬
	CommonDirectX* directX = new CommonDirectX();

	int width = 640;	// ��ʉ���
	int height = 480;	// ��ʏc��
	// DirectX�������� �ł��Ȃ���Ή��
	if (FAILED(directX->Init(hInstance, nCmdShow, WndProc, width, height)))
	{
		directX->Release(); // �ł��Ȃ���Ή��
		return 0;
	}
	DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 25.0f, -15.0f, 0.0f);	// ���_�ʒu
	DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);	// ���_�����ʒu
	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	// ���_������x�N�g��
	directX->SetCameraPosition(Eye, At, Up, DirectX::XM_PIDIV2, width / (FLOAT)height, 0.01f, 50.0f); // �J�����ʒu��ݒ�
	
	// ���f���t�@�C���p�X
	const char* filename = "NINJA_Slime.fbx";
	// ���f���e�N�X�`���p�X
	//const char* texturename = "Robert.dds";
	// �V�F�[�_�p�X
	const char* shadername = "CommonShader.fx";
	// ���f���C���X�^���X���쐬
	std::shared_ptr<CommonModel> model(directX->LoadModel(filename, shadername));
	std::shared_ptr<CommonModel> floor(directX->LoadModel("floor.fbx", "CommonShader.fx"));
	std::vector<Vertex> vertex = model->GetVertex(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	std::shared_ptr<Line> line(directX->LoadLine());
	//float rad = 0;
	// ���C�����b�Z�[�W���[�v
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
			directX->DrawStart();	// �`��̂͂��߂Ɏ��s
			// ��ڂ̃��f����ݒu
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

			directX->DrawEnd();		// �`��̏I���Ɏ��s�@�����ŉ�ʏ�ɕ`�悳���
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
// �A�v���P�[�V���������b�Z�[�W����M���邽�тɌĂяo�����B
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