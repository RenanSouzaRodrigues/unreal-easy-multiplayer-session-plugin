// Fill out your copyright notice in the Description page of Project Settings.

#include "Plugin_Multiplayer/Public/Characters/GPlayerCharacter.h"
#include "Actors/GWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/GCombatComponent.h"
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

	this->CombatComponent = this->CreateDefaultSubobject<UGCombatComponent>("Combat Component");
	// In order for this component to replicate it's variables, the component itself needs to replicate. -Renan
	this->CombatComponent->SetIsReplicated(true);
;}

void AGPlayerCharacter::BeginPlay() {
	Super::BeginPlay();
	this->GetCharacterMovement()->MaxWalkSpeed = this->DefaultMovementSpeed;
}

void AGPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (this->CombatComponent) {
		this->UpdateSpringArmTargetLength(
			this->CombatComponent->bIsAiming ? this->SpringArmAimTargetArmLength :
			this->SpringArmDefaultTargetArmLength,
			DeltaTime
		);
	}
} 

void AGPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// on this macros I cant use the 'this' keyword to refence variables. -Renan
	DOREPLIFETIME_CONDITION(AGPlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AGPlayerCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (this->CombatComponent) {
		this->CombatComponent->PlayerCharacter = this;
	}
}


// =========================================================================
// Player Weapon Detection
// =========================================================================
void AGPlayerCharacter::SetOverlappedWeapon(AGWeapon* weapon) {
	// The weapon in this case can be null because when the player end overlapping with the weapon,
	// I set this reference to nullprt. -Renan
	if (weapon == nullptr && this->OverlappingWeapon) {
		this->OverlappingWeapon->ShowInteractionHud(false);
	}
	
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

void AGPlayerCharacter::OnRep_SetOverlappedWeapon(AGWeapon* lastWeapon) {
	if (this->OverlappingWeapon) {
		this->OverlappingWeapon->ShowInteractionHud(true);
	}

	if (lastWeapon) {
		lastWeapon->ShowInteractionHud(false);
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
	if (this->HasAuthority()) {
		if (this->CombatComponent && !this->CombatComponent->bIsAiming) {
			this->GetCharacterMovement()->MaxWalkSpeed = this->SprintMovementSpeed;
			return;	
		}
	}
	this->ServerSprint();
}

void AGPlayerCharacter::ServerSprint_Implementation() {
	if (this->CombatComponent && !this->CombatComponent->bIsAiming) {
		this->GetCharacterMovement()->MaxWalkSpeed = this->SprintMovementSpeed;
	}
}

void AGPlayerCharacter::StopSprint() {
	if (this->HasAuthority()) {
		if (this->CombatComponent && !this->CombatComponent->bIsAiming) {
			this->GetCharacterMovement()->MaxWalkSpeed = this->DefaultMovementSpeed;
		}
		return;
	}
	this->ServerStopSprint();
}

void AGPlayerCharacter::ServerStopSprint_Implementation() {
	this->GetCharacterMovement()->MaxWalkSpeed = this->DefaultMovementSpeed;
}

void AGPlayerCharacter::PerformJump() {
	this->Jump();
}



// =========================================================================
// Equip Weapon
// =========================================================================
void AGPlayerCharacter::EquipWeapon() {
	if (this->HasAuthority()) {
		if (this->CombatComponent) {
			this->CombatComponent->EquipWeapon(this->OverlappingWeapon);
		}
		return;
	}

	this->Server_EquipWeapon();
}

void AGPlayerCharacter::Server_EquipWeapon_Implementation() {
	if (this->CombatComponent) {
		this->CombatComponent->EquipWeapon(this->OverlappingWeapon);
	}
}

bool AGPlayerCharacter::HasWeaponEquipped() const {
	return this->CombatComponent && this->CombatComponent->EquippedWeapon;
}



// =========================================================================
// Aim Weapon
// =========================================================================
void AGPlayerCharacter::AimWeapon() {
	if (this->CombatComponent && this->CombatComponent->EquippedWeapon) {
		this->CombatComponent->bIsAiming = true;
		this->GetCharacterMovement()->MaxWalkSpeed = this->AimMovementSpeed;
	}
}

void AGPlayerCharacter::StopAimWeapon() {
	if (this->CombatComponent && this->CombatComponent->EquippedWeapon) {
		this->CombatComponent->bIsAiming = false;
		this->GetCharacterMovement()->MaxWalkSpeed = this->DefaultMovementSpeed;
	}	
}

bool AGPlayerCharacter::IsPlayerAiming() const {
	if (this->CombatComponent) {
		return this->CombatComponent->bIsAiming;
	}
	return false;
}

void AGPlayerCharacter::UpdateSpringArmTargetLength(float finalTargetLength, float deltaTime) {
	if (this->CameraSpringArm) {
		if (this->CameraSpringArm->TargetArmLength == finalTargetLength) return;
		
		auto currentTargetLength = this->CameraSpringArm->TargetArmLength;
		auto newTargetLength = FMath::FInterpConstantTo(currentTargetLength, finalTargetLength, deltaTime, this->SpringArmUpdateSpeed);

		this->CameraSpringArm->TargetArmLength = newTargetLength;
	}
}