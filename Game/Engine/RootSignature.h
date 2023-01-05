#pragma once

// CPU [   ] GPU [   ]


class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetSignature() { return _signature; }

private:
	void CreateSamplerDesc();
	void CreateRootSignature();

private:
	ComPtr<ID3D12RootSignature> _signature; // GPU�ʿ� ��༭�� �ѱ�� ����
	D3D12_STATIC_SAMPLER_DESC _samplerDesc;
};

