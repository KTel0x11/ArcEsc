#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class DebugRenderer
{
public:
	DebugRenderer(ID3D11Device* device);
	~DebugRenderer() {}

public:
	// 描画実行
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// 球描画
	void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

	// 円柱描画
	void DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& rotation);

	//カプセル描画
	void DrawCapsule(const DirectX::XMFLOAT3& position, float height, float radius,const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& rotation);

	//四角形描画
	//void DrawQuad(const DirectX::XMFLOAT3& center, float width, float height,
	//	const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& rotation);

	//箱描画
	void DrawBox(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& size,
		const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& rotation);
	

private:
	// 球メッシュ作成
	void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

	// 円柱メッシュ作成
	void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

	//カプセルメッシュ作成
	void CreateCapsuleMesh(ID3D11Device* device, float radius, float height, int slices, int stacks);

	//四角メッシュ作成
	void CreateBoxMesh(ID3D11Device* device);
	
private:
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	wvp;
		DirectX::XMFLOAT4	color;
	};

	struct Sphere
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	center;
		float				radius;
	};

	struct Cylinder
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	rotation;
		float				radius;
		float				height;
	};

	struct Capsule
	{
		DirectX::XMFLOAT3 position;  // カプセルの中心位置
		float height;               // カプセルの高さ (円柱部分を含む全体)
		float radius;               // 半径
		DirectX::XMFLOAT4 color;    // 色
		DirectX::XMFLOAT3 rotation; // 回転 (ラジアンで X, Y, Z)
	};

	struct Box
	{
		DirectX::XMFLOAT3 position;  // 箱の中心位置
		DirectX::XMFLOAT3 size;      // 箱のサイズ (幅, 高さ, 奥行き)
		DirectX::XMFLOAT4 color;     // 色
		DirectX::XMFLOAT3 rotation;  // 回転 (ラジアンで X, Y, Z)
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cylinderVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			capsuleVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			quadVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			quadIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			boxVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			boxIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	std::vector<Sphere>		spheres;
	std::vector<Cylinder>	cylinders;
	std::vector<Capsule> capsules;
	std::vector<Box> boxes;

	UINT	sphereVertexCount = 0;
	UINT	cylinderVertexCount = 0;
	UINT	capsuleVertexCount = 0;
	UINT	quadVertexCount = 0;
	UINT	quadIndexCount = 0;
	UINT	boxVertexCount = 0;
	UINT	boxIndexCount = 0;
};
