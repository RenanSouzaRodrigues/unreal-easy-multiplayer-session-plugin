// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EMSUtils.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API UEMSUtils : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	/**
	 * Method that displays a message on the game screen. Only available on debug or development builds
	 * @param message The Message you want to display
	 * @param color The color of the message. Has a default color of FColor::Orange
	 * @param time The amount of time in seconds the message needs to stay visible. Has a default value of 10 seconds
	 */
	void static ShowDebugMessage(FString message, FColor color = FColor::Orange, float time = 10);

	/**
	 * Method that displays a perpetual message on the game screen. Only available on debug or development builds
	 * If this method is called one time after another, it will update the original message.
	 * @param message The Message you want to display
	 * @param color The color of the message. Has a default color of FColor::Orange
	 */
	void static ShowPersistentDebugMessage(FString message, FColor color = FColor::Orange);
};
