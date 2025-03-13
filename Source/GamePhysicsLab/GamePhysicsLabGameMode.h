// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePhysicsLabGameMode.generated.h"

UCLASS(minimalapi)
class AGamePhysicsLabGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGamePhysicsLabGameMode();
	
	virtual void StartPlay() override;
};



