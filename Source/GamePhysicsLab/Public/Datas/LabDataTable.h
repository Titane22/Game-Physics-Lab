// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "./GamePhysicsLab/GamePhysicsLabCharacter.h"
#include "LabDataTable.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString CharacterName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    UTexture2D* CharacterPortrait;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TSubclassOf<AGamePhysicsLabCharacter> CharacterClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString AssociatedLevelName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FText CharacterDescription;
    
};

UCLASS()
class GAMEPHYSICSLAB_API ULabDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
