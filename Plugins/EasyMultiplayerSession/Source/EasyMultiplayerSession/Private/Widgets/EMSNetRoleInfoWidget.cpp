// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/EMSNetRoleInfoWidget.h"
#include "EasyMultiplayerSession/EMSUtils.h"
#include "GameFramework/PlayerState.h"

void UEMSNetRoleInfoWidget::BuildCharacterNetInfo(APawn* pawnReference, APlayerState* playerState) {
	if (pawnReference == nullptr) {
		this->PlayerName = this->LocalNetRoleName = this->RemoteNetRoleName = FString("undefined");
		UEMSUtils::ShowPersistentDebugMessage(TEXT("Invalid Pawn to display information on Roles"), FColor::Red);
		return;
	}
	
	ENetRole localRole = pawnReference->GetLocalRole();
	ENetRole remoteRole = pawnReference->GetRemoteRole();

	this->LocalNetRoleName = this->GetRoleName(localRole);
	this->RemoteNetRoleName = this->GetRoleName(remoteRole);
	
	if (playerState) {
		this->PlayerName = playerState->GetPlayerName();
	} else {
		this->PlayerName = FString("No player state provided");
	}
}

FText UEMSNetRoleInfoWidget::GetPawnLocalNetRoleName() {
	return FText::FromString(this->LocalNetRoleName);
}

FText UEMSNetRoleInfoWidget::GetPawnRemoteNetRoleName() {
	return FText::FromString(this->RemoteNetRoleName);
}

FText UEMSNetRoleInfoWidget::GetPawnPlayerName() {
	return FText::FromString(this->PlayerName);
}

FString UEMSNetRoleInfoWidget::GetRoleName(ENetRole role) {
	switch (role) {
	case ROLE_Authority: return FString("Authority");
	case ROLE_AutonomousProxy: return FString("Autonomous Proxy");
	case ROLE_SimulatedProxy: return FString("Simulated Proxy");
	default: return FString("None");
	}
}
