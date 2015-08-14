// Audio.EndPoint.Controller.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Audio.EndPoint.Controller.h"
#include "PolicyConfig.h"
#include <tchar.h>
#include "Functiondiscoverykeys_devpkey.h"

// Create a multimedia device enumerator.
std::list<AudioDevice*> getAudioDevices(int deviceFilter)
{
	TGlobalState state;
	state.hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	state.deviceStateFilter = deviceFilter;
	std::list<AudioDevice*> list;
	createDeviceEnumerator(&state, &list);
	return list;
}

AUDIOENDPOINTCONTROLLER_API void createDeviceEnumerator(TGlobalState* state, std::list<AudioDevice*> * list)
{
	state->pEnum = NULL;
	state->hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&state->pEnum);
	if (SUCCEEDED(state->hr))
	{
		prepareDeviceEnumerator(state, list);
	}
}

// Prepare the device enumerator
void prepareDeviceEnumerator(TGlobalState* state, std::list<AudioDevice*> * list)
{
	state->hr = state->pEnum->EnumAudioEndpoints(eRender, state->deviceStateFilter, &state->pDevices);
	if SUCCEEDED(state->hr)
	{
		enumerateOutputDevices(state, list);
	}
	state->pEnum->Release();
}

void enumerateOutputDevices(TGlobalState* state, std::list<AudioDevice*> * list)
{
	state->pDevices->GetCount(&state->nbDevices);

	// Get default device
	IMMDevice* pDefaultDevice;
	state->hr = state->pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);
	if (SUCCEEDED(state->hr))
	{

		state->hr = pDefaultDevice->GetId(&state->strDefaultDeviceID);

		// Iterate all devices
		for (UINT i = 1; i <= state->nbDevices; i++)
		{
			state->hr = state->pDevices->Item(i - 1, &state->pCurrentDevice);
			if (SUCCEEDED(state->hr))
			{
				list->push_back(buildAudioDevice(state->pCurrentDevice, i,
				                                 state->strDefaultDeviceID));
				state->pCurrentDevice->Release();
			}
		}
	}
	

	state->pDevices->Release();
}

AudioDevice * buildAudioDevice(IMMDevice* pDevice, int index, LPWSTR strDefaultDeviceID)
{
	// Device ID
	LPWSTR strID = NULL;
	HRESULT hr = pDevice->GetId(&strID);
	if (!SUCCEEDED(hr))
	{
		return nullptr;
	}

	bool deviceDefault = (strDefaultDeviceID != '\0' && (wcscmp(strDefaultDeviceID, strID) == 0));

	// Device state
	DWORD dwState;
	hr = pDevice->GetState(&dwState);
	if (!SUCCEEDED(hr))
	{
		return nullptr;
	}

	IPropertyStore *pStore;
	hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
	AudioDevice* device = nullptr;
	if (SUCCEEDED(hr))
	{
		std::wstring friendlyName = getDeviceProperty(pStore, PKEY_Device_FriendlyName);
		std::wstring Desc = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
		std::wstring interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);

		if (SUCCEEDED(hr))
		{
			device = new AudioDevice(index, friendlyName.c_str(), dwState, deviceDefault, Desc.c_str(), interfaceFriendlyName.c_str(), strID);
		}

		pStore->Release();
	}
	return device;
}

std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
{
	PROPVARIANT prop;
	PropVariantInit(&prop);
	HRESULT hr = pStore->GetValue(key, &prop);
	if (SUCCEEDED(hr))
	{
		std::wstring result(prop.pwszVal);
		PropVariantClear(&prop);
		return result;
	}
	else
	{
		return std::wstring(L"");
	}
}

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
	IPolicyConfigVista *pPolicyConfig;
	ERole reserved = eConsole;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
		NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
		pPolicyConfig->Release();
	}
	return hr;
}