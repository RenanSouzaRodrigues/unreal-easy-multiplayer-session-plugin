// Fill out your copyright notice in the Description page of Project Settings.

#include "EMSUtils.h"

void UEMSUtils::ShowDebugMessage(FString message, FColor color, float time) {
	if (GEngine) GEngine->AddOnScreenDebugMessage(2, time, color, message, false);
}

void UEMSUtils::ShowPersistentDebugMessage(FString message, FColor color) {
	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 99999, color, message);
}
