// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GProjectile.h"

#include "Components/BoxComponent.h"

// ===================================================
// Unreal Methods
// ===================================================
AGProjectile::AGProjectile() {
	PrimaryActorTick.bCanEverTick = true;

	this->CollisionBox = this->CreateDefaultSubobject<UBoxComponent>("Collision Box");
	this->SetRootComponent(this->CollisionBox);
	this->CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	this->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Block);
	this->CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
}

void AGProjectile::BeginPlay() {
	Super::BeginPlay();
}

void AGProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

