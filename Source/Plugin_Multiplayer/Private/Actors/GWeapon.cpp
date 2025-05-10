// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GWeapon.h"
#include "Characters/GPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// =============================================================
// Unreal Methods
// =============================================================
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

	this->PickupWidget = this->CreateDefaultSubobject<UWidgetComponent>("Pickup Widget");
	this->PickupWidget->SetupAttachment(this->WeaponMesh);
}

void AGWeapon::BeginPlay() {
	Super::BeginPlay();

	// At the beginning, the interaction hud is aways hidden. -Renan
	this->ShowInteractionHud(false);
	
	// If this actor is running on the server, them I enable the collision of the player detection sphere. -Renan
	if (this->HasAuthority() && this->PlayerDetectionSphere) {
		this->PlayerDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->PlayerDetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		this->PlayerDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGWeapon::OnDetectPlayerSphereBeginOverlap);
		// this->PlayerDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AGWeapon::OnDetectPlayerSphereEndOverlap);
	}
}



// =============================================================
// Pickup Widget
// =============================================================
void AGWeapon::OnDetectPlayerSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	// validates if the pawn overlapping is the player character
	AGPlayerCharacter* playerCharacter = Cast<AGPlayerCharacter>(OtherActor);
	if (playerCharacter) {
		playerCharacter->SetOverlappedWeapon(this);
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, TEXT("player overlapped with weapon"));
	}
} 

void AGWeapon::ShowInteractionHud(bool value) {
	if (this->PickupWidget) this->PickupWidget->SetVisibility(value);
}
