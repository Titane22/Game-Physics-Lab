// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachComponent.h"
#include "Actors/AttachableBase.h"

// Sets default values for this component's properties
UAttachComponent::UAttachComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
}


// Called when the game starts
void UAttachComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMeshComponent* OwnerMesh = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		OwnerMesh->OnComponentHit.AddDynamic(this, &UAttachComponent::OnHit);
		OwnerMesh->SetNotifyRigidBodyCollision(true);
		KatamariMesh = OwnerMesh;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("KatamariMesh: %s"), *KatamariMesh->GetName()));
	}
}


// Called every frame
void UAttachComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttachComponent::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner() || AttachedObjects.Contains(OtherActor))
		return;
	if (AAttachableBase* AttachableActor = Cast<AAttachableBase>(OtherActor))
	{
		if (CanAttachObject(AttachableActor, OtherComp))
		{
			AttachObject(AttachableActor, OtherComp);
		}
	}
}

bool UAttachComponent::CanAttachObject(AAttachableBase* ObjectToAttach, UPrimitiveComponent* ObjectComp)
{
	if (!ObjectToAttach || !ObjectComp || !KatamariMesh)
		return false;

	// TODO: Add Condition For Checking Attach Actor
	return true;
}

void UAttachComponent::AttachObject(AAttachableBase* ObjectToAttach, UPrimitiveComponent* ObjectComp)
{
	if (!ObjectToAttach || !ObjectComp || !KatamariMesh)
		return;

	if (UStaticMeshComponent* MeshComp = ObjectToAttach->GetStaticMeshComponent())
	{
		// 물리 시뮬레이션 중단
		MeshComp->SetSimulatePhysics(false);

		// 콜리전 활성화 (충돌 응답만 활성화)
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 특정 채널만 충돌 무시 설정
		//MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // 플레이어와의 충돌 무시

		// 물리 시뮬레이션 비활성화 후 웰딩 설정
		MeshComp->SetEnableGravity(false);
		MeshComp->WeldTo(KatamariMesh);
	}
	
	// 부착 규칙 변경: KeepWorldTransform -> SnapToTargetNotIncludingScale
	ObjectToAttach->AttachToComponent(KatamariMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	AttachedObjects.Add(ObjectToAttach);
	UpdateKatamariSize();

	OnObjectAttached.Broadcast(ObjectToAttach);
}

void UAttachComponent::UpdateKatamariSize()
{
	if (!KatamariMesh)
		return;

	// TODO: Add Formular Updating for Size
}

