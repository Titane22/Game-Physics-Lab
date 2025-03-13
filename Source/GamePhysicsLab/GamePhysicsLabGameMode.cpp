// Copyright Epic Games, Inc. All Rights Reserved.

#include "GamePhysicsLabGameMode.h"
#include "GamePhysicsLabCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

AGamePhysicsLabGameMode::AGamePhysicsLabGameMode()
{
	// 현재 맵 이름 가져오기
	FString CurrentMapName = GetWorld() ? GetWorld()->GetMapName() : TEXT("");
	// 맵 이름에서 경로 부분 제거 (PIE 모드에서는 UEDPIE_0_ 같은 접두사가 붙을 수 있음)
	CurrentMapName = FPackageName::GetShortName(CurrentMapName);
	if (CurrentMapName.StartsWith(TEXT("UEDPIE_")))
	{
		// PIE 모드에서 접두사 제거e
		int32 UnderscorePos = CurrentMapName.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);
		if (UnderscorePos != INDEX_NONE)
		{
			int32 SecondUnderscorePos = CurrentMapName.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, UnderscorePos + 1);
			if (SecondUnderscorePos != INDEX_NONE)
			{
				CurrentMapName = CurrentMapName.Mid(SecondUnderscorePos + 1);
			}
		}
	}

	// 맵 이름에 따라 다른 폰 클래스 설정
	if (CurrentMapName == TEXT("ThirdPersonMap"))
	{
		// ThirdPersonMap인 경우 GOW 캐릭터 사용
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_GOW_Character"));
		if (PlayerPawnBPClass.Class != NULL)
		{
			DefaultPawnClass = PlayerPawnBPClass.Class;
		}
	}
	else if (CurrentMapName == TEXT("CannonMap"))
	{
		// ThirdPersonMap인 경우 GOW 캐릭터 사용
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_PirateCharacter"));
		if (PlayerPawnBPClass.Class != NULL)
		{
			DefaultPawnClass = PlayerPawnBPClass.Class;
		}
	}
	else if (CurrentMapName == TEXT("KatamariBallMap"))
	{
		// 경로가 정확한지 확인하세요
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/Charaters/BP_KatamariBall"));
		if (PlayerPawnBPClass.Class != NULL)
		{
			DefaultPawnClass = PlayerPawnBPClass.Class;
			UE_LOG(LogTemp, Warning, TEXT("카타마리 볼 폰 클래스 로드 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("카타마리 볼 폰 클래스 로드 실패"));
		}
	}
	else
	{
		// 다른 맵인 경우 기본 ThirdPersonCharacter 사용
		static ConstructorHelpers::FClassFinder<APawn> DefaultPlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
		if (DefaultPlayerPawnBPClass.Class != NULL)
		{
			DefaultPawnClass = DefaultPlayerPawnBPClass.Class;
		}
	}
}

void AGamePhysicsLabGameMode::StartPlay()
{
	// 기존 폰 생성 방지
	// 주의: 이렇게 하면 Super::StartPlay()에서 자동으로 폰을 생성하지 않음
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->GetPawn())
	{
		// 이미 폰이 있다면 제거
		PC->GetPawn()->Destroy();
	}
	
	// 이제 Super::StartPlay() 호출
	Super::StartPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("StartPlay 호출됨"));
	
	// 현재 맵 이름 확인
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentMapName = World->GetMapName();
		CurrentMapName.RemoveFromStart(World->StreamingLevelsPrefix);
		
		if (CurrentMapName == TEXT("KatamariBallMap"))
		{
			// 이미 존재하는 카타마리 볼 확인
			TArray<AActor*> ExistingBalls;
			UGameplayStatics::GetAllActorsOfClass(World, DefaultPawnClass, ExistingBalls);
			
			// 이미 존재하는 볼이 있으면 추가 생성하지 않음
			if (ExistingBalls.Num() > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("이미 %d개의 카타마리 볼이 존재합니다."), ExistingBalls.Num());
				
				// 첫 번째 볼만 남기고 나머지 제거
				for (int32 i = 1; i < ExistingBalls.Num(); i++)
				{
					ExistingBalls[i]->Destroy();
				}
				
				// 플레이어 컨트롤러가 첫 번째 볼에 빙의
				PC = UGameplayStatics::GetPlayerController(World, 0);
				if (PC && ExistingBalls[0])
				{
					PC->Possess(Cast<APawn>(ExistingBalls[0]));
					UE_LOG(LogTemp, Warning, TEXT("기존 카타마리 볼에 빙의함"));
				}
				
				return;
			}
			
			// 기존 볼이 없으면 새로 생성
			UE_LOG(LogTemp, Warning, TEXT("카타마리 맵 확인됨, 폰 생성 시도"));
			
			// PlayerStart 찾기
			AActor* StartSpot = FindPlayerStart(nullptr, FString());
			if (StartSpot)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerStart 찾음: %s"), *StartSpot->GetName());
				
				// 폰 수동 생성
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				
				APawn* NewPawn = World->SpawnActor<APawn>(DefaultPawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);
				if (NewPawn)
				{
					UE_LOG(LogTemp, Warning, TEXT("폰 생성 성공: %s"), *NewPawn->GetName());
					
					// 플레이어 컨트롤러 가져와서 빙의
					PC = UGameplayStatics::GetPlayerController(World, 0);
					if (PC)
					{
						PC->Possess(NewPawn);
						UE_LOG(LogTemp, Warning, TEXT("플레이어 컨트롤러가 폰에 빙의함"));
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("플레이어 컨트롤러를 찾을 수 없음"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("폰 생성 실패"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerStart를 찾을 수 없음"));
			}
		}
	}
}
