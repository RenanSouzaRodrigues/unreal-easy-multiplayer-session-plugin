// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/GCombatComponent.h"

#include "Actors/GWeapon.h"
#include "Camera/CameraComponent.h"
#include "Characters/GPlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"

// ==================================================
// Unreal Methods
// ==================================================	
UGCombatComponent::UGCombatComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UGCombatComponent::BeginPlay() {
	Super::BeginPlay();
}

void UGCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	this->TraceProjectileHitDestination(TraceHitResult);
}

void UGCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UGCombatComponent, bIsAiming);
}


// ==================================================
// Equip Weapon Actions
// ==================================================
void UGCombatComponent::EquipWeapon(AGWeapon* newWeapon) {
	if (this->PlayerCharacter == nullptr || newWeapon == nullptr) return;
	
	this->EquippedWeapon = newWeapon;
	this->EquippedWeapon->SetWeaponState(EGWeaponState::Equipped);
	
	const USkeletalMeshSocket* weaponSocket = this->PlayerCharacter->GetMesh()->GetSocketByName(this->PlayerWeaponSocket);
	if (weaponSocket) {
		weaponSocket->AttachActor(this->EquippedWeapon, this->PlayerCharacter->GetMesh());
	}
	
	this->EquippedWeapon->SetOwner(this->PlayerCharacter);
}



// ==================================================
// Equip Aim Actions
// ==================================================
void UGCombatComponent::OnRep_SetAiming() {
	if (this->PlayerCharacter) {
		this->PlayerCharacter->UpdateMovementSpeed();
	}
}

void UGCombatComponent::SetAiming(bool value) {
	this->bIsAiming = value;
}

void UGCombatComponent::TraceProjectileHitDestination(FHitResult& hitResult) {
	if (this->PlayerCharacter) {
		FVector startLocation = this->PlayerCharacter->PlayerCamera->GetComponentLocation();
		FVector endLocation = startLocation + (this->PlayerCharacter->PlayerCamera->GetForwardVector() * 10000.f);
		
		bool bHit = this->GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility);

		if (!hitResult.bBlockingHit) {
			hitResult.ImpactPoint = endLocation;
			this->TraceHitTarget = endLocation;
			return;
		}

		this->TraceHitTarget = hitResult.ImpactPoint;
		DrawDebugSphere(this->GetWorld(), hitResult.ImpactPoint, 32, 8, FColor::Red);
	}
	
}

void UGCombatComponent::ServerSetAiming_Implementation(bool value) {
	this->bIsAiming = value;
	if (this->PlayerCharacter) this->PlayerCharacter->UpdateMovementSpeed();
}



// ==================================================
// Fire Weapon Actions
// ==================================================
void UGCombatComponent::MulticastFireWeapon_Implementation() {
	if (this->EquippedWeapon && this->bIsAiming) {
		this->EquippedWeapon->Fire(this->TraceHitTarget);
		if (this->PlayerCharacter) this->PlayerCharacter->PlayFireMontage(this->EquippedWeapon->GetWeaponFireType());
	}
}

void UGCombatComponent::ServerFireWeapon_Implementation() {
	this->MulticastFireWeapon();
}
