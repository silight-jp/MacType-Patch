#include <d2d1_3.h>
#include "common.h"
#include "userparams.h"



static void hookID2D1RenderTargetIfStill(ID2D1RenderTarget* pD2D1RenderTarget);
static void hookID2D1FactoryIfStill(ID2D1Factory* pD2D1Factory);
static void hookID2D1DeviceIfStill(ID2D1Device* pD2D1Device);



namespace Impl_ID2D1Device
{
	static HRESULT WINAPI CreateDeviceContext(
		ID2D1Device* This,
		D2D1_DEVICE_CONTEXT_OPTIONS options,
		ID2D1DeviceContext** deviceContext
	) {
		HRESULT hr = This->CreateDeviceContext(
			options,
			deviceContext
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*deviceContext);
		}
		return hr;
	}
}

namespace Impl_ID2D1Device1
{
	static HRESULT WINAPI CreateDeviceContext(
		ID2D1Device1* This,
		D2D1_DEVICE_CONTEXT_OPTIONS options,
		ID2D1DeviceContext1** deviceContext1
	) {
		HRESULT hr = This->CreateDeviceContext(
			options,
			deviceContext1
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*deviceContext1);
		}
		return hr;
	}
}

namespace Impl_ID2D1Device2
{
	static HRESULT WINAPI CreateDeviceContext(
		ID2D1Device2* This,
		D2D1_DEVICE_CONTEXT_OPTIONS options,
		ID2D1DeviceContext2** deviceContext2
	) {
		HRESULT hr = This->CreateDeviceContext(
			options,
			deviceContext2
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*deviceContext2);
		}
		return hr;
	}
}

namespace Impl_ID2D1Factory
{
	static HRESULT WINAPI CreateWicBitmapRenderTarget(
		ID2D1Factory* This,
		IWICBitmap* target,
		const D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
		ID2D1RenderTarget** renderTarget
	) {
		HRESULT hr = This->CreateWicBitmapRenderTarget(
			target,
			renderTargetProperties,
			renderTarget
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*renderTarget);
		}
		return hr;
	}
	
	static HRESULT WINAPI CreateHwndRenderTarget(
		ID2D1Factory* This,
		const D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
		const D2D1_HWND_RENDER_TARGET_PROPERTIES* hwndRenderTargetProperties,
		ID2D1HwndRenderTarget** hwndRenderTarget
	) {
		HRESULT hr = This->CreateHwndRenderTarget(
			renderTargetProperties,
			hwndRenderTargetProperties,
			hwndRenderTarget
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*hwndRenderTarget);
		}
		return hr;
	}
	
	static HRESULT WINAPI CreateDxgiSurfaceRenderTarget(
		ID2D1Factory* This,
		IDXGISurface* dxgiSurface,
		const D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
		ID2D1RenderTarget** renderTarget
	) {
		HRESULT hr = This->CreateDxgiSurfaceRenderTarget(
			dxgiSurface,
			renderTargetProperties,
			renderTarget
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*renderTarget);
		}
		return hr;
	}
	
	static HRESULT WINAPI CreateDCRenderTarget(
		ID2D1Factory* This,
		const D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
		ID2D1DCRenderTarget** dcRenderTarget
	) {
		HRESULT hr = This->CreateDCRenderTarget(
			renderTargetProperties,
			dcRenderTarget
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*dcRenderTarget);
		}
		return hr;
	}
}

namespace Impl_ID2D1Factory1
{
	static HRESULT WINAPI CreateDevice(
		ID2D1Factory1* This,
		IDXGIDevice* dxgiDevice,
		ID2D1Device** d2dDevice
	) {
		HRESULT hr = This->CreateDevice(
			dxgiDevice,
			d2dDevice
		);
		if (SUCCEEDED(hr)) {
			hookID2D1DeviceIfStill(*d2dDevice);
		}
		return hr;
	}
}

namespace Impl_ID2D1Factory2
{
	static HRESULT WINAPI CreateDevice(
		ID2D1Factory2* This,
		IDXGIDevice* dxgiDevice,
		ID2D1Device1** d2dDevice1
	) {
		HRESULT hr = This->CreateDevice(
			dxgiDevice,
			d2dDevice1
		);
		if (SUCCEEDED(hr)) {
			hookID2D1DeviceIfStill(*d2dDevice1);
		}
		return hr;
	}
}

namespace Impl_ID2D1Factory3
{
	static HRESULT WINAPI CreateDevice(
		ID2D1Factory3* This,
		IDXGIDevice* dxgiDevice,
		ID2D1Device2** d2dDevice2
	) {
		HRESULT hr = This->CreateDevice(
			dxgiDevice,
			d2dDevice2
		);
		if (SUCCEEDED(hr)) {
			hookID2D1DeviceIfStill(*d2dDevice2);
		}
		return hr;
	}
}

namespace Impl_ID2D1RenderTarget
{
	static HRESULT WINAPI CreateCompatibleRenderTarget(
		ID2D1RenderTarget* This,
		CONST D2D1_SIZE_F* desiredSize,
		CONST D2D1_SIZE_U* desiredPixelSize,
		CONST D2D1_PIXEL_FORMAT* desiredFormat,
		D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS options,
		ID2D1BitmapRenderTarget** bitmapRenderTarget
	) {
		HRESULT hr = This->CreateCompatibleRenderTarget(
			desiredSize,
			desiredPixelSize,
			desiredFormat,
			options,
			bitmapRenderTarget
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*bitmapRenderTarget);
		}
		return hr;
	}
	
	static void WINAPI SetTextAntialiasMode(
		ID2D1RenderTarget* This,
		D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode
	) {
		This->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
	}
	
	static void WINAPI SetTextRenderingParams(
		ID2D1RenderTarget* This,
		_In_opt_ IDWriteRenderingParams* textRenderingParams
	) {
		This->SetTextRenderingParams(Direct2DParams.pDWriteRenderingParams);
	}
}



static void hookID2D1Device(ID2D1Device* pD2D1Device) {
	void** v = getVtbl(pD2D1Device);
	hook(v[4], Impl_ID2D1Device::CreateDeviceContext);
}

static void hookID2D1Device1(ID2D1Device1* pD2D1Device1) {
	void** v = getVtbl(pD2D1Device1);
	hook(v[11], Impl_ID2D1Device1::CreateDeviceContext);
}

static void hookID2D1Device2(ID2D1Device2* pD2D1Device2) {
	void** v = getVtbl(pD2D1Device2);
	hook(v[12], Impl_ID2D1Device2::CreateDeviceContext);
}

static void hookID2D1Factory(ID2D1Factory* pD2D1Factory) {
	void** v = getVtbl(pD2D1Factory);
	hook(v[13], Impl_ID2D1Factory::CreateWicBitmapRenderTarget);
	hook(v[14], Impl_ID2D1Factory::CreateHwndRenderTarget);
	hook(v[15], Impl_ID2D1Factory::CreateDxgiSurfaceRenderTarget);
	hook(v[16], Impl_ID2D1Factory::CreateDCRenderTarget);
}

static void hookID2D1Factory1(ID2D1Factory1* pD2D1Factory1) {
	void** v = getVtbl(pD2D1Factory1);
	hook(v[17], Impl_ID2D1Factory1::CreateDevice);
}

static void hookID2D1Factory2(ID2D1Factory2* pD2D1Factory2) {
	void** v = getVtbl(pD2D1Factory2);
	hook(v[27], Impl_ID2D1Factory2::CreateDevice);
}

static void hookID2D1Factory3(ID2D1Factory3* pD2D1Factory3) {
	void** v = getVtbl(pD2D1Factory3);
	hook(v[28], Impl_ID2D1Factory3::CreateDevice);
}

static void hookID2D1RenderTarget(ID2D1RenderTarget* pD2D1RenderTarget) {
	void** v = getVtbl(pD2D1RenderTarget);
	hook(v[12], Impl_ID2D1RenderTarget::CreateCompatibleRenderTarget);
	hook(v[34], Impl_ID2D1RenderTarget::SetTextAntialiasMode);
	hook(v[36], Impl_ID2D1RenderTarget::SetTextRenderingParams);
}



static void hookID2D1DeviceIfStill(ID2D1Device* pD2D1Device) {
	void** vtbl = getVtbl(pD2D1Device);
	auto lock = globalMutex.getLock();
	if (insertVtbl(vtbl)) {
		hookID2D1Device(pD2D1Device);
		hookIfImplemented(pD2D1Device, hookID2D1Device1);
		hookIfImplemented(pD2D1Device, hookID2D1Device2);
	}
}

static void hookID2D1RenderTargetIfStill(ID2D1RenderTarget* pD2D1RenderTarget) {
	void** vtbl = getVtbl(pD2D1RenderTarget);
	auto lock = globalMutex.getLock();
	if (insertVtbl(vtbl)) {
		hookID2D1RenderTarget(pD2D1RenderTarget);
		
		ID2D1Factory* pD2D1Factory;
		pD2D1RenderTarget->GetFactory(&pD2D1Factory);
		hookID2D1FactoryIfStill(pD2D1Factory);
	}
	
	pD2D1RenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
	pD2D1RenderTarget->SetTextRenderingParams(Direct2DParams.pDWriteRenderingParams);
}

static void hookID2D1FactoryIfStill(ID2D1Factory* pD2D1Factory) {
	void** vtbl = getVtbl(pD2D1Factory);
	auto lock = globalMutex.getLock();
	if (insertVtbl(vtbl)) {
		hookID2D1Factory(pD2D1Factory);
		hookIfImplemented(pD2D1Factory, hookID2D1Factory1);
		hookIfImplemented(pD2D1Factory, hookID2D1Factory2);
		hookIfImplemented(pD2D1Factory, hookID2D1Factory3);
	}
}



namespace Orig
{
	static HRESULT(WINAPI *D2D1CreateDevice)(
		IDXGIDevice* dxgiDevice,
		CONST D2D1_CREATION_PROPERTIES* creationProperties,
		ID2D1Device** d2dDevice
	);
	
	static HRESULT(WINAPI *D2D1CreateDeviceContext)(
		IDXGISurface* dxgiSurface,
		CONST D2D1_CREATION_PROPERTIES* creationProperties,
		ID2D1DeviceContext** d2dDeviceContext
	);
}

namespace Impl
{
	static HRESULT WINAPI D2D1CreateFactory(
		D2D1_FACTORY_TYPE factoryType,
		REFIID riid,
		const D2D1_FACTORY_OPTIONS* pFactoryOptions,
		void** ppIFactory
	) {
		HRESULT hr = ::D2D1CreateFactory(factoryType, riid, pFactoryOptions, ppIFactory);
		if (SUCCEEDED(hr)) {
			auto pUnknown = reinterpret_cast<IUnknown*>(*ppIFactory);
			ID2D1Factory* pD2D1Factory;
			HRESULT hr2 = pUnknown->QueryInterface(&pD2D1Factory);
			if (SUCCEEDED(hr2)) {
				hookID2D1FactoryIfStill(pD2D1Factory);
				pD2D1Factory->Release();
			}
		}
		return hr;
	}
	
	static HRESULT WINAPI D2D1CreateDevice(
		IDXGIDevice* dxgiDevice,
		CONST D2D1_CREATION_PROPERTIES* creationProperties,
		ID2D1Device** d2dDevice
	) {
		auto orig = getWithLock(&Orig::D2D1CreateDevice);
		HRESULT hr = orig(
			dxgiDevice,
			creationProperties,
			d2dDevice
		);
		if (SUCCEEDED(hr)) {
			hookID2D1DeviceIfStill(*d2dDevice);
		}
		return hr;
	}
	
	static HRESULT WINAPI D2D1CreateDeviceContext(
		IDXGISurface* dxgiSurface,
		CONST D2D1_CREATION_PROPERTIES* creationProperties,
		ID2D1DeviceContext** d2dDeviceContext
	) {
		auto orig = getWithLock(&Orig::D2D1CreateDeviceContext);
		HRESULT hr = orig(
			dxgiSurface,
			creationProperties,
			d2dDeviceContext
		);
		if (SUCCEEDED(hr)) {
			hookID2D1RenderTargetIfStill(*d2dDeviceContext);
		}
		return hr;
	}
}



extern void hookDirect2D() {
	auto lock = globalMutex.getLock();
	
	HMODULE hModuleD2D1 = GetModuleHandleW(L"d2d1.dll");
	if (!hModuleD2D1) return;
	
	FARPROC D2D1CreateFactoryProc = GetProcAddress(hModuleD2D1, "D2D1CreateFactory");
	if (D2D1CreateFactoryProc) {
		auto orig = (decltype(&Impl::D2D1CreateFactory))D2D1CreateFactoryProc;
		hook(orig, Impl::D2D1CreateFactory);
	}
	
	FARPROC D2D1CreateDeviceProc = GetProcAddress(hModuleD2D1, "D2D1CreateDevice");
	if (D2D1CreateDeviceProc) {
		Orig::D2D1CreateDevice = (decltype(Orig::D2D1CreateDevice))D2D1CreateDeviceProc;
		hook(Orig::D2D1CreateDevice, Impl::D2D1CreateDevice);
	}
	
	FARPROC D2D1CreateDeviceContextProc = GetProcAddress(hModuleD2D1, "D2D1CreateDeviceContext");
	if (D2D1CreateDeviceContextProc) {
		Orig::D2D1CreateDeviceContext = (decltype(Orig::D2D1CreateDeviceContext))D2D1CreateDeviceContextProc;
		hook(Orig::D2D1CreateDeviceContext, Impl::D2D1CreateDeviceContext);
	}
}
