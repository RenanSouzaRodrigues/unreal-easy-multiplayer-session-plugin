// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EMSTestConnectionMenuWidget.h"
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
