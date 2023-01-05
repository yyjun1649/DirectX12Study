#pragma once
#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeap.h"
#include "Texture.h"
#include "DepthStencilBuffer.h"
#include "Input.h"
#include "Timer.h"

class Engine
{
public:

	void Init(const WindowInfo& info);
	void Render();

public:
	void Update();
	void LateUpdate();

public:
	//Rendering
	shared_ptr<Device>					GetDevice() { return _device; }	
	shared_ptr<CommandQueue>			GetCmdQueue() { return _cmdQueue; }
	shared_ptr<SwapChain>				GetSwapChain() { return _swapChain; }	
	shared_ptr<RootSignature>			GetRootSignature() { return _rootSignature; }
	shared_ptr<TableDescriptorHeap>		GetTableDescHeap() { return _tableDescHeap; }
	shared_ptr<DepthStencilBuffer>		GetDepthStencilBuffer() { return _depthStencilBuffer; }

	//etc
	shared_ptr<Input>					GetInput() { return _input; }
	shared_ptr<Timer>					GetTimer() { return _timer; }

	shared_ptr<ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }

public:
	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);

private:
	void ShowFPS();
	void CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count);

private:
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	shared_ptr<Device>					_device					= make_shared<Device>();	// GPU에게 보낼 때 사용
	shared_ptr<CommandQueue>			_cmdQueue				= make_shared<CommandQueue>();	// GPU에게 요청을 보낼 요청을 쌓기위한 수단
	shared_ptr<SwapChain>				_swapChain				= make_shared<SwapChain>();	// 더블 버퍼링
	shared_ptr<RootSignature>			_rootSignature			= make_shared<RootSignature>();
	shared_ptr<ConstantBuffer>			_cb						= make_shared<ConstantBuffer>();
	shared_ptr<TableDescriptorHeap>		_tableDescHeap			= make_shared<TableDescriptorHeap>();
	shared_ptr<DepthStencilBuffer>		_depthStencilBuffer		= make_shared<DepthStencilBuffer>();

	shared_ptr<Input>					_input					= make_shared<Input>();
	shared_ptr<Timer>					_timer					= make_shared<Timer>();

	vector<shared_ptr<ConstantBuffer>>	_constantBuffers;


};

