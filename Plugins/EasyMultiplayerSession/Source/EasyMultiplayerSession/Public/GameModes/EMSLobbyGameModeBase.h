// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EMSLobbyGameModeBase.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API AEMSLobbyGameModeBase : public AGameMode {
	GENERATED_BODY()

public:
	/**
	 * This field is only required if you pretend to use Lobby events. In order to Dispatch the Lobby global events,
	 * first create a new Data Asset based on EMSLobbyEventsPDA, then assign it here.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lobby Configuration")
	TObjectPtr<class UEMSLobbyEventsPDA> EasyMultiplayerLobbyEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lobby Configuration")
	bool bDispatchEventWhenNewPlayerJoinsLobby { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lobby Configuration")
	bool bDispatchEventWhenPlayerLeavesLobby { false };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lobby Configuration")
	bool bEnableAutoGameStartWhenReachMinimalPlayersAmount { false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lobby Configuration", meta=(EditCondition="bEnableAutoGameStartWhenReachMinimalPlayersAmount"))
	int32 MinimalPlayersAmountToStart { 16 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lobby Configuration")
	FString LevelNameToLoadWhenReadyToPlay;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/**
	 * Method that moves the players on the lobby to the game making a seamless transition.
	 */
	UFUNCTION(BlueprintCallable)
	void StartGameSeamless();

	/**
	 * Method that moves the players on the lobby to the game making a non-seamless transition.
	 */
	UFUNCTION(BlueprintCallable)
	void StartGameNonSeamless();

private:
	bool IsLobbyEventsReferenceValid() const;
};
