#include <stdio.h>
#include <memory>
#include "Misc.h"
#include "Graphics/DebugRenderer.h"

DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	// 頂点シェーダー
	{
		// ファイルを開く
		FILE* fp = nullptr;
		fopen_s(&fp, "Shader\\DebugVS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// 頂点シェーダー生成
		HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ピクセルシェーダー
	{
		// ファイルを開く
		FILE* fp = nullptr;
		fopen_s(&fp, "Shader\\DebugPS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// ピクセルシェーダー生成
		HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbMesh);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, constantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 球メッシュ作成
	CreateSphereMesh(device, 1.0f, 16, 16);

	// 円柱メッシュ作成
	CreateCylinderMesh(device, 1.0f, 1.0f, 0.0f, 1.0f, 16, 1);

	//カプセルメッシュ作成
	CreateCapsuleMesh(device, 1.0f, 2.0f, 16, 16);

	//四角メッシュ作成
	CreateBoxMesh(device);

}

// 描画開始
void DebugRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// シェーダー設定
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	//context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// レンダーステート設定
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState.Get(), blendFactor, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	context->RSSetState(rasterizerState.Get());

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;

	// プリミティブ設定
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 球描画
	context->IASetVertexBuffers(0, 1, sphereVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Sphere& sphere : spheres)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(sphere.radius, sphere.radius, sphere.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(sphere.center.x, sphere.center.y, sphere.center.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = sphere.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		context->Draw(sphereVertexCount, 0);
	}
	spheres.clear();

	// 円柱描画
	context->IASetVertexBuffers(0, 1, cylinderVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Cylinder& cylinder : cylinders)
	{
		// ワールドビュープロジェクション行列作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cylinder.radius, cylinder.height, cylinder.radius);
		DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(cylinder.rotation.x);
		DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(cylinder.rotation.y);
		DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(cylinder.rotation.z);
		DirectX::XMMATRIX R = Y * X * Z;
		//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(cylinder.rotation.x, cylinder.rotation.y, cylinder.rotation.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cylinder.position.x, cylinder.position.y, cylinder.position.z);
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = cylinder.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		context->Draw(cylinderVertexCount, 0);
	}
	cylinders.clear();

	context->IASetVertexBuffers(0, 1, capsuleVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Capsule& capsule : capsules)
	{
		// スケール行列 (半径と高さを適用)
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(capsule.radius, capsule.height, capsule.radius);

		// 回転行列 (X, Y, Z 軸の回転を適用)
		DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(capsule.rotation.x);
		DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(capsule.rotation.y);
		DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(capsule.rotation.z);
		DirectX::XMMATRIX R = Y * X * Z;

		//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
		//	capsule.rotation.x, capsule.rotation.y, capsule.rotation.z);

		// 移動行列 (カプセルの中心位置を指定)
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(capsule.position.x, capsule.position.y, capsule.position.z);

		// ワールド行列を作成
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMMATRIX WVP = W * VP;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = capsule.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);
		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

		// カプセルメッシュの描画
		context->IASetVertexBuffers(0, 1, capsuleVertexBuffer.GetAddressOf(), &stride, &offset);
		context->Draw(capsuleVertexCount, 0);
	}
	capsules.clear();

	// 四角形の描画設定
	//context->VSSetShader(vertexShader.Get(), nullptr, 0);
	//context->PSSetShader(pixelShader.Get(), nullptr, 0);
	//context->IASetInputLayout(inputLayout.Get());
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	context->IASetVertexBuffers(0, 1, boxVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(boxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	for (const Box& box : boxes)
	{
		// スケール、回転、移動行列の作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(box.size.x, box.size.y, box.size.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(box.rotation.x, box.rotation.y, box.rotation.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(box.position.x, box.position.y, box.position.z);
		DirectX::XMMATRIX W = S * R * T;

		// ワールド・ビュー・プロジェクション行列
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX WVP = W * V * P;

		// 定数バッファ更新
		CbMesh cbMesh;
		cbMesh.color = box.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);
		context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cbMesh, 0, 0);

		// 描画
		context->DrawIndexed(boxIndexCount, 0, 0);
	}

	boxes.clear();


}

// 球描画
void DebugRenderer::DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	sphere.color = color;
	spheres.emplace_back(sphere);
}

// 円柱描画
void DebugRenderer::DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& rotation)
{
	Cylinder cylinder;
	cylinder.position = position;
	cylinder.radius = radius;
	cylinder.height = height;
	cylinder.color = color;
	cylinder.rotation = rotation;
	cylinders.emplace_back(cylinder);
}

void DebugRenderer::DrawCapsule(const DirectX::XMFLOAT3& position, float height, float radius,
	const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& rotation)
{
	Capsule capsule;
	capsule.position = position;
	capsule.height = height;
	capsule.radius = radius;
	capsule.color = color;
	capsule.rotation = rotation;
	capsules.emplace_back(capsule);
}

void DebugRenderer::DrawBox(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& size,
	const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& rotation)
{
	Box box;
	box.position = position;
	box.size = size;
	box.color = color;
	box.rotation = rotation;
	boxes.emplace_back(box);
}

// 球メッシュ作成
void DebugRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks)
{
	sphereVertexCount = stacks * slices * 2 + slices * stacks * 2;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(sphereVertexCount);

	float phiStep = DirectX::XM_PI / stacks;
	float thetaStep = DirectX::XM_2PI / slices;

	DirectX::XMFLOAT3* p = vertices.get();
	
	for (int i = 0; i < stacks; ++i)
	{
		float phi = i * phiStep;
		float y = radius * cosf(phi);
		float r = radius * sinf(phi);

		for (int j = 0; j < slices; ++j)
		{
			float theta = j * thetaStep;
			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;

			theta += thetaStep;

			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;
		}
	}

	thetaStep = DirectX::XM_2PI / stacks;
	for (int i = 0; i < slices; ++i)
	{
		DirectX::XMMATRIX M = DirectX::XMMatrixRotationY(i * thetaStep);
		for (int j = 0; j < stacks; ++j)
		{
			float theta = j * thetaStep;
			DirectX::XMVECTOR V1 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P1 = DirectX::XMVector3TransformCoord(V1, M);
			DirectX::XMStoreFloat3(p++, P1);

			int n = (j + 1) % stacks;
			theta += thetaStep;

			DirectX::XMVECTOR V2 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P2 = DirectX::XMVector3TransformCoord(V2, M);
			DirectX::XMStoreFloat3(p++, P2);
		}
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * sphereVertexCount);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, sphereVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

// 円柱メッシュ作成
void DebugRenderer::CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks)
{
	cylinderVertexCount = 2 * slices * (stacks + 1) + 2 * slices;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(cylinderVertexCount);

	DirectX::XMFLOAT3* p = vertices.get();

	float stackHeight = height / stacks;
	float radiusStep = (radius2 - radius1) / stacks;

	// vertices of ring
	float dTheta = DirectX::XM_2PI / slices;

	for (int i = 0; i < slices; ++i)
	{
		int n = (i + 1) % slices;

		float c1 = cosf(i * dTheta);
		float s1 = sinf(i * dTheta);

		float c2 = cosf(n * dTheta);
		float s2 = sinf(n * dTheta);

		for (int j = 0; j <= stacks; ++j)
		{
			float y = start + j * stackHeight;
			float r = radius1 + j * radiusStep;

			p->x = r * c1;
			p->y = y;
			p->z = r * s1;
			p++;

			p->x = r * c2;
			p->y = y;
			p->z = r * s2;
			p++;
		}

		p->x = radius1 * c1;
		p->y = start;
		p->z = radius1 * s1;
		p++;

		p->x = radius2 * c1;
		p->y = start + height;
		p->z = radius2 * s1;
		p++;
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * cylinderVertexCount);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, cylinderVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

//カプセルメッシュ作成
void DebugRenderer::CreateCapsuleMesh(ID3D11Device* device, float radius, float height, int slices, int stacks)
{
	// 円柱部分と半球部分を組み合わせた頂点数を計算
	int cylinderVertexCount = 2 * slices * (stacks + 1);
	int hemisphereVertexCount = stacks * slices * 2;
	capsuleVertexCount = cylinderVertexCount + 2 * hemisphereVertexCount;

	// 頂点データを格納するバッファ
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(capsuleVertexCount);
	DirectX::XMFLOAT3* p = vertices.get();

	// === 円柱の頂点を生成 ===
	float stackHeight = height / stacks;
	float thetaStep = DirectX::XM_2PI / slices;
	// === 円柱の頂点を生成 ===
	for (int i = 0; i < slices; ++i)
	{
		float theta = i * thetaStep;
		float nextTheta = (i + 1) % slices * thetaStep;

		float cosTheta = cosf(theta);
		float sinTheta = sinf(theta);

		float cosNextTheta = cosf(nextTheta);
		float sinNextTheta = sinf(nextTheta);

		for (int j = 0; j <= stacks; ++j)
		{
			float y = -height / 2 + j * stackHeight;

			// 現在のスライス
			p->x = radius * cosTheta;
			p->y = y;
			p->z = radius * sinTheta;
			p++;

			// 次のスライス
			p->x = radius * cosNextTheta;
			p->y = y;
			p->z = radius * sinNextTheta;
			p++;
		}
	}
	// === 半球 (上) の頂点を生成 ===
	float phiStep = DirectX::XM_PI / 2 / stacks;

	// === 半球の頂点を生成 (上下2つ) ===
	for (int i = 0; i < stacks; ++i)
	{
		float phi = i * phiStep;
		float nextPhi = (i + 1) * phiStep;

		for (int j = 0; j < slices; ++j)
		{
			float theta = j * thetaStep;
			float nextTheta = (j + 1) % slices * thetaStep;

			// 上半球
			p->x = radius * sinf(phi) * cosf(theta);
			p->y = height / 2 + radius * cosf(phi);
			p->z = radius * sinf(phi) * sinf(theta);
			p++;

			p->x = radius * sinf(nextPhi) * cosf(nextTheta);
			p->y = height / 2 + radius * cosf(nextPhi);
			p->z = radius * sinf(nextPhi) * sinf(nextTheta);
			p++;

			// 下半球
			p->x = radius * sinf(phi) * cosf(theta);
			p->y = -height / 2 - radius * cosf(phi);
			p->z = radius * sinf(phi) * sinf(theta);
			p++;

			p->x = radius * sinf(nextPhi) * cosf(nextTheta);
			p->y = -height / 2 - radius * cosf(nextPhi);
			p->z = radius * sinf(nextPhi) * sinf(nextTheta);
			p++;
		}
	}

	// 頂点バッファの作成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * capsuleVertexCount);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = vertices.get();

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, capsuleVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}
void DebugRenderer::CreateBoxMesh(ID3D11Device* device)
{
	DirectX::XMFLOAT3 vertices[] = {
	{ -0.5f, -0.5f, -0.5f }, // 0: 左下手前
	{  0.5f, -0.5f, -0.5f }, // 1: 右下手前
	{ -0.5f,  0.5f, -0.5f }, // 2: 左上手前
	{  0.5f,  0.5f, -0.5f }, // 3: 右上手前
	{ -0.5f, -0.5f,  0.5f }, // 4: 左下奥
	{  0.5f, -0.5f,  0.5f }, // 5: 右下奥
	{ -0.5f,  0.5f,  0.5f }, // 6: 左上奥
	{  0.5f,  0.5f,  0.5f }  // 7: 右上奥
	};

	uint32_t indices[] = {
		// 前面
		0, 1, 2, 2, 1, 3,
		// 背面
		5, 4, 7, 7, 4, 6,
		// 左側面
		4, 0, 6, 6, 0, 2,
		// 右側面
		1, 5, 3, 3, 5, 7,
		// 上面
		2, 3, 6, 6, 3, 7,
		// 底面
		4, 5, 0, 0, 5, 1
	};

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(vertices);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, boxVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// インデックスバッファ
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(indices);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = indices;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, boxIndexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	boxIndexCount = _countof(indices);
}
