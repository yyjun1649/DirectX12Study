#pragma once
#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"

class Engine
{
public:

	void Init(const WindowInfo& info);
	void Render();

public:
	shared_ptr<Device>				GetDevice() { return _device; }	
	shared_ptr<CommandQueue>		GetCmdQueue() { return _cmdQueue; }
	shared_ptr<SwapChain>			GetSwapChain() { return _swapChain; }	
	shared_ptr<RootSignature>		GetRootSignature() { return _rootSignature; }

public:
	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);
private:
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	shared_ptr<Device>				_device;	// GPU에게 보낼 때 사용
	shared_ptr<CommandQueue>		_cmdQueue;	// GPU에게 요청을 보낼 요청을 쌓기위한 수단
	shared_ptr<SwapChain>			_swapChain;	// 더블 버퍼링
	shared_ptr<RootSignature>		_rootSignature;

};

