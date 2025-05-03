// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EMSTestConnectionMenuWidget.generated.h"

UCLASS()
class EASYMULTIPLAYERSESSION_API UEMSTestConnectionMenuWidget : public UUserWidget {
	GENERATED_BODY()

public:
	/**
	 * Add the menu to the screen, removes the player controller input capability and display buttons to handle session host and join.
	 */
	UFUNCTION(BlueprintCallable)
	void BuildMenu();

	/**
	 * Makes the widget disappear and removes it from the view port. 
	 */
	UFUNCTION(BlueprintCallable)
	void CloseMenu();
};
