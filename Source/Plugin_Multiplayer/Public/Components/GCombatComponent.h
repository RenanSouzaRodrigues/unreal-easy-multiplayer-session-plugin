// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GCombatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLUGIN_MULTIPLAYER_API UGCombatComponent : public UActorComponent {
	GENERATED_BODY()

	// ==================================================
	// Unreal Methods
	// ==================================================	
public:
	UGCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// This component can only be used for the player character, there is no problem to make it a friend class. -Renan
	friend class AGPlayerCharacter;

	
	
	// ==================================================
	// Equip Weapon Actions
	// ==================================================
private:
	class AGPlayerCharacter* PlayerCharacter;

	UPROPERTY(Replicated)
	class AGWeapon* EquippedWeapon;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PlayerWeaponSocket { FName("WeaponSocket") };
	
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AGWeapon* newWeapon);



	// ==================================================
	// Equip Aim Actions
	// ==================================================
private:
	UPROPERTY(ReplicatedUsing=OnRep_SetAiming)
	bool bIsAiming { false };

	UFUNCTION()
	void OnRep_SetAiming();
	
protected:
	void SetAiming(bool value);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool value);



	// ==================================================
	// Fire Weapon Actions
	// ==================================================
protected:
	void FireWeapon();

	UFUNCTION(Server, Reliable)
	void ServerFireWeapon();
};
