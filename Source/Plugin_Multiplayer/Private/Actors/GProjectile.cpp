// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GProjectile.h"

#include "Characters/GPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

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
	
	this->ProjectileMovementComponent = this->CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement Component");
	this->ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AGProjectile::BeginPlay() {
	Super::BeginPlay();

	if (this->TraceParticles) {
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			this->TraceParticles,
			this->GetRootComponent(),
			FName(),
			this->GetActorLocation(),
			this->GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			true
		);
	}
	
	// Needs to have authority to make sure this only runs on server. -Renan
	if (this->HasAuthority()) {
		this->CollisionBox->OnComponentHit.AddDynamic(this, &AGProjectile::OnProjectileHit);
	}
}

void AGProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AGProjectile::OnProjectileHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector normalImpulse, const FHitResult& hitResult) {
	AGPlayerCharacter* playerCharacter = Cast<AGPlayerCharacter>(otherActor);
	if (!playerCharacter) {
		if (this->HitSound) {
			UGameplayStatics::PlaySoundAtLocation(this->GetWorld(), this->HitSound, this->GetActorLocation());
		}
		
		if (this->HitParticles) {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this->GetWorld(), this->HitParticles, this->GetActorLocation(), this->GetActorRotation());			
		}
	}
}

