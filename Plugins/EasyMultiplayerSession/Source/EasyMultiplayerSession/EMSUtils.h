// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EMSUtils.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API UEMSUtils : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	void static ShowDebugMessage(FString message, FColor color = FColor::Orange, float time = 10);
};
