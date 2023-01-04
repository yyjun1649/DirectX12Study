#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void Mesh::Init(vector<Vertex>& vec)
{
	_vertexCount = static_cast<uint32>(vec.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer)); // GPU 안에있는 공간

	// 따라서 두단계를 통해 전달
	// Copy the triangle data to the vertex buffer.
	void* vertexDataBuffer = nullptr; // 잠시 데이터 복사
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	::memcpy(vertexDataBuffer, &vec[0], bufferSize); // GPU 메모리에 Copy
	_vertexBuffer->Unmap(0, nullptr); // 뚜껑 다시 덮음

	// Initialize the vertex buffer view.
	// GPU가 View를 통해 어떤걸 가르키고 있는지 판별 
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress(); // 어디에 있는지
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void Mesh::Render()
{
	// CMD List / RenderBegin() 과 RenderEnd() 사이에서 호출
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15) View를 통해 전달
	CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
}