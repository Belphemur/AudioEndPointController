#include "ExportDll.h"
#include <Mmdeviceapi.h>
#include <string>
#include <list>
#include "AudioDevice.h"

typedef struct TGlobalState
{
	HRESULT hr;
	IMMDeviceEnumerator *pEnum;
	IMMDeviceCollection *pDevices;
	LPWSTR strDefaultDeviceID;
	IMMDevice *pCurrentDevice;
	int deviceStateFilter;
	UINT nbDevices;
} TGlobalState;

std::list<AudioDevice*> getAudioDevices(int deviceStateFilter);
AUDIOENDPOINTCONTROLLER_API void createDeviceEnumerator(TGlobalState* state, std::list<AudioDevice*> * list);
void prepareDeviceEnumerator(TGlobalState* state, std::list<AudioDevice*> * list);
void enumerateOutputDevices(TGlobalState* state, std::list<AudioDevice*> * list);
std::wstring getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key);
HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID);
AudioDevice * buildAudioDevice(IMMDevice* pDevice, int index, LPWSTR strDefaultDeviceID);
