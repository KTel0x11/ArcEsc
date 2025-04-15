#include "Toon.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    // �e�N�X�`���J���[���擾
   // float4 texColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord);
    float4 texColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * 1.3f; // ���邳�𒲐�
    // ��{�I�ȃJ���[�ƃ��C�e�B���O��g�ݍ��킹��
    float3 baseColor = texColor.rgb * pin.color.rgb;

    // ���邳�̌v�Z�i�O���[�X�P�[���I�E�F�C�g���g�p�j
    float intensity = dot(baseColor, float3(0.3, 0.59, 0.11));

    intensity *= 1.5; // ���邳�� 1.5 �{��

    // �g�D�[�����C�e�B���O�X�e�b�s���O�i�i�K�I�Ȗ��邳�j
    if (intensity > 0.8)
        intensity = 1.0;  // ���邢����
    else if (intensity > 0.6)
        intensity = 0.8;  // ���ԕ���
    else if (intensity > 0.4)
        intensity = 0.6;  // �Â�����
    else 
        intensity = 0.1;

    // �ŏI�F�̏o��
    return float4(intensity * baseColor, texColor.a * pin.color.a);
}