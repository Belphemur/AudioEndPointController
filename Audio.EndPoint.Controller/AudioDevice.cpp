#include "stdafx.h"
#include "AudioDevice.h"
#include "Audio.EndPoint.Controller.h"


void AudioDevice::SetDefault()
{
	SetDefaultAudioPlaybackDevice(this->id);
}

AudioDevice::AudioDevice()
{
}


AudioDevice::~AudioDevice()
{
}
