// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AttachableBase.h"

// Sets default values
AAttachableBase::AAttachableBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스태틱 메시 컴포넌트를 생성하고 루트로 설정
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMeshComponent; // 스태틱 메시를 루트로 설정
	
	// 물리 시뮬레이션 설정
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetEnableGravity(true);
	
	// 충돌 설정
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
}

// Called when the game starts or when spawned
void AAttachableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAttachableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

