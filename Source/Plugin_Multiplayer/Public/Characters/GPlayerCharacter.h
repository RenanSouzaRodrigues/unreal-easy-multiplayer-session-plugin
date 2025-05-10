// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GPlayerCharacter.generated.h"

UCLASS()
class PLUGIN_MULTIPLAYER_API AGPlayerCharacter : public ACharacter {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class USpringArmComponent> CameraSpringArm;

	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class UCameraComponent> PlayerCamera;

	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float DefaultMovementSpeed { 450 };
	
	UPROPERTY(EditAnywhere, Category="Actor Configuration")
	float SprintMovementSpeed { 800 };

	// ==============================================================
	// Replicated Variables
	// ==============================================================
	UPROPERTY(ReplicatedUsing=OnRep_WeaponOverlapped)
	class AGWeapon* OverlappingWeapon;

private:
	bool bIsSprinting { false };
	
public:
	AGPlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// this function is where I register variables to be replicated **Everything using UPROPERTY(Replicated)**
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================
	// REP NOTIFIES
	// =================================================================
	UFUNCTION()
	void OnRep_WeaponOverlapped();


	// =================================================================
	// CLASS METHODS
	// =================================================================
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
	
	
	// Getters and Setters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE float GetPlayerVelocityLength() const { return this->GetVelocity().Length(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE bool IsPlayerSprinting() const { return this->bIsSprinting; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	FORCEINLINE bool IsPlayerInAir() const { return this->GetCharacterMovement()->IsFalling(); };
	
	void SetOverlappedWeapon(AGWeapon* weapon);
};
