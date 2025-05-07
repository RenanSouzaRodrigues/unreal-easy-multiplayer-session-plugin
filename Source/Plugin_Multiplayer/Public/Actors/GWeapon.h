// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GWeapon.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EGWeaponState : uint8 {
	Default,
	Equipped,
	Dropped,
};

UCLASS()
class PLUGIN_MULTIPLAYER_API AGWeapon : public AActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class USphereComponent> PlayerDetectionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

private:
	EGWeaponState CurrentWeaponState { EGWeaponState::Default };
	
public:
	AGWeapon();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
