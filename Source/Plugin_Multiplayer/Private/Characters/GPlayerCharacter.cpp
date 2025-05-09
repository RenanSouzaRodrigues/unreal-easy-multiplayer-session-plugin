// Fill out your copyright notice in the Description page of Project Settings.

#include "Plugin_Multiplayer/Public/Characters/GPlayerCharacter.h"

#include "Actors/GWeapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"


// =========================================================================
// LIFE CYCLE
// =========================================================================
AGPlayerCharacter::AGPlayerCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	this->CameraSpringArm = this->CreateDefaultSubobject<USpringArmComponent>("Camera Spring Arm");
	this->CameraSpringArm->SetupAttachment(this->GetRootComponent());
	this->CameraSpringArm->bUsePawnControlRotation = true;
	
	this->PlayerCamera = this->CreateDefaultSubobject<UCameraComponent>("Player Camera");
	this->PlayerCamera->SetupAttachment(this->CameraSpringArm);
}

void AGPlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	this->GetCharacterMovement()->MaxWalkSpeed = this->DefaultMovementSpeed;
	this->bIsSprinting = false;
}

void AGPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


// =========================================================================
// REPLICATION
// =========================================================================
void AGPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// on this macros I cant use the 'this' keyword to refence variables
	DOREPLIFETIME_CONDITION(AGPlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}


// =========================================================================
// CLASS METHODS
// =========================================================================
void AGPlayerCharacter::MovePlayer(float valueX, float valueY) {
	if (valueX == 0 && valueY == 0) return;
	this->AddMovementInput(this->GetActorForwardVector(), valueX);
	this->AddMovementInput(this->GetActorRightVector(), valueY);
}

void AGPlayerCharacter::MoveCamera(float valueX, float valueY) {
	if (valueX == 0 && valueY == 0) return;
	this->AddControllerYawInput(valueX);
	this->AddControllerPitchInput(valueY);
}

void AGPlayerCharacter::Sprint() {
	this->GetCharacterMovement()->MaxWalkSpeed = this->SprintMovementSpeed;
	this->bIsSprinting = true;
}

void AGPlayerCharacter::StopSprint() {
	this->GetCharacterMovement()->MaxWalkSpeed = this->DefaultMovementSpeed;
	this->bIsSprinting = false;
}

void AGPlayerCharacter::PerformJump() {
	this->Jump();
}


// =========================================================================
// Setters
// =========================================================================
void AGPlayerCharacter::SetOverlappedWeapon(AGWeapon* weapon) {
	// this value change will trigger the Replication Delegate AGPlayerCharacter::GetLifetimeReplicatedProps 
	this->OverlappingWeapon = weapon;
	this->OverlappingWeapon->ShowInteractionHud(true);
}