// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/PirateCannon.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"

#include "CannonBasedF.generated.h"

class UInputMappingContext;
class UInputAction;
class ACannonBall;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class GAMEPHYSICSLAB_API ACannonBasedF : public APirateCannon
{
	GENERATED_BODY()
protected:
	ACannonBasedF();
	
	virtual void BeginPlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* GunSmokeEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> ShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ReloadWidgetClass;

	UPROPERTY()
	UUserWidget* ReloadWidget;

	UPROPERTY()
	FTimerHandle ReloadTimerHandle;

	UPROPERTY()
	FTimerHandle WidgetTimerHandle;
public:
	UFUNCTION()
	void UpdateReloadProgress();

	void Fire();
	
	void Reload();
	
	float ReloadTime = 5.0f;

	float CurrentReloadTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Gameplay")
	float ReloadProgress = 0.0f;
};
