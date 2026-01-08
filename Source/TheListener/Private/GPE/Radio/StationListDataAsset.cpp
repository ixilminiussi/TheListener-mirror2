// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/Radio/StationListDataAsset.h"

const TArray<UStationAsset*>& UStationListDataAsset::GetStationsData() const
{
	return CurrentStationList;
}
