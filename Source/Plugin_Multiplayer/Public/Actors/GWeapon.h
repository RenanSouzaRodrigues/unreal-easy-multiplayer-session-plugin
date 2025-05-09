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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class USphereComponent> PlayerDetectionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Actor Components")
	TObjectPtr<class UWidgetComponent> PickupWidget;

	UPROPERTY(VisibleAnywhere)
	EGWeaponState CurrentWeaponState { EGWeaponState::Default };

	// this function will be used by the player to show the interaction hud. Online code is a mess. -Renan
	UFUNCTION(BlueprintCallable)
	void ShowInteractionHud(bool value);
	
public:
	AGWeapon();
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnDetectPlayerSphereBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
