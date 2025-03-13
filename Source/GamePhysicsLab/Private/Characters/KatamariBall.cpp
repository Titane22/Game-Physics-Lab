// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/KatamariBall.h"
#include "AttachComponent.h"

// Sets default values
AKatamariBall::AKatamariBall()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultComponent;

	KatamariBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katamari Base Component"));
	KatamariBase->SetupAttachment(RootComponent);
	KatamariBase->SetRelativeScale3D(KatamariScale);
	KatamariBase->SetSimulatePhysics(true);
	KatamariBase->SetLinearDamping(0.5f);
	KatamariBase->SetAngularDamping(0.5f);
	KatamariBase->SetMassOverrideInKg(NAME_None, 100.0f);
	KatamariBase->GetBodyInstance()->bLockXRotation = false;
	KatamariBase->GetBodyInstance()->bLockYRotation = false;
	KatamariBase->GetBodyInstance()->bLockZRotation = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(KatamariBase);
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->SetRelativeRotation(FRotator(150.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(CameraBoom);
	CameraComponent->bUsePawnControlRotation = false;

	AttachComponent = CreateDefaultSubobject<UAttachComponent>(TEXT("Attach Component"));

	// 자동 포제스 설정
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	// 폰이 컨트롤러의 회전을 사용하도록 설정
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AKatamariBall::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKatamariBall::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(BallMappingContext);
				Subsystem->AddMappingContext(BallMappingContext, 0);
			}
		}
	}
}

void AKatamariBall::UnPossessed()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(BallMappingContext);
			}
		}
	}

	Super::UnPossessed();
}

// Called every frame
void AKatamariBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKatamariBall::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(ForwardAction, ETriggerEvent::Triggered, this, &AKatamariBall::MoveForward);
		EIC->BindAction(RightAction, ETriggerEvent::Triggered, this, &AKatamariBall::MoveRight);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKatamariBall::Look);
	}
}

void AKatamariBall::MoveForward(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();

	if (MovementValue != 0.0f)
	{
		const FRotator YawRotation(0, CameraComponent->GetComponentRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		KatamariBase->AddForce(Direction * MovementValue * RollingForce);
	}
}

void AKatamariBall::MoveRight(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();

	if (MovementValue != 0.0f)
	{
		const FRotator YawRotation(0, CameraComponent->GetComponentRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		KatamariBase->AddForce(Direction * MovementValue * RollingForce);
	}
}

void AKatamariBall::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		
		// 컨트롤러 입력 추가
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
		
		// 카메라 붐의 회전 직접 업데이트
		if (CameraBoom)
		{
			// 현재 회전 가져오기
			FRotator CurrentRotation = CameraBoom->GetRelativeRotation();
			
			// 마우스 입력에 따라 Yaw 회전 업데이트 (좌우 회전)
			CurrentRotation.Yaw += LookAxisVector.X * 2.0f; // 감도 조절 가능
			
			// 필요하다면 Pitch도 업데이트 (상하 회전)
			// CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch - LookAxisVector.Y * 2.0f, -80.0f, 80.0f);
			
			// 회전 적용
			CameraBoom->SetRelativeRotation(CurrentRotation);
		}
	}
}

