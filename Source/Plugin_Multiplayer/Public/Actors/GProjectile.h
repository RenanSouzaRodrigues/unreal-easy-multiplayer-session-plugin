// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GProjectile.generated.h"

UCLASS()
class PLUGIN_MULTIPLAYER_API AGProjectile : public AActor {
	GENERATED_BODY()

	// ===================================================
	// Unreal Methods
	// ===================================================
public:
	AGProjectile();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
	
	// ===================================================
	// Actor Core Components
	// ===================================================
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere, Category="Actor Components")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;
};
