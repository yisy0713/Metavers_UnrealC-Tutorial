// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/ABCharacterControllDataAsset.h"
#include "Character/ABComboAttackDataAsset.h"

// Sets default values
AAABCharacterBase::AAABCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ĸ��������Ʈ
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	//������Ʈ �ݸ��� ���� ������ �̸� ���ϴ� �ڵ�
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCapsule"));

	//���̷��� �Ž�
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	//ĳ���� �����Ʈ ������Ʈ
	//�̵��ϴ� �������� �ٶ󺸴� ���� ����
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Skeletal Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	//Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_CharacterAnimation.ABP_CharacterAnimation_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	//������ ����, CharacterControl DataAsset
	static ConstructorHelpers::FObjectFinder<UABCharacterControllDataAsset> ShoulderDataAssetRef(TEXT("/Game/ArenaBattle/CharacterControlData/DA_CCShoulder.DA_CCShoulder"));
	if (ShoulderDataAssetRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataAssetRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControllDataAsset> QuaterDataAssetRef(TEXT("/Game/ArenaBattle/CharacterControlData/DA_CCQuater.DA_CCQuater"));
	if (QuaterDataAssetRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataAssetRef.Object);
	}
}

void AAABCharacterBase::SetCharacterControlData(const UABCharacterControllDataAsset* CharacterControlData)
{
	//pawn
	bUseControllerRotationPitch =  CharacterControlData->bUseControlRotationPitch;
	bUseControllerRotationYaw = CharacterControlData->bUseControlRotationYaw;
	bUseControllerRotationRoll = CharacterControlData->bUseControlRotationRoll;

	//Movement
	GetCharacterMovement()->bOrientRotationToMovement =  CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AAABCharacterBase::ProcessComboCommand()
{
	if (CurrentComboCount == 0)
	{
		//UE_LOG(LogTemp, Log, TEXT("CurrentComboCount == 0"));		// �α� ���
		ComboActionBegin();
		return;
	}

	if(ComboTimerHandle.IsValid() && !ComboStartTimerHandle.IsValid())					// && !ComboTimerHandle2.IsValid())
	{
		//UE_LOG(LogTemp, Log, TEXT("HasNextComboCommand : true"));		// �α� ���
		HasNextComboCommand = true;
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("HasNextComboCommand : false"));
		HasNextComboCommand = false;

		//CurrentComboCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(ComboStartTimerHandle);
		ComboStartTimerHandle.Invalidate();
		GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
		ComboTimerHandle.Invalidate();
	}
}

void AAABCharacterBase::ComboActionBegin()
{
	CurrentComboCount = 1;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage);

	FOnMontageEnded EndDelegate;		// ��������Ʈ
	EndDelegate.BindUObject(this, &AAABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	SetComboStartCheckTimer();
}

void AAABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool bIsInterruped)
{
	CurrentComboCount = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AAABCharacterBase::SetComboStartCheckTimer()
{
	GetWorld()->GetTimerManager().SetTimer(ComboStartTimerHandle, this, &AAABCharacterBase::SetComboCheckTimer, 0.3f, false);
}

void AAABCharacterBase::SetComboCheckTimer()
{
	if (CurrentComboCount <= 0) return;
	float ComboEffectiveTime = ComboAttackData->EffectiveFrameCount[CurrentComboCount - 1] / ComboAttackData->FrameRate;	// (20 / 30)

	if (ComboEffectiveTime > 0.0f)
	{
		ComboStartTimerHandle.Invalidate();
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AAABCharacterBase::ComboCheck, 0.3f, false);
	}
}

void AAABCharacterBase::ComboCheck()
{
	if (HasNextComboCommand)
	{
		//UE_LOG(LogTemp, Log, TEXT("ComboCheck() == %d"), CurrentComboCount);

		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, ComboAttackData->MaxComboCount);
		
		FName NextSectionName = *FString::Printf(TEXT("%s%d"), *ComboAttackData->MontageSectionPrefix, CurrentComboCount);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_JumpToSection(NextSectionName, ComboActionMontage);

		SetComboStartCheckTimer();
		HasNextComboCommand = false;
	}
}

void AAABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetActorEnableCollision(false);

	// Dead Animation
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage);

	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			Destroy();
		}), 3.0f, false);
}


