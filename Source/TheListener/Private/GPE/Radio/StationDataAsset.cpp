#include "GPE/Radio/StationDataAsset.h"

UConstantQNRT* UQNRTInfoDataAsset::GetConstantQNRT(UAkAudioEvent* AudioEvent) const
{
	if (QNRTMap.Contains(AudioEvent))
	{
		return QNRTMap[AudioEvent];
	}
	return nullptr;
}
