// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GProjectileWeapon.h"

// ==============================================
// Unreal Methods
// ==============================================

AGProjectileWeapon::AGProjectileWeapon() {
	PrimaryActorTick.bCanEverTick = true;
}

void AGProjectileWeapon::BeginPlay() {
	Super::BeginPlay();
}

void AGProjectileWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}