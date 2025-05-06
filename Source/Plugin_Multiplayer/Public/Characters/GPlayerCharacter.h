// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

private:
	bool bIsSprinting { false };
	
public:
	AGPlayerCharacter();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	float GetPlayerVelocityLength() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	bool IsPlayerSprinting() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Getters")
	bool IsPlayerInAir() const;

protected:
	virtual void BeginPlay() override;

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
	
public:
	virtual void Tick(float DeltaTime) override;
};
