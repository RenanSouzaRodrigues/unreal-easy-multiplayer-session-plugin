// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GWeapon.h"

#include "Components/SphereComponent.h"

AGWeapon::AGWeapon() {
	this->PrimaryActorTick.bCanEverTick = false;

	// Define the Actor Authority and server replication
	this->bReplicates = true;

	this->WeaponMesh = this->CreateDefaultSubobject<UStaticMeshComponent>("Weapon Mesh");
	this->SetRootComponent(this->WeaponMesh);
	this->WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	this->WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	// This is the initial state of collision so players dont trip on the gun. -Renan 
	this->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	this->PlayerDetectionSphere = this->CreateDefaultSubobject<USphereComponent>("Player Detection Sphere");
	this->PlayerDetectionSphere->SetupAttachment(this->GetRootComponent());

	// Disables the player detection sphere collision to be sure this only runs on the server. -Renan
	this->PlayerDetectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->PlayerDetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGWeapon::BeginPlay() {
	Super::BeginPlay();

	// If this actor is running on the server, them I enable the collision of the player detection sphere. -Renan
	if (this->HasAuthority()) {
		this->PlayerDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->PlayerDetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
} 

void AGWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

