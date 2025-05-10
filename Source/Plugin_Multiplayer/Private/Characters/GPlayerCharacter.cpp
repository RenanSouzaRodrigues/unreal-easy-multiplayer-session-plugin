// Fill out your copyright notice in the Description page of Project Settings.

#include "Plugin_Multiplayer/Public/Characters/GPlayerCharacter.h"
#include "Actors/GWeapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

// =========================================================================
// Unreal Methods
// =========================================================================
AGPlayerCharacter::AGPlayerCharacter() {
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates = true;
	
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

void AGPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// on this macros I cant use the 'this' keyword to refence variables. -Renan
	DOREPLIFETIME_CONDITION(AGPlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}



// =========================================================================
// Player Weapon Detection
// =========================================================================
void AGPlayerCharacter::SetOverlappedWeapon(AGWeapon* weapon) {
	// Changing this value will trigger the replication
	this->OverlappingWeapon = weapon;
	// This validation is only for the server, because the client will use the RepNotify to handle it.
	// This also only works because I know only the server handles the weapon overlapping with the player,
	// with this in mind, I can use the Locally controlled to check if the server is the one controlling to local controller
	// this way I can also show the Pickup Widget to the player server. -Renan
	if (this->IsLocallyControlled() && this->OverlappingWeapon) {
		this->OverlappingWeapon->ShowInteractionHud(true);
	}
}

void AGPlayerCharacter::OnRep_SetOverlappedWeapon() {
	if (this->OverlappingWeapon) {
		this->OverlappingWeapon->ShowInteractionHud(true);
	}
}



// =========================================================================
// Player Locomotion
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