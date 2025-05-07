// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EMSNetRoleInfoWidget.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API UEMSNetRoleInfoWidget : public UUserWidget {
	GENERATED_BODY()
	
private:
	FString LocalNetRoleName;
	FString RemoteNetRoleName;
	FString PlayerName;
	
public:
	UFUNCTION(BlueprintCallable)
	void BuildNetRoleNames(APawn* pawnReference);

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetPawnLocalNetRoleName();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetPawnRemoteNetRoleName();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetPawnPlayerName();
};
