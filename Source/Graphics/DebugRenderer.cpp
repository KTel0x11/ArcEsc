#include <stdio.h>
#include <memory>
#include "Misc.h"
#include "Graphics/DebugRenderer.h"

DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	// ���_�V�F�[�_�[
	{
		// �t�@�C�����J��
		FILE* fp = nullptr;
		fopen_s(&fp, "Shader\\DebugVS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// �t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// ���_�V�F�[�_�[����
		HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// ���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// �s�N�Z���V�F�[�_�[
	{
		// �t�@�C�����J��
		FILE* fp = nullptr;
		fopen_s(&fp, "Shader\\DebugPS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// �t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// �s�N�Z���V�F�[�_�[����
		HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// �萔�o�b�t�@
	{
		// �V�[���p�o�b�t�@
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

	// �u�����h�X�e�[�g
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

	// �[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ���X�^���C�U�[�X�e�[�g
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

	// �����b�V���쐬
	CreateSphereMesh(device, 1.0f, 16, 16);

	// �~�����b�V���쐬
	CreateCylinderMesh(device, 1.0f, 1.0f, 0.0f, 1.0f, 16, 1);

	//�J�v�Z�����b�V���쐬
	CreateCapsuleMesh(device, 1.0f, 2.0f, 16, 16);

	//�l�p���b�V���쐬
	CreateBoxMesh(device);

}

// �`��J�n
void DebugRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// �V�F�[�_�[�ݒ�
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());

	// �萔�o�b�t�@�ݒ�
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	//context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// �����_�[�X�e�[�g�ݒ�
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState.Get(), blendFactor, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	context->RSSetState(rasterizerState.Get());

	// �r���[�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX VP = V * P;

	// �v���~�e�B�u�ݒ�
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ���`��
	context->IASetVertexBuffers(0, 1, sphereVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Sphere& sphere : spheres)
	{
		// ���[���h�r���[�v���W�F�N�V�����s��쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(sphere.radius, sphere.radius, sphere.radius);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(sphere.center.x, sphere.center.y, sphere.center.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		// �萔�o�b�t�@�X�V
		CbMesh cbMesh;
		cbMesh.color = sphere.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);

		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);
		context->Draw(sphereVertexCount, 0);
	}
	spheres.clear();

	// �~���`��
	context->IASetVertexBuffers(0, 1, cylinderVertexBuffer.GetAddressOf(), &stride, &offset);
	for (const Cylinder& cylinder : cylinders)
	{
		// ���[���h�r���[�v���W�F�N�V�����s��쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cylinder.radius, cylinder.height, cylinder.radius);
		DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(cylinder.rotation.x);
		DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(cylinder.rotation.y);
		DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(cylinder.rotation.z);
		DirectX::XMMATRIX R = Y * X * Z;
		//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(cylinder.rotation.x, cylinder.rotation.y, cylinder.rotation.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cylinder.position.x, cylinder.position.y, cylinder.position.z);
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMMATRIX WVP = W * VP;

		// �萔�o�b�t�@�X�V
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
		// �X�P�[���s�� (���a�ƍ�����K�p)
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(capsule.radius, capsule.height, capsule.radius);

		// ��]�s�� (X, Y, Z ���̉�]��K�p)
		DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(capsule.rotation.x);
		DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(capsule.rotation.y);
		DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(capsule.rotation.z);
		DirectX::XMMATRIX R = Y * X * Z;

		//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
		//	capsule.rotation.x, capsule.rotation.y, capsule.rotation.z);

		// �ړ��s�� (�J�v�Z���̒��S�ʒu���w��)
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(capsule.position.x, capsule.position.y, capsule.position.z);

		// ���[���h�s����쐬
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMMATRIX WVP = W * VP;

		// �萔�o�b�t�@�X�V
		CbMesh cbMesh;
		cbMesh.color = capsule.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);
		context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

		// �J�v�Z�����b�V���̕`��
		context->IASetVertexBuffers(0, 1, capsuleVertexBuffer.GetAddressOf(), &stride, &offset);
		context->Draw(capsuleVertexCount, 0);
	}
	capsules.clear();

	// �l�p�`�̕`��ݒ�
	//context->VSSetShader(vertexShader.Get(), nullptr, 0);
	//context->PSSetShader(pixelShader.Get(), nullptr, 0);
	//context->IASetInputLayout(inputLayout.Get());
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	context->IASetVertexBuffers(0, 1, boxVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(boxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	for (const Box& box : boxes)
	{
		// �X�P�[���A��]�A�ړ��s��̍쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(box.size.x, box.size.y, box.size.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(box.rotation.x, box.rotation.y, box.rotation.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(box.position.x, box.position.y, box.position.z);
		DirectX::XMMATRIX W = S * R * T;

		// ���[���h�E�r���[�E�v���W�F�N�V�����s��
		DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
		DirectX::XMMATRIX WVP = W * V * P;

		// �萔�o�b�t�@�X�V
		CbMesh cbMesh;
		cbMesh.color = box.color;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp, WVP);
		context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cbMesh, 0, 0);

		// �`��
		context->DrawIndexed(boxIndexCount, 0, 0);
	}

	boxes.clear();


}

// ���`��
void DebugRenderer::DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
	Sphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	sphere.color = color;
	spheres.emplace_back(sphere);
}

// �~���`��
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

// �����b�V���쐬
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

	// ���_�o�b�t�@
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

// �~�����b�V���쐬
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

	// ���_�o�b�t�@
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

//�J�v�Z�����b�V���쐬
void DebugRenderer::CreateCapsuleMesh(ID3D11Device* device, float radius, float height, int slices, int stacks)
{
	// �~�������Ɣ���������g�ݍ��킹�����_�����v�Z
	int cylinderVertexCount = 2 * slices * (stacks + 1);
	int hemisphereVertexCount = stacks * slices * 2;
	capsuleVertexCount = cylinderVertexCount + 2 * hemisphereVertexCount;

	// ���_�f�[�^���i�[����o�b�t�@
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(capsuleVertexCount);
	DirectX::XMFLOAT3* p = vertices.get();

	// === �~���̒��_�𐶐� ===
	float stackHeight = height / stacks;
	float thetaStep = DirectX::XM_2PI / slices;
	// === �~���̒��_�𐶐� ===
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

			// ���݂̃X���C�X
			p->x = radius * cosTheta;
			p->y = y;
			p->z = radius * sinTheta;
			p++;

			// ���̃X���C�X
			p->x = radius * cosNextTheta;
			p->y = y;
			p->z = radius * sinNextTheta;
			p++;
		}
	}
	// === ���� (��) �̒��_�𐶐� ===
	float phiStep = DirectX::XM_PI / 2 / stacks;

	// === �����̒��_�𐶐� (�㉺2��) ===
	for (int i = 0; i < stacks; ++i)
	{
		float phi = i * phiStep;
		float nextPhi = (i + 1) * phiStep;

		for (int j = 0; j < slices; ++j)
		{
			float theta = j * thetaStep;
			float nextTheta = (j + 1) % slices * thetaStep;

			// �㔼��
			p->x = radius * sinf(phi) * cosf(theta);
			p->y = height / 2 + radius * cosf(phi);
			p->z = radius * sinf(phi) * sinf(theta);
			p++;

			p->x = radius * sinf(nextPhi) * cosf(nextTheta);
			p->y = height / 2 + radius * cosf(nextPhi);
			p->z = radius * sinf(nextPhi) * sinf(nextTheta);
			p++;

			// ������
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

	// ���_�o�b�t�@�̍쐬
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
	{ -0.5f, -0.5f, -0.5f }, // 0: ������O
	{  0.5f, -0.5f, -0.5f }, // 1: �E����O
	{ -0.5f,  0.5f, -0.5f }, // 2: �����O
	{  0.5f,  0.5f, -0.5f }, // 3: �E���O
	{ -0.5f, -0.5f,  0.5f }, // 4: ������
	{  0.5f, -0.5f,  0.5f }, // 5: �E����
	{ -0.5f,  0.5f,  0.5f }, // 6: ���㉜
	{  0.5f,  0.5f,  0.5f }  // 7: �E�㉜
	};

	uint32_t indices[] = {
		// �O��
		0, 1, 2, 2, 1, 3,
		// �w��
		5, 4, 7, 7, 4, 6,
		// ������
		4, 0, 6, 6, 0, 2,
		// �E����
		1, 5, 3, 3, 5, 7,
		// ���
		2, 3, 6, 6, 3, 7,
		// ���
		4, 5, 0, 0, 5, 1
	};

	// ���_�o�b�t�@
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

	// �C���f�b�N�X�o�b�t�@
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
