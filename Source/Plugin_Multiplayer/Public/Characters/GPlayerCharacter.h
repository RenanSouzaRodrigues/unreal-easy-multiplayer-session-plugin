// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GPlayerCharacter.generated.h"

UCLASS()
class PLUGIN_MULTIPLAYER_API AGPlayerCharacter : public ACharacter {
	GENERATED_BODY()

	// ================================================
	// Unreal Methods
	// ================================================
public:
	AGPlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// this function is where I register variables to be replicated **Everything using UPROPERTY(Replicated)**
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	
	
	// ================================================
	// Actor Core Components
	// ================================================
public:
	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class USpringArmComponent> CameraSpringArm;

	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class UCameraComponent> PlayerCamera;

	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class UGCombatComponent> CombatComponent;
	

	
	// ================================================
	// Actor Properties
	// ================================================
public:
	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float DefaultMovementSpeed { 450 };
	
	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float SprintMovementSpeed { 800 };

	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float AimMovementSpeed { 200 };

	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float SpringArmDefaultTargetArmLength { 450 };
	
	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float SpringArmAimTargetArmLength { 150 };

	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float SpringArmUpdateSpeed { 10 };

	

	// =================================================================
	// Player Basic Movement
	// =================================================================
public:
	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void MovePlayer(float valueX, float valueY);

	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void MoveCamera(float valueX, float valueY);

	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void PerformJump();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE float GetPlayerVelocityLength() const { return this->GetVelocity().Length(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE bool IsPlayerInAir() const { return this->GetCharacterMovement()->IsFalling(); };


	
	// =================================================================
	// Player Sprint
	// =================================================================
private:
	UPROPERTY(ReplicatedUsing=OnRep_SetIsSprinting)
	bool bIsSprinting { false };

	void SetIsSprinting(bool value);
	
	UFUNCTION()
	void OnRep_SetIsSprinting();

public:
	void UpdateMovementSpeed();
	
	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void Sprint();

	UFUNCTION(Server, Reliable)
	void ServerSprint();

	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void StopSprint();

	UFUNCTION(Server, Reliable)
	void ServerStopSprint();

	

	

	// ==============================================================
	// Player Weapon Detection
	// ==============================================================
public:
	UPROPERTY(ReplicatedUsing=OnRep_SetOverlappedWeapon)
	class AGWeapon* OverlappingWeapon;

	void SetOverlappedWeapon(AGWeapon* weapon);

	UFUNCTION()
	void OnRep_SetOverlappedWeapon(AGWeapon* lastWeaponPointer);
	

	
	// =========================================================================
	// Equip Weapon
	// =========================================================================
public:
	UFUNCTION(BlueprintCallable, Category="Equip Weapon")
	void EquipWeapon();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Equip Weapon")
	bool HasWeaponEquipped() const;


	
	// =========================================================================
	// Aim Weapon
	// =========================================================================
public:
	UFUNCTION(BlueprintCallable, Category="Aim Weapon")
	void AimWeapon();

	UFUNCTION(BlueprintCallable, Category="Aim Weapon")
	void StopAimWeapon();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPlayerAiming() const;

private:
	void UpdateSpringArmTargetLength(float finalTargetLength, float deltaTime);
	

	
	// =========================================================================
	// Fire Weapon
	// =========================================================================
private:
	UPROPERTY(EditAnywhere, Category="Actor Animations")
	TObjectPtr<class UAnimMontage> FireAnimationMontage;
	
public:
	UFUNCTION(BlueprintCallable, Category="Fire Weapon")
	void FireWeapon();
	
	void PlayFireMontage();
};