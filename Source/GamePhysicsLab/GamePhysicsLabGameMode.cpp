// Copyright Epic Games, Inc. All Rights Reserved.

#include "GamePhysicsLabGameMode.h"
#include "GamePhysicsLabCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Characters/KatamariBall.h"

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
			KatamariBallClass = PlayerPawnBPClass.Class;
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

void AGamePhysicsLabGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	// 맵 이름에서 접두사 제거
	FString CleanMapName = MapName;
	if (GetWorld())
	{
		CleanMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	}
	
	// 카타마리 맵인지 확인
	bIsKatamariBallMap = (CleanMapName == TEXT("KatamariBallMap"));
	
	UE_LOG(LogTemp, Warning, TEXT("InitGame - 맵: %s, 카타마리 맵 여부: %s"), 
		*CleanMapName, bIsKatamariBallMap ? TEXT("예") : TEXT("아니오"));
}

void AGamePhysicsLabGameMode::StartPlay()
{
	// 기본 StartPlay 호출
	Super::StartPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("GameMode StartPlay 호출됨"));
	
	// 카타마리 맵이 아니면 추가 로직 실행하지 않음
	if (!bIsKatamariBallMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("카타마리 맵이 아니므로 기본 빙의 사용"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("카타마리 맵 확인됨, 특수 빙의 로직 시작"));
	
	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 컨트롤러를 찾을 수 없음"));
		return;
	}
	
	// 현재 빙의된 폰 확인
	APawn* CurrentPawn = PC->GetPawn();
	
	// 이미 카타마리 볼에 빙의되어 있는지 확인
	if (CurrentPawn && KatamariBallClass && CurrentPawn->IsA(KatamariBallClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 카타마리 볼에 빙의되어 있음: %s"), *CurrentPawn->GetName());
		return;
	}
	
	// PlayerStart 찾기
	AActor* StartSpot = FindPlayerStart(PC, FString());
	if (!StartSpot)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStart를 찾을 수 없음"));
		return;
	}
	
	// 기존 폰 제거
	if (CurrentPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("기존 폰 제거: %s"), *CurrentPawn->GetName());
		CurrentPawn->Destroy();
		PC->UnPossess();
	}
	
	// 카타마리 볼 클래스 확인
	if (!KatamariBallClass)
	{
		UE_LOG(LogTemp, Error, TEXT("카타마리 볼 클래스가 설정되지 않음"));
		return;
	}
	
	// 새 폰 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(KatamariBallClass, 
		StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);
	
	if (NewPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("카타마리 볼 생성 성공: %s"), *NewPawn->GetName());
		
		// 빙의 시도
		PC->Possess(NewPawn);
		
		// 빙의 확인
		if (PC->GetPawn() == NewPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("카타마리 볼 빙의 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("카타마리 볼 빙의 실패"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("카타마리 볼 생성 실패"));
	}
}
