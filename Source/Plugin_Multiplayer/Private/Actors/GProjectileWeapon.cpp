// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GProjectileWeapon.h"

#include "Actors/GProjectile.h"

// ==============================================
// Unreal Methods
// ==============================================
AGProjectileWeapon::AGProjectileWeapon() {
	PrimaryActorTick.bCanEverTick = true;

	this->ProjectSpawnLocation = this->CreateDefaultSubobject<USceneComponent>("Projectile Spawn Location");
	this->ProjectSpawnLocation->SetupAttachment(this->GetRootComponent());
}

void AGProjectileWeapon::BeginPlay() {
	Super::BeginPlay();
}

void AGProjectileWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AGProjectileWeapon::Fire(const FVector_NetQuantize& hitTarget) {
	Super::Fire(hitTarget);

	APawn* pawnInstigator = Cast<APawn>(this->GetOwner());
	
	if (this->WeaponProjectile && pawnInstigator) {
		FVector spawnLocation = this->ProjectSpawnLocation->GetComponentLocation();

		FVector direction = (hitTarget - spawnLocation).GetSafeNormal();
		FRotator projectileRotation = direction.Rotation();
		
		FActorSpawnParameters params;
		params.Owner = this->GetOwner();
		params.Instigator = Cast<APawn>(this->GetOwner());
		this->GetWorld()->SpawnActor<AGProjectile>(this->WeaponProjectile, spawnLocation, projectileRotation, params);
	}
}
