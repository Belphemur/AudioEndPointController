// Audio.EndPoint.Controller.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Audio.EndPoint.Controller.h"
#include "PolicyConfig.h"
#include <tchar.h>
#include "Functiondiscoverykeys_devpkey.h"

// Create a multimedia device enumerator.
AUDIOENDPOINTCONTROLLER_API void createDeviceEnumerator(TGlobalState* state)
{
	state->pEnum = NULL;
	state->hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&state->pEnum);
	if (SUCCEEDED(state->hr))
	{
		prepareDeviceEnumerator(state);
	}
}

// Prepare the device enumerator
AUDIOENDPOINTCONTROLLER_API void prepareDeviceEnumerator(TGlobalState* state)
{
	state->hr = state->pEnum->EnumAudioEndpoints(eRender, state->deviceStateFilter, &state->pDevices);
	if SUCCEEDED(state->hr)
	{
		enumerateOutputDevices(state);
	}
	state->pEnum->Release();
}

// Enumerate the output devices
AUDIOENDPOINTCONTROLLER_API void enumerateOutputDevices(TGlobalState* state)
{
	UINT count;
	state->pDevices->GetCount(&count);

	// If option is less than 1, list devices
	if (state->option < 1)
	{

		// Get default device
		IMMDevice* pDefaultDevice;
		state->hr = state->pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);
		if (SUCCEEDED(state->hr))
		{

			state->hr = pDefaultDevice->GetId(&state->strDefaultDeviceID);

			// Iterate all devices
			for (int i = 1; i <= (int)count; i++)
			{
				state->hr = state->pDevices->Item(i - 1, &state->pCurrentDevice);
				if (SUCCEEDED(state->hr))
				{
					state->hr = printDeviceInfo(state->pCurrentDevice, i, state->pDeviceFormatStr,
						state->strDefaultDeviceID);
					state->pCurrentDevice->Release();
				}
			}
		}
	}
	// If option corresponds with the index of an audio device, set it to default
	else if (state->option <= (int)count)
	{
		state->hr = state->pDevices->Item(state->option - 1, &state->pCurrentDevice);
		if (SUCCEEDED(state->hr))
		{
			LPWSTR strID = NULL;
			state->hr = state->pCurrentDevice->GetId(&strID);
			if (SUCCEEDED(state->hr))
			{
				state->hr = SetDefaultAudioPlaybackDevice(strID);
			}
			state->pCurrentDevice->Release();
		}
	}
	// Otherwise inform user than option doesn't correspond with a device
	else
	{
		wprintf_s(_T("Error: No audio end-point device with the index '%d'.\n"), state->option);
	}

	state->pDevices->Release();
}

AUDIOENDPOINTCONTROLLER_API HRESULT printDeviceInfo(IMMDevice* pDevice, int index, LPCWSTR outFormat, LPWSTR strDefaultDeviceID)
{
	// Device ID
	LPWSTR strID = NULL;
	HRESULT hr = pDevice->GetId(&strID);
	if (!SUCCEEDED(hr))
	{
		return hr;
	}

	int deviceDefault = (strDefaultDeviceID != '\0' && (wcscmp(strDefaultDeviceID, strID) == 0));

	// Device state
	DWORD dwState;
	hr = pDevice->GetState(&dwState);
	if (!SUCCEEDED(hr))
	{
		return hr;
	}

	IPropertyStore *pStore;
	hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
	if (SUCCEEDED(hr))
	{
		std::wstring friendlyName = getDeviceProperty(pStore, PKEY_Device_FriendlyName);
		std::wstring Desc = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
		std::wstring interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);

		if (SUCCEEDED(hr))
		{
			wprintf_s(outFormat,
				index,
				friendlyName.c_str(),
				dwState,
				deviceDefault,
				Desc.c_str(),
				interfaceFriendlyName.c_str(),
				strID
				);
			wprintf_s(_T("\n"));
			fflush(stdout);
		}

		pStore->Release();
	}
	return hr;
}

AUDIOENDPOINTCONTROLLER_API std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
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

AUDIOENDPOINTCONTROLLER_API HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
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