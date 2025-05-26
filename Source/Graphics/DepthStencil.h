#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include"Graphics/Graphics.h"

class DepthStencil {
public:
    enum class MODE {
        NONE,       // 通常
        MASK,       // マスク生成
        APPLY_MASK, // マスクに描画
        EXCLUSIVE,  // マスク以外に描画
		DEFAULT,    // デフォルト
        NUM         // モードの総数
    };

public:
    static DepthStencil& Instance() {
        static DepthStencil inst(Graphics::Instance().GetDevice());
        return inst;
    }
    void set(MODE mode);    // モードの設定
    void clear();           // マスクをクリアする

private:
    DepthStencil(ID3D11Device* device);
    ~DepthStencil() {}
    DepthStencil(const DepthStencil&) = delete; // コピーコンストラクタが存在しないことを明示

private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> modes[static_cast<size_t>(MODE::NUM)];
};
