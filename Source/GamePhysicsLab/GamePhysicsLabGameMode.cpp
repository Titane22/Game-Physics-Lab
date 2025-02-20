// Copyright Epic Games, Inc. All Rights Reserved.

#include "GamePhysicsLabGameMode.h"
#include "GamePhysicsLabCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGamePhysicsLabGameMode::AGamePhysicsLabGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_GOW_Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
