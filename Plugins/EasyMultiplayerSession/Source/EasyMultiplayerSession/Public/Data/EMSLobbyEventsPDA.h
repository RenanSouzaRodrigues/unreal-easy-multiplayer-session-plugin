// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EMSLobbyEventsPDA.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinsLobbyDelegate, APlayerController*, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeavesLobbyDelegate, AController*, PlayerLeaving);

UCLASS()
class EASYMULTIPLAYERSESSION_API UEMSLobbyEventsPDA : public UPrimaryDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Lobby Events")
	FOnPlayerJoinsLobbyDelegate OnPlayerJoinsLobbyEvent;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Lobby Events")
	FOnPlayerLeavesLobbyDelegate OnPlayerLeaverLobbyEvent;
};
