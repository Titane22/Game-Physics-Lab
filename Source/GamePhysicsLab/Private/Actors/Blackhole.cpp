// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Blackhole.h"
#include "Actors/AttachableBase.h"

// Sets default values
ABlackhole::ABlackhole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root Component"));
	StaticMeshComponent->SetNotifyRigidBodyCollision(true); 
	RootComponent = StaticMeshComponent;

	GravitySphere = CreateDefaultSubobject<USphereComponent>(TEXT("Gravity Sphere"));
	GravitySphere->SetupAttachment(RootComponent);
	GravitySphere->SetSphereRadius(GravityDistance);
}

// Called when the game starts or when spawned
void ABlackhole::BeginPlay()
{
	Super::BeginPlay();
	
	// 방법 1: 소유자 대신 자신의 컴포넌트 사용
	StaticMeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComponent)
	{
		StaticMeshComponent->OnComponentHit.AddDynamic(this, &ABlackhole::OnHit);
		StaticMeshComponent->SetNotifyRigidBodyCollision(true);
		UE_LOG(LogTemp, Warning, TEXT("블랙홀: 자체 스태틱 메시 컴포넌트 설정 완료"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("블랙홀: 스태틱 메시 컴포넌트를 찾을 수 없습니다!"));
	}
	
	// 또는 방법 2: 소유자 널 체크 추가
	/*
	AActor* Owner = GetOwner();
	if (Owner)
	{
		if (UStaticMeshComponent* OwnerMesh = Cast<UStaticMeshComponent>(Owner->GetComponentByClass(UStaticMeshComponent::StaticClass())))
		{
			OwnerMesh->OnComponentHit.AddDynamic(this, &ABlackhole::OnHit);
			OwnerMesh->SetNotifyRigidBodyCollision(true);
			StaticMeshComponent = OwnerMesh;
			UE_LOG(LogTemp, Warning, TEXT("블랙홀: 소유자의 스태틱 메시 컴포넌트 설정 완료"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("블랙홀: 소유자에게 스태틱 메시 컴포넌트가 없습니다!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("블랙홀: 소유자가 없습니다!"));
	}
	*/
}


void ABlackhole::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;
	if (AAttachableBase* AttachableActor = Cast<AAttachableBase>(OtherActor))
	{
		AttachableActor->Destroy();
	}
}

// Called every frame
void ABlackhole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> OverlappingActors;
	GravitySphere->GetOverlappingActors(OverlappingActors);

	if (OverlappingActors.Num() > 0)
	{
		for (AActor* overlappedActor : OverlappingActors)
		{
			if (overlappedActor && overlappedActor != this)
			{
				if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(overlappedActor->GetRootComponent()))
				{
					if (!primComp->IsSimulatingPhysics())
						continue;
					FVector direction = overlappedActor->GetActorLocation() - GetActorLocation();
					if (direction.Normalize())
					{
						primComp->AddForce(direction * GravityStrength * primComp->GetMass());
					}
				}
			}
		}
	}
}

