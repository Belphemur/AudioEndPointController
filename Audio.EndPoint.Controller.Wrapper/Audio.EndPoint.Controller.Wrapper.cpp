// This is the main DLL file.

#include "stdafx.h"
#include "Audio.EndPoint.Controller.h"

#include "Audio.EndPoint.Controller.Wrapper.h"

using namespace System;
using namespace AudioEndPointControllerWrapper;


List<AudioDeviceWrapper^>^ AudioEndPointControllerWrapper::AudioController::getAvailableAudioDevices()
{
	return AudioController::getAudioDevices(DeviceState::Active);
}

List<AudioDeviceWrapper^>^ AudioEndPointControllerWrapper::AudioController::getAllAudioDevices()
{
	return AudioController::getAudioDevices(DeviceState::All);
}

List<AudioDeviceWrapper^>^ AudioEndPointControllerWrapper::AudioController::getAudioDevices(DeviceState state)
{
	std::list<AudioDevice*>* audioDeviceList = AudioEndPointerController::getAudioDevices((int)state);
	auto result = AudioController::convertNativeList(audioDeviceList);
	delete audioDeviceList;
	return result;
}

List<AudioDeviceWrapper^>^ AudioEndPointControllerWrapper::AudioController::convertNativeList(std::list<AudioDevice*>* audioDeviceList)
{
	List<AudioDeviceWrapper^>^ list = gcnew List<AudioDeviceWrapper^>();
	for (std::list<AudioDevice*>::iterator i = audioDeviceList->begin(); i != audioDeviceList->end(); i++)
	{
		AudioDeviceWrapper^ wrapper = gcnew AudioDeviceWrapper(*i);
		list->Add(wrapper);
	}
	return list;
}
