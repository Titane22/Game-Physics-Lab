// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileMaster.h"

// Sets default values
AProjectileMaster::AProjectileMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
    PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot Point"));
    LodgePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Lodge Point"));
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));

    RootComponent = DefaultSceneRoot;
    PivotPoint->SetupAttachment(DefaultSceneRoot);
    LodgePoint->SetupAttachment(PivotPoint);
    SkeletalMesh->SetupAttachment(LodgePoint);


    static ConstructorHelpers::FObjectFinder<USkeletalMesh> AxeMesh(TEXT("/Game/Character/Weapons/Leviathon/Mesh/Leviathon_Axe.Leviathon_Axe"));
    if (AxeMesh.Succeeded() && SkeletalMesh)
    {
        SkeletalMesh->SetSkeletalMesh(AxeMesh.Object);
    }

    // 스켈레탈 메시의 상대 위치와 회전 설정
    SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    SkeletalMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    // 필요한 경우 스케일도 설정
    SkeletalMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

    if (SkeletalMesh)
    {
        // 스켈레탈 메시 컴포넌트에만 물리 설정 적용
        SkeletalMesh->SetSimulatePhysics(false);
        SkeletalMesh->SetEnableGravity(false);
        SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        SkeletalMesh->SetMobility(EComponentMobility::Movable);
    }

}

// Called when the game starts or when spawned
void AProjectileMaster::BeginPlay()
{
	Super::BeginPlay();
}

