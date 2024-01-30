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

	//캡슐컴포넌트
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	//프로젝트 콜리젼 설정 프리셋 이름 정하는 코드
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCapsule"));

	//스켈레톤 매쉬
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	//캐릭터 무브먼트 컴포넌트
	//이동하는 방향으로 바라보는 방향 변경
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

	//데이터 에셋, CharacterControl DataAsset
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
	/*UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();		//캡슐 스켈레탈메쉬 무브먼트 에로우 네가지컴포넌트중에
	AnimInstance->Montage_Play(ComboActionMontage);*/

	if (CurrentComboCount == 0) {
		ComboActionBegin();
		return;
	}

	if(ComboTimerHandle.IsValid() && !ComboTimerHandle2.IsValid())
	{
		HasNextComboCommand = true;
	}
	else
	{
		HasNextComboCommand = false;
	}
}

void AAABCharacterBase::ComboActionBegin()
{
	CurrentComboCount = 1;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage);

	FOnMontageEnded EndDelegate;		// 델리게이트
	EndDelegate.BindUObject(this, &AAABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	SetComboCheckTimer();
}

void AAABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool bIsInterruped)
{
	CurrentComboCount = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AAABCharacterBase::SetComboCheckTimer()
{
	float ComboEffectiveTime2 = ComboAttackData->EffectiveFrameCount[CurrentComboCount - 1 + 4] / ComboAttackData->FrameRate;	// (5 / 30)???????????
	float ComboEffectiveTime = ComboAttackData->EffectiveFrameCount[CurrentComboCount - 1] / ComboAttackData->FrameRate;	// (20 / 30)

	if (ComboEffectiveTime > 0.0f && ComboEffectiveTime2 > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle2, this, &AAABCharacterBase::ComboCheck2, ComboEffectiveTime2, false);
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AAABCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void AAABCharacterBase::ComboCheck()
{
	if (!ComboTimerHandle2.IsValid())
	{
		ComboTimerHandle.Invalidate();
	}
	
	if (HasNextComboCommand)
	{
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, ComboAttackData->MaxComboCount);
		
		FName NextSectionName = *FString::Printf(TEXT("%s%d"), *ComboAttackData->MontageSectionPrefix, CurrentComboCount);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_JumpToSection(NextSectionName, ComboActionMontage);

		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

void AAABCharacterBase::ComboCheck2()
{
	ComboTimerHandle2.Invalidate();
}


