// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePhysicsLabGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPHYSICSLAB_API AGamePhysicsLabGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGamePhysicsLabGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void StartPlay() override;

private:
	bool bIsKatamariBallMap = false;
	TSubclassOf<APawn> KatamariBallClass = nullptr;
};



