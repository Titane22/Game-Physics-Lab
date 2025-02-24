// Fill out your copyright notice in the Description page of Project Settings.


#include "GOW_Character.h"
#include "GamePhysicsLabClasses.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Curves/CurveFloat.h"
#include "Weapons/Projectile_Leviathan.h"
#include "Kismet/GameplayStatics.h"

AGOW_Character::AGOW_Character()
{
	// Capsule Component Settings
	// Set the default collision capsule size (radius, height)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Controller Rotation Settings
	// Configure controller rotation to not directly affect the character
	bUseControllerRotationPitch = false;  // Disable pitch rotation
	bUseControllerRotationYaw = false;     // Disable yaw rotation
	bUseControllerRotationRoll = false;   // Disable roll rotation

	// Character Movement Settings
	// Configure character movement and rotation properties
	GetCharacterMovement()->bOrientRotationToMovement = false;     // Rotate character towards movement direction
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // Don't use controller rotation
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 540.0f);  // Rotation speed
	GetCharacterMovement()->JumpZVelocity = 400.f;               // Jump strength
	GetCharacterMovement()->AirControl = 0.35f;                  // Air control
	GetCharacterMovement()->MaxWalkSpeed = 500.f;                // Maximum movement speed
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;           // Minimum movement speed
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; // Deceleration when stopping
	GetCharacterMovement()->MaxAcceleration = 1000.0f;           // Maximum acceleration
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true; // Allow ledge movement while crouching

	// Navigation Settings
	// Configure agent properties for AI navigation system
	GetCharacterMovement()->NavAgentProps.AgentRadius = 42.0f;            // Agent radius
	GetCharacterMovement()->NavAgentProps.AgentHeight = 192.0f;          // Agent height
	GetCharacterMovement()->NavAgentProps.AgentStepHeight = -1.0f;       // Agent step height
	GetCharacterMovement()->NavAgentProps.NavWalkingSearchHeightScale = 0.5f;  // Path search height scale

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom->TargetArmLength = 300.0f;                        // Camera distance
	CameraBoom->bUsePawnControlRotation = true;                  // Controller-based rotation
	CameraBoom->SocketOffset = FVector(10.0f, 95.0f, 20.0f);      // Socket offset
	CameraBoom->bInheritRoll = false;                           // Disable roll rotation inheritance
	CameraBoom->bEnableCameraLag = true;                        // Enable camera lag effect
	CameraBoom->bEnableCameraRotationLag = true;                // Enable camera rotation lag
	CameraBoom->CameraLagSpeed = 30.0f;                         // Camera lag speed
	CameraBoom->CameraRotationLagSpeed = 30.0f;                 // Camera rotation lag speed
	CameraBoom->CameraLagMaxDistance = 5.0f;                    // Maximum lag distance
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f)); // Relative location
	CameraBoom->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f)); // Relative rotation
	CameraBoom->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));   // Relative scale

	// Create a follow camera
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	LeviathanAxe = CreateDefaultSubobject<UChildActorComponent>(TEXT("Leviathan Axe"));
	LeviathanAxe->SetupAttachment(GetMesh(), FName("RightHandWeaponBoneSocket"));

	AxeCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Axe Capsule"));
	AxeCapsule->SetupAttachment(LeviathanAxe);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// 무기의 물리 시뮬레이션 비활성화
	if (LeviathanAxe)
	{
		// 자식 액터 클래스 설정
		LeviathanAxe->SetChildActorClass(AProjectile_Leviathan::StaticClass());
	}
}

void AGOW_Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 카메라의 회전값을 가져옴
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 이동 벡터에 따른 방향 계산
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 이동 방향에 따라 캐릭터 이동
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		// 이동 방향으로 캐릭터 회전
		if (!MovementVector.IsNearlyZero())
		{
			// 이동 방향을 기반으로 회전값 계산
			FVector MovementDirection = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal();
			FRotator NewRotation = MovementDirection.Rotation();
			SetActorRotation(NewRotation);
		}
	}
}

void AGOW_Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		float worldDeltaSeconds = GetWorld()->GetDeltaSeconds();
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * worldDeltaSeconds * CameraTurnRate);
		AddControllerPitchInput(LookAxisVector.Y * worldDeltaSeconds * CameraTurnRate);
	}
}

void AGOW_Character::Aim(const FInputActionValue& Value)
{
	IsAim = Value.Get<bool>();

	if (IsAim)
	{
		
	}
	else
	{
		
	}
}

void AGOW_Character::ThrowAxe()
{
	if (!IsAim || !LeviathanAxeRef)
		return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UAnimMontage* ThrowMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), nullptr, TEXT("/Game/Character/Animation/Axe/M_ThrowAxe")));

		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AGOW_Character::OnNotifyBegin);
		AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &AGOW_Character::OnNotifyEnd);

		AnimInstance->Montage_Play(ThrowMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.037f);
	}
}

void AGOW_Character::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	// TODO: SpawnSoundAttached
}

void AGOW_Character::OnNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	LeviathanAxeRef->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	FRotator cameraRotation = FollowCamera->GetComponentRotation();
	FVector throwDirectionVector = FollowCamera->GetForwardVector();
	FVector cameraLocation = FollowCamera->GetComponentLocation();

	LeviathanAxeRef->Throw(cameraRotation, throwDirectionVector, cameraLocation);

	AxeThrown = true;
}

void AGOW_Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (AimCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateAimView"));
		AimTimeline.AddInterpFloat(AimCurve, TimelineCallback);
		AimTimeline.SetPlayRate(4.0f);
	}

	if (AProjectile_Leviathan* PJ_Axe = Cast<AProjectile_Leviathan>(LeviathanAxe->GetChildActor()))
	{
		LeviathanAxeRef = PJ_Axe;
	}

	GetMesh()->HideBoneByName(FName("hips_cloth_main_l"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(FName("hips_cloth_main_r"), EPhysBodyOp::PBO_None);

	// 실행 시 물리 시뮬레이션 비활성화
	if (AActor* WeaponActor = LeviathanAxe->GetChildActor())
	{
		if (UPrimitiveComponent* WeaponMesh = Cast<UPrimitiveComponent>(WeaponActor->GetRootComponent()))
		{
			WeaponMesh->SetSimulatePhysics(false);
			WeaponMesh->SetEnableGravity(false);
		}
	}
}

void AGOW_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//AimTimeline.TickTimeline(DeltaTime);

	// Manage Character Rotation
	if (GetVelocity().Length() > 1.0f || bUserControllerRotation)
	{
		FRotator curActorRotation = GetActorRotation();
		FRotator tarActorRotation = FRotator(curActorRotation.Pitch, GetControlRotation().Yaw, curActorRotation.Roll);

		SetActorRotation(FMath::RInterpTo(curActorRotation, tarActorRotation, DeltaTime, 50.0f));
	}
}

void AGOW_Character::LerpCameraPosition(float Value)
{
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = FMath::Lerp(SpringArmLengthIdle, SpringArmLengthAim, Value);

		CameraBoom->SocketOffset = FMath::Lerp(CameraVector, RangedCameraVector, Value);

		DesiredSocketOffset = FMath::Lerp(CameraVector, RangedCameraVector, Value);
	}
}
