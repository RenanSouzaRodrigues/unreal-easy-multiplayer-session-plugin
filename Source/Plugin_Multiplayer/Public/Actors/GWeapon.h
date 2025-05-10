// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GWeapon.generated.h"

// Enum used to defined the current state of the weapon. -Renan
UENUM(Blueprintable, BlueprintType)
enum class EGWeaponState : uint8 {
	Default,
	Equipped,
	Dropped,
};

UCLASS()
class PLUGIN_MULTIPLAYER_API AGWeapon : public AActor {
	GENERATED_BODY()

	// ==================================================
	// Unreal Methods
	// ==================================================
public:
	AGWeapon();
	virtual void BeginPlay() override;


	
	// ==================================================
	// Actor Core Components
	// ==================================================
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class USphereComponent> PlayerDetectionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class UWidgetComponent> PickupWidget;


	
	// ==================================================
	// Actor Properties
	// ==================================================
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EGWeaponState CurrentWeaponState { EGWeaponState::Default };


	
	// ==================================================
	// Pickup Widget
	// ==================================================
public:
	UFUNCTION()
	virtual void OnDetectPlayerSphereBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable)
	void ShowInteractionHud(bool value); // This is a bad way of doing things, but replication is on the character at this points. -Renan
};
