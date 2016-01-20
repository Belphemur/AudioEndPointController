// EndPointController.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <wchar.h>
#include <tchar.h>
#include <string>
#include "windows.h"
#include "Mmdeviceapi.h"
#include "Propidl.h"

// Format default string for outputing a device entry. The following parameters will be used in the following order:
// Index, Device Friendly Name
#define DEVICE_OUTPUT_FORMAT "Audio Device %d: %ws"
#include "../Audio.EndPoint.Controller/Audio.EndPoint.Controller.h"
#include <algorithm>

void invalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file,
	unsigned int line, uintptr_t pReserved);

void printAudioDevice(AudioDevice * device, LPCWSTR outFormat);
// EndPointController.exe [NewDefaultDeviceID]
int _tmain(int argc, LPCWSTR argv[])
{
	TGlobalState state;
	auto option = 0; // 0 indicates list devices.
	auto displayFormat = _T(DEVICE_OUTPUT_FORMAT);

	// Process command line arguments
	state.strDefaultDeviceID = '\0';
	state.deviceStateFilter = DEVICE_STATE_ACTIVE;

	setlocale(LC_ALL, "");
	for (int i = 1; i < argc; i++) 
	{
		if (wcscmp(argv[i], _T("--help")) == 0)
		{
			wprintf_s(_T("Lists active audio end-point playback devices or sets default audio end-point\n"));
			wprintf_s(_T("playback device.\n\n"));
			wprintf_s(_T("USAGE\n"));
			wprintf_s(_T("  EndPointController.exe [-a] [-f format_str]  Lists audio end-point playback\n"));
			wprintf_s(_T("                                               devices that are enabled.\n"));
			wprintf_s(_T("  EndPointController.exe device_index          Sets the default playvack device\n"));
			wprintf_s(_T("                                               with the given index.\n"));
			wprintf_s(_T("\n"));
			wprintf_s(_T("OPTIONS\n"));
			wprintf_s(_T("  -a             Display all devices, rather than just active devices.\n"));
			wprintf_s(_T("  -f format_str  Outputs the details of each device using the given format\n"));
			wprintf_s(_T("                 string. If this parameter is ommitted the format string\n"));
			wprintf_s(_T("                 defaults to: \"%s\"\n\n"), _T(DEVICE_OUTPUT_FORMAT));
			wprintf_s(_T("                 Parameters that are passed to the 'printf' function are\n"));
			wprintf_s(_T("                 ordered as follows:\n"));
			wprintf_s(_T("                   - Device index (int)\n"));
			wprintf_s(_T("                   - Device friendly name (wstring)\n"));
			wprintf_s(_T("                   - Device state (int)\n"));
			wprintf_s(_T("				     - Device default? (1 for true 0 for false as int)\n"));
			wprintf_s(_T("                   - Device description (wstring)\n"));
			wprintf_s(_T("                   - Device interface friendly name (wstring)\n"));
			wprintf_s(_T("                   - Device ID (wstring)\n"));
			exit(0);
		}
		else if (wcscmp(argv[i], _T("-a")) == 0)
		{
			state.deviceStateFilter = DEVICE_STATEMASK_ALL;
			continue;
		}
		else if (wcscmp(argv[i], _T("-f")) == 0)
		{
			if ((argc - i) >= 2) {
				displayFormat = argv[++i];
				
				// If printf is called with an invalid format string, jump to the invalidParameterHandler function.
				_set_invalid_parameter_handler(invalidParameterHandler);
				_CrtSetReportMode(_CRT_ASSERT, 0);
				continue;
			}
			else
			{
				wprintf_s(_T("Missing format string"));
				exit(1);
			}
		}
	}
	
	if (argc == 2) option = _wtoi(argv[1]);

	state.hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(state.hr))
	{
		std::list<AudioDevice*> devices;
		AudioEndPointerController::createDeviceEnumerator(&state, &devices);
		if(option < 1)
		{
			auto first = devices.begin();
			auto last = devices.end();
			while (first != last) {
				printAudioDevice(*first, displayFormat);
				++first;
			}			
		} else if( option <= static_cast<int>(state.nbDevices))
		{
			auto first = devices.begin();
			auto last = devices.end();
			while (first != last) {
				if((*first)->Index == option)
				{
					(*first)->SetDefault();
					break;
				}
				++first;
			}
		}
		else
		{
			wprintf_s(_T("Error: No audio end-point device with the index '%d'.\n"), option);
		}
	}
	return state.hr;
}

void printAudioDevice(AudioDevice * device, LPCWSTR outFormat)
{
	wprintf_s(outFormat,
		device->Index,
		device->FriendlyName,
		device->State,
		device->IsDefault,
		device->Description,
		device->InterfaceFriendlyName,
		device->Id
		);
	wprintf_s(_T("\n"));
	fflush(stdout);
}

void invalidParameterHandler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved)
{
   wprintf_s(_T("\nError: Invalid format_str.\n"));
   exit(1);
}