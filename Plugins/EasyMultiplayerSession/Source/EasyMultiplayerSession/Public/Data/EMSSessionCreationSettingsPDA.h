// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EMSSessionCreationSettingsPDA.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API UEMSSessionCreationSettingsPDA : public UPrimaryDataAsset {
	GENERATED_BODY()

public:
	// Define if a Player can join the created session even if the match is already in progress
	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	bool bAllowJoinInProgress { true };
	
	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	bool bAllowJoinViaPresence { true };

	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	bool bAllowInvites { true };

	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	bool bAllowJoinViaPresenceFriendsOnly { false };

	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	bool bShouldAdvertise { true };

	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	bool bUsesPresence { true };

	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	bool bUseLobbiesIfAvailable { true };

	UPROPERTY(EditAnywhere, Category="Easy Multiplayer Session Creation Settings")
	int32 BuildUniqueId { 1 };
};
