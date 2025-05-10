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

	
	
	// ================================================
	// Actor Core Components
	// ================================================
public:
	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class USpringArmComponent> CameraSpringArm;

	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class UCameraComponent> PlayerCamera;
	

	
	// ================================================
	// Actor Properties
	// ================================================
public:
	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float DefaultMovementSpeed { 450 };
	
	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float SprintMovementSpeed { 800 };


	
	// ==============================================================
	// Player Weapon Detection
	// ==============================================================
public:
	UPROPERTY(ReplicatedUsing=OnRep_SetOverlappedWeapon)
	class AGWeapon* OverlappingWeapon;

	void SetOverlappedWeapon(AGWeapon* weapon);

	UFUNCTION()
	void OnRep_SetOverlappedWeapon(AGWeapon* lastWeaponPointer);

	

	// =================================================================
	// Player Locomotion
	// =================================================================
private:
	bool bIsSprinting { false };
	
public:
	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void MovePlayer(float valueX, float valueY);

	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void MoveCamera(float valueX, float valueY);

	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void PerformJump();

	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void Sprint();

	UFUNCTION(BlueprintCallable, Category="Player Locomotion")
	void StopSprint();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE float GetPlayerVelocityLength() const { return this->GetVelocity().Length(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE bool IsPlayerSprinting() const { return this->bIsSprinting; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE bool IsPlayerInAir() const { return this->GetCharacterMovement()->IsFalling(); };
};
