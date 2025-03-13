// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttachComponent.generated.h"

class AAttachableBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectAttachedSignature, AActor*, AttachedObject);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPHYSICSLAB_API UAttachComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttachComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Katamari")
	bool CanAttachObject(AAttachableBase* ObjectToAttach, UPrimitiveComponent* ObjectComp);

	UFUNCTION(BlueprintCallable, Category = "Katamari")
	void AttachObject(AAttachableBase* ObjectToAttach, UPrimitiveComponent* ObjectComp);

	UFUNCTION(BlueprintCallable, Category = "Katamari")
	void UpdateKatamariSize();

	UPROPERTY(BlueprintAssignable, Category = "Katamari")
	FOnObjectAttachedSignature OnObjectAttached;
private:
	UPROPERTY()
	class UStaticMeshComponent* KatamariMesh;

	UPROPERTY()
	TArray<AAttachableBase*> AttachedObjects;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Katamari")
	float InitialScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Katamari")
	float GrowthRate = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Katamari")
	float MaxScale = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Katamari")
	float AttachSizeRatio = 0.5f;
};
