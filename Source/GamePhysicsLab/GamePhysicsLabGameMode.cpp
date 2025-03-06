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
		// PIE 모드에서 접두사 제거
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
