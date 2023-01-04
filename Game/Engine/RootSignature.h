#pragma once

// CPU [   ] GPU [   ]


class RootSignature
{
public:
	void Init(ComPtr<ID3D12Device> device);

	ComPtr<ID3D12RootSignature>	GetSignature() { return _signature; }

private:
	ComPtr<ID3D12RootSignature> _signature; // GPU쪽에 계약서를 넘기는 역할
};

