// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/EMSLobbyGameModeBase.h"

#include "EasyMultiplayerSubsystem.h"
#include "Data/EMSLobbyEventsPDA.h"
#include "EasyMultiplayerSession/EMSUtils.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void AEMSLobbyGameModeBase::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	if (this->bDispatchEventWhenNewPlayerJoinsLobby && this->IsLobbyEventsReferenceValid()) {
		this->EasyMultiplayerLobbyEvents->OnPlayerJoinsLobbyEvent.Broadcast(NewPlayer);
	}
	
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

	if (this->bDispatchEventWhenPlayerLeavesLobby && this->IsLobbyEventsReferenceValid()) {
		this->EasyMultiplayerLobbyEvents->OnPlayerLeaverLobbyEvent.Broadcast(Exiting);
	}
	
	if (this->GameState) {
		int32 numberOfPlayers = this->GameState.Get()->PlayerArray.Num() - 1;
		UEMSUtils::ShowPersistentDebugMessage(FString::Printf(TEXT("Current number of players: %d"), numberOfPlayers), FColor::Yellow);
	}
	
	if (APlayerState* playerState = Exiting->GetPlayerState<APlayerState>()) {
		UEMSUtils::ShowDebugMessage(FString::Printf(TEXT("%s exited the session"), *playerState->GetPlayerName()), FColor::Cyan);
	}
}

void AEMSLobbyGameModeBase::StartGameSeamless() {
	UEasyMultiplayerSubsystem* EasyMultiplayerSubsystem = this->GetGameInstance()->GetSubsystem<UEasyMultiplayerSubsystem>();
	if (EasyMultiplayerSubsystem) {
		this->bUseSeamlessTravel;
		EasyMultiplayerSubsystem->OpenGameLevelAsHostServer(this->LevelNameToLoadWhenReadyToPlay);
	}
}

bool AEMSLobbyGameModeBase::IsLobbyEventsReferenceValid() const {
	if (this->EasyMultiplayerLobbyEvents == nullptr) {
		UEMSUtils::ShowDebugMessage(TEXT("Lobby Game Mode can't Dispatch events. Make sure that the EasyMultiplayerLobbyEvents reference is defined"), FColor::Red);
		return false;
	}
	return true;
}
