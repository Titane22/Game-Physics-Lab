// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PirateCannon.h"
#include "Characters/PirateCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Weapons/CannonBall.h"

// Sets default values
APirateCannon::APirateCannon()
{
	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CannonBaseComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	CannonBarrelComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel"));
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	InteractComponet = CreateDefaultSubobject<UBoxComponent>(TEXT("Interction Box"));
	CannonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Cannon View"));

	RootComponent = DefaultComponent;
	CannonBaseComponent->SetupAttachment(RootComponent);
	CannonBarrelComponent->SetupAttachment(CannonBaseComponent);
	MuzzleLocation->SetupAttachment(CannonBarrelComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseAsset(TEXT("/Game/Cannon/SM_CannonBase.SM_CannonBase"));
	if (BaseAsset.Succeeded())
	{
		CannonBaseComponent->SetStaticMesh(BaseAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BarrelAsset(TEXT("/Game/Cannon/SM_CannonBarrel.SM_CannonBarrel"));
	if (BarrelAsset.Succeeded())
	{
		CannonBarrelComponent->SetStaticMesh(BarrelAsset.Object);
	}

	InteractComponet->SetupAttachment(RootComponent);
	InteractComponet->SetBoxExtent(FVector(100.f, 100.f, 100.f));

	CannonCamera->SetupAttachment(CannonBarrelComponent);
	CannonCamera->SetRelativeLocation(FVector(-50.0f, 0.0f, 75.0f)); 
	CannonCamera->bUsePawnControlRotation = false;
	CannonCamera->SetActive(false); 
}

void APirateCannon::BeginPlay()
{
	Super::BeginPlay();
	if (!CannonBallClass)
	{
		CannonBallClass = LoadClass<ACannonBall>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/Weapons/BP_CannonBall.BP_CannonBall_C"));
	}
}

void APirateCannon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APirateCannon::Interact);

		EIC->BindAction(RotateYawAction, ETriggerEvent::Triggered, this, &APirateCannon::RotateYaw);
		EIC->BindAction(RotatePitchAction, ETriggerEvent::Triggered, this, &APirateCannon::RotatePitch);

		EIC->BindAction(FireAction, ETriggerEvent::Triggered, this, &APirateCannon::Fire);
		EIC->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &APirateCannon::Reload);
	}
}

void APirateCannon::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(CanonMappingContext);
				Subsystem->AddMappingContext(CanonMappingContext, 0);

				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Cannon mapping context added"));
			}
		}
	}
}

void APirateCannon::UnPossessed()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(CanonMappingContext);

				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Cannon mapping context removed"));
			}
		}
	}

	Super::UnPossessed();
}

void APirateCannon::Interact()
{
	DeactivateCannonView();
}

void APirateCannon::RotateYaw(const FInputActionValue& Value)
{
	float YawInput = Value.Get<float>();
	FRotator currentBaseRotation = CannonBaseComponent->GetRelativeRotation();
	
	// 부드러운 움직임을 위한 SmoothDamp 구현
	float targetYaw = currentBaseRotation.Yaw + YawInput * RotationSpeed * GetWorld()->GetDeltaSeconds();
	targetYaw = FMath::Clamp(targetYaw, -MaxYawAngle, MaxYawAngle);
	
	float newYaw = FMath::FInterpTo(currentBaseRotation.Yaw, targetYaw, GetWorld()->GetDeltaSeconds(), 1.0f/SmoothTime);
	
	CannonBaseComponent->SetRelativeRotation(FRotator(0.0f, newYaw, 0.0f));
}

void APirateCannon::RotatePitch(const FInputActionValue& Value)
{
	float PitchInput = Value.Get<float>();
	FRotator currentBarrelRotation = CannonBarrelComponent->GetRelativeRotation();
	
	// 부드러운 움직임을 위한 SmoothDamp 구현
	float targetPitch = currentBarrelRotation.Pitch + PitchInput * RotationSpeed * GetWorld()->GetDeltaSeconds();
	targetPitch = FMath::Clamp(targetPitch, 0.0f, MaxPitchAngle);
	
	float newPitch = FMath::FInterpTo(currentBarrelRotation.Pitch, targetPitch, GetWorld()->GetDeltaSeconds(), 1.0f/SmoothTime);
	
	CannonBarrelComponent->SetRelativeRotation(FRotator(newPitch, 0.0f, 0.0f));
}

void APirateCannon::ActivateCannonView(APirateCharacter* Cahracter)
{
	if (APlayerController* PC = Cast<APlayerController>(Cahracter->GetController()))
	{
		bIsPlayerControlling = true;
		CharacterRef = Cahracter;

		//CharacterRef->DisableInput(PC);

		CannonCamera->SetActive(true);
		PC->SetViewTargetWithBlend(this, 0.5f);
		
		FTimerHandle timeHandle;
		GetWorldTimerManager().SetTimer(
			timeHandle,
			[this, PC]()
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Called"));
				PC->Possess(this);
			},
			0.5f,
			false
		);
	}
}

void APirateCannon::DeactivateCannonView()
{
	if (!bIsPlayerControlling || !CharacterRef)
		return;

	if (APlayerController* PC = GetController<APlayerController>())
	{
		bIsPlayerControlling = false;
		
		PC->UnPossess();

		FTimerHandle timeHandle;
		GetWorldTimerManager().SetTimer(
			timeHandle,
			[this, PC]()
			{
			},
			0.5f,
			false
		);
		PC->Possess(CharacterRef);
		CharacterRef->EnableInput(PC);

		CannonCamera->SetActive(false);
		PC->SetViewTargetWithBlend(CharacterRef, 0.5f);
		CharacterRef = nullptr;
	}
}

void APirateCannon::OnCannonBallHit()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(
			timerHandle,
			[this, PC]()
			{
				if (PC)
				{
					CannonCamera->SetActive(true);
					PC->SetViewTargetWithBlend(this, 0.5f);
					CurrentCannonBallRef = nullptr;
				}
			},
			1.0f,
			false
		);
	}
}

void APirateCannon::Fire()
{

}

void APirateCannon::Reload()
{
}
