// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EMSNetRoleInfoWidget.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API UEMSNetRoleInfoWidget : public UUserWidget {
	GENERATED_BODY()
	
private:
	FString LocalNetRoleName { FString("undefined") };
	FString RemoteNetRoleName { FString("undefined") };
	FString PlayerName { FString("undefined") };
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void BuildNetRoleNames(APawn* pawnReference, APlayerController* playerController);

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetPawnLocalNetRoleName();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetPawnRemoteNetRoleName();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetPawnPlayerName();

private:
	FString GetRoleName(ENetRole role);
};
