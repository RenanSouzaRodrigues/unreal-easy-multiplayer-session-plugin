// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EMSNetRoleInfoWidget.h"

#include "GameFramework/PlayerState.h"

void UEMSNetRoleInfoWidget::BuildNetRoleNames(APawn* pawnReference) {
	ENetRole localRole = pawnReference->GetLocalRole();
	ENetRole remoteRole = pawnReference->GetRemoteRole();

	switch (localRole) {
		case ROLE_Authority: this->LocalNetRoleName = FString("Authority"); break;
		case ROLE_AutonomousProxy: this->LocalNetRoleName = FString("Autonomous Proxy"); break;
		case ROLE_SimulatedProxy: this->LocalNetRoleName = FString("Simulated Proxy"); break;
		case ROLE_None: this->LocalNetRoleName = FString("None"); break;
		default: this->LocalNetRoleName = FString("None");
	}

	switch (remoteRole) {
		case ROLE_Authority: this->RemoteNetRoleName = FString("Authority"); break;
		case ROLE_AutonomousProxy: this->RemoteNetRoleName = FString("Autonomous Proxy"); break;
		case ROLE_SimulatedProxy: this->RemoteNetRoleName = FString("Simulated Proxy"); break;
		case ROLE_None: this->RemoteNetRoleName = FString("None"); break;
		default: this->RemoteNetRoleName = FString("None");
	}

	if (auto playerState = pawnReference->GetController()->GetPlayerState<APlayerState>()) {
		this->PlayerName = playerState->GetPlayerName();
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