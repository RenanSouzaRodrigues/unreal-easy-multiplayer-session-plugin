// Fill out your copyright notice in the Description page of Project Settings.

#include "Plugin_Multiplayer/Public/Characters/GPlayerCharacter.h"

AGPlayerCharacter::AGPlayerCharacter() {
	PrimaryActorTick.bCanEverTick = true;
}

void AGPlayerCharacter::BeginPlay() {
	Super::BeginPlay();
}

void AGPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

