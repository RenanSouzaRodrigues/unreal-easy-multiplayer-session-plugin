// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EMSTestConnectionMenuWidget.h"

#include "EasyMultiplayerSubsystem.h"
#include "EasyMultiplayerSession/EMSUtils.h"

void UEMSTestConnectionMenuWidget::NativeDestruct() {
	Super::NativeDestruct();
	this->CloseMenu();
}

void UEMSTestConnectionMenuWidget::BuildMenu() {
	this->AddToViewport();
	this->SetVisibility(ESlateVisibility::Visible);

	if (auto playerController = this->GetWorld()->GetFirstPlayerController()) {
		FInputModeUIOnly inputModeData;
		inputModeData.SetWidgetToFocus(TakeWidget());
		inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		playerController->SetInputMode(inputModeData);
		playerController->SetShowMouseCursor(true);
	}
}

void UEMSTestConnectionMenuWidget::CloseMenu() {
	this->SetVisibility(ESlateVisibility::Collapsed);
	this->RemoveFromParent();

	if (auto playerController = this->GetWorld()->GetFirstPlayerController()) {
		FInputModeGameOnly inputModeData;
		playerController->SetInputMode(inputModeData);
		playerController->SetShowMouseCursor(false);
	}
}

// TEST FUNCTION (REMOVE AFTER)
void UEMSTestConnectionMenuWidget::CreateSession() {
	if (UEasyMultiplayerSubsystem* easyMultiplayerSubsystem = this->GetGameInstance()->GetSubsystem<UEasyMultiplayerSubsystem>()) {
		easyMultiplayerSubsystem->CreateSession(10, FString("Free For All"));
		
		if (UWorld* world = this->GetWorld()) {
			// This will be on the easy multiplayer subsystem eventually. -Renan
			world->ServerTravel("/Game/GameAssets/Levels/LVL_Lobby?Listen");
		}
	} else {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to retrive Easy Multiplayer Subsystem. Something went wrong with the Engine lifecycle"), FColor::Red);
	}
}

void UEMSTestConnectionMenuWidget::JoinSession() {
	if (UEasyMultiplayerSubsystem* EasyMultiplayerSubsystem = this->GetGameInstance()->GetSubsystem<UEasyMultiplayerSubsystem>()) {
		UEMSUtils::ShowDebugMessage(TEXT("Not implemented yet."), FColor::Red);
	} else {
		UEMSUtils::ShowDebugMessage(TEXT("Unable to retrive Easy Multiplayer Subsystem. Something went wrong with the Engine lifecycle"), FColor::Red);
	}
}
