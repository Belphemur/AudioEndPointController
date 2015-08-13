#include "ExportDll.h"
#include <Mmdeviceapi.h>
#include <string>

typedef struct TGlobalState
{
	HRESULT hr;
	int option;
	IMMDeviceEnumerator *pEnum;
	IMMDeviceCollection *pDevices;
	LPWSTR strDefaultDeviceID;
	IMMDevice *pCurrentDevice;
	LPCWSTR pDeviceFormatStr;
	int deviceStateFilter;
} TGlobalState;

AUDIOENDPOINTCONTROLLER_API void createDeviceEnumerator(TGlobalState* state);
AUDIOENDPOINTCONTROLLER_API void prepareDeviceEnumerator(TGlobalState* state);
AUDIOENDPOINTCONTROLLER_API void enumerateOutputDevices(TGlobalState* state);
AUDIOENDPOINTCONTROLLER_API HRESULT printDeviceInfo(IMMDevice* pDevice, int index, LPCWSTR outFormat, LPWSTR strDefaultDeviceID);
AUDIOENDPOINTCONTROLLER_API std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);
AUDIOENDPOINTCONTROLLER_API HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID);
