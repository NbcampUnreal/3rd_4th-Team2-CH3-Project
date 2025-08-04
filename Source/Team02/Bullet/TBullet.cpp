// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet/TBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ATBullet::ATBullet()
{
	// [1] 충돌체 생성 및 세팅
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.f);
	CollisionComp->SetCollisionProfileName("BlockAllDynamic");
	RootComponent = CollisionComp;

	// [2] (옵션) Mesh
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(CollisionComp);

	// [3] Projectile Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// [4] LifeSpan
	SetLifeSpan(2.f);

	// [5] 충돌 시 OnHit 함수 바인딩
	CollisionComp->OnComponentHit.AddDynamic(this, &ATBullet::OnHit);

}

void ATBullet::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	FString Msg = FString::Printf(TEXT("총알 데미지: %f (타겟: %s)"), Damage, *OtherActor->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Msg);
	if (OtherActor && OtherActor != this)
	{
		// [A] 데미지 처리, 이펙트, 사운드 등
		// [B] 총알 파괴
		Destroy();
	}
}

void ATBullet::SetDamage(float InDamage)
{
	Damage = InDamage;
}

void ATBullet::SetRange(float InRange)
{
	Range = InRange;
}
