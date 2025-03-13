// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AttachableSpawnVolume.h"
#include "Actors/AttachableBase.h"

// Sets default values
AAttachableSpawnVolume::AAttachableSpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 박스 컴포넌트 생성 및 루트로 설정
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = SpawnVolume;
	
	// 기본 박스 크기 설정
	SpawnVolume->SetBoxExtent(FVector(500.0f, 500.0f, 100.0f));
	
	// 충돌 설정 - 오버랩 이벤트만 활성화
	SpawnVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpawnVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	// 디버그 모드에서 볼륨 시각화 (반투명 박스로 표시)
	SpawnVolume->bHiddenInGame = false;
	SpawnVolume->SetVisibility(true);
	
	// 기본값 설정
	NumberToSpawn = 10;
	MinSpawnDistance = 100.0f;
	bSpawnOnBeginPlay = true;
}

// Called when the game starts or when spawned
void AAttachableSpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
	if (bSpawnOnBeginPlay)
	{
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(
			timerHandle,
			this,
			&AAttachableSpawnVolume::SpawnAttachables,
			1.5f,
			false
		);

	}

	if (SpawnVolume)
	{
		FVector origin = SpawnVolume->GetComponentLocation();
		FVector extent = SpawnVolume->GetScaledBoxExtent();

		DrawDebugBox(GetWorld(), origin, extent, FColor::Green, false, 10.0f, 0, 2.0f);
	}
}

// Called every frame
void AAttachableSpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AAttachableSpawnVolume::GetRandomPointInVolume()
{
	if (!SpawnVolume)
		return FVector::ZeroVector;

	FVector boxOrigin = SpawnVolume->GetComponentLocation();
	FVector boxExtent = SpawnVolume->GetScaledBoxExtent();

	FVector randomPoint = boxOrigin + FVector(
		FMath::RandRange(-boxExtent.X, boxExtent.X),
		FMath::RandRange(-boxExtent.Y, boxExtent.Y),
		FMath::RandRange(-boxExtent.Z, boxExtent.Z)
	);

	return randomPoint;
}

void AAttachableSpawnVolume::SpawnAttachables()
{
	if (AttachableClassesToSpawn.Num() > 0)
	{
		ClearSpawnedAttachables();
		for (int32 spawnIdx = 0; spawnIdx < NumberToSpawn; spawnIdx++)
		{
			if (AAttachableBase* spawnedActor = SpawnSingleAttachable())
			{
				SpawnedActors.Add(spawnedActor);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AttachableClassesToSpawn.Num() Is 0"));
	}
}

AAttachableBase* AAttachableSpawnVolume::SpawnSingleAttachable()
{
	int32 randomIndex = FMath::RandRange(0, AttachableClassesToSpawn.Num() - 1);
	auto spawnToActorClass = AttachableClassesToSpawn[randomIndex];

	FVector spawnLocation = FVector::ZeroVector;
	bool bValidLocation = false;
	FRotator spawnRotation = FRotator(
		FMath::RandRange(-100.0f, 100.0f),
		FMath::RandRange(-100.0f, 100.0f),
		FMath::RandRange(-100.0f, 100.0f)
	);
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int32 attempt = 0; attempt < NumberToSpawn; attempt++)
	{
		spawnLocation = GetRandomPointInVolume();
		if (IsLocationValid(spawnLocation))
		{
			bValidLocation = true;
			break;
		}
	}
	if (!bValidLocation)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("SpawnSingleAttachable() bValidLocation is false"));

		return nullptr;
	}
	if (AAttachableBase* spawnActor = GetWorld()->SpawnActor<AAttachableBase>(spawnToActorClass, spawnLocation, spawnRotation, spawnParams))
	{
		if (UStaticMeshComponent* meshComp = spawnActor->GetStaticMeshComponent())
		{
			//meshComp->SetRelativeLocation(FVector::ZeroVector);
			meshComp->AddImpulse(FVector(
				FMath::RandRange(-10.0f, 10.0f),
				FMath::RandRange(-10.0f, 10.0f),
				FMath::RandRange(0.0f, 10.0f)
			));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("SpawnSingleAttachable() is Successed"));
		UE_LOG(LogTemp, Warning, TEXT("SpawnSingleAttachable() is Successed"));
		return spawnActor;
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("SpawnSingleAttachable() is Failed"));
	return nullptr;
}

void AAttachableSpawnVolume::ClearSpawnedAttachables()
{
	for (AAttachableBase* actor : SpawnedActors)
	{
		if(actor)
			actor->Destroy();
	}
	SpawnedActors.Empty();
}

bool AAttachableSpawnVolume::IsLocationValid(const FVector& Location)
{
    // 기존 스폰된 객체와의 거리 확인
    for (AAttachableBase* ExistingActor : SpawnedActors)
    {
        if (ExistingActor)
        {
            // 두 위치 간의 거리 계산
            float Distance = FVector::DistSquared(ExistingActor->GetActorLocation(), Location);
            float MinDistanceSquared = MinSpawnDistance * MinSpawnDistance;
            
            // 최소 거리보다 가까우면 위치가 유효하지 않음
            if (Distance < MinDistanceSquared)
            {
                return false;
            }
        }
    }
    
    return true;
}


