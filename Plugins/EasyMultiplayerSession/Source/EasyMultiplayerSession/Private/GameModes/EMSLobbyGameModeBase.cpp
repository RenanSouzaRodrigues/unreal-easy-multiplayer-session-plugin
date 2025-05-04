// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/EMSLobbyGameModeBase.h"
#include "EasyMultiplayerSession/EMSUtils.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void AEMSLobbyGameModeBase::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	if (this->GameState) {
		int32 numberOfPlayers = this->GameState.Get()->PlayerArray.Num();
		UEMSUtils::ShowPersistentDebugMessage(FString::Printf(TEXT("Current number of players: %d"), numberOfPlayers), FColor::Yellow);
	}

	if (APlayerState* playerState = NewPlayer->GetPlayerState<APlayerState>()) {
		UEMSUtils::ShowDebugMessage(FString::Printf(TEXT("%s Joined the session"), *playerState->GetPlayerName()), FColor::Cyan);
	}
}

void AEMSLobbyGameModeBase::Logout(AController* Exiting) {
	Super::Logout(Exiting);

	if (this->GameState) {
		int32 numberOfPlayers = this->GameState.Get()->PlayerArray.Num() - 1;
		UEMSUtils::ShowPersistentDebugMessage(FString::Printf(TEXT("Current number of players: %d"), numberOfPlayers), FColor::Yellow);
	}
	
	if (APlayerState* playerState = Exiting->GetPlayerState<APlayerState>()) {
		UEMSUtils::ShowDebugMessage(FString::Printf(TEXT("%s exited the session"), *playerState->GetPlayerName()), FColor::Cyan);
	}
}