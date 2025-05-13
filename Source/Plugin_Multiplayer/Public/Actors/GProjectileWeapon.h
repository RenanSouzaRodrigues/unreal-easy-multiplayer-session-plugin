// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GWeapon.h"
#include "GProjectileWeapon.generated.h"

UCLASS()
class PLUGIN_MULTIPLAYER_API AGProjectileWeapon : public AGWeapon {
	GENERATED_BODY()

	// ==============================================
	// Unreal Methods
	// ==============================================
public:
	AGProjectileWeapon();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
