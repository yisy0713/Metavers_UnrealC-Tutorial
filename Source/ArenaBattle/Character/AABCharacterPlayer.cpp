// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AABCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/ABCharacterControllDataAsset.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"

AAABCharacterPlayer::AAABCharacterPlayer()
{
	//Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->bUsePawnControlRotation = true;
	//Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	//Input

	//static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Input/IMC_Default.IMC_Default'"));
	//if (InputMappingContextRef.Object)
	//{
	//	DefaultMappingContext = InputMappingContextRef.Object;
	//}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMoveRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove"));
	if (InputActionQuaterMoveRef.Object)
	{
		QuaterMoveAction = InputActionQuaterMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove"));
	if (InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook"));
	if (InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump"));
	if (InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeControlRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl"));
	if (InputActionChangeControlRef.Object)
	{
		ChangeControlAction = InputActionChangeControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack"));
	if (InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Quater;
}

void AAABCharacterPlayer::AttackHitCheck()
{
	// ���� �浹 ������ �Ѵ�.
	UE_LOG(LogTemp, Log, TEXT("AttackHitCheck_P"));

	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(Attack), false, this);
	FHitResult OutHitResult;
	const float AttackRange = 150.0f;
	const float CapsuleRadius = 50.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	TArray<FOverlapResult> OutOverlapResultArray;
	//const FVector OverlapPosition = Start + (End - Start) / 2.0f;
	const FQuat OverlapRotation = GetActorQuat();
	bool IsHit = false;
	bool IsFullComboHit = false;
	if (CurrentComboCount < 4)
	{
		IsHit = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(CapsuleRadius), CollisionParams);
	}
	else
	{
		IsFullComboHit = GetWorld()->OverlapMultiByChannel(OutOverlapResultArray, Start, OverlapRotation, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(100.0f));
	}

	if (IsHit)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(100.0f, DamageEvent, GetController(), this);
	}
	else if (IsFullComboHit)
	{
		FDamageEvent DamageEvent;
		for (int32 i = 0; i < OutOverlapResultArray.Num(); i++)
		{
			OutOverlapResultArray[i].GetActor()->TakeDamage(100.0f, DamageEvent, GetController(), this);
		}
	}

#if ENABLE_DRAW_DEBUG
	if (CurrentComboCount < 4)
	{
		FVector CapsulePosition = Start + (End - Start) / 2.0f;
		float HalfHeight = AttackRange / 2.0f;
		DrawDebugCapsule(GetWorld(), CapsulePosition, HalfHeight, CapsuleRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Red, false, 3.0f);
	}
	else
	{
		DrawDebugSphere(GetWorld(), Start, 100.0f, 24, FColor::Red, false, 3.0f);
	}
#endif // ENABLE_DRAW_DEBUG

}

void AAABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	//APlayerController* PlayerController =CastChecked<APlayerController>(GetController());
	//UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	//if (nullptr != Subsystem)
	//{
	//	Subsystem->AddMappingContext(DefaultMappingContext, 0);
	//	//Subsystem->RemoveMappingContext(DefaultMappingContext);
	//}

	SetCharacterControl(ECharacterControlType::Quater);
}

void AAABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//���� �Է� �ý��� ���
	//�Է¸������ؽ�Ʈ���� �׼ǰ� �Լ�
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked <UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AAABCharacterPlayer::QuaterMove);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AAABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AAABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AAABCharacterPlayer::ChangeControl);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AAABCharacterPlayer::Attack);

}

void AAABCharacterPlayer::SetCharacterControlData(const UABCharacterControllDataAsset* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);//����Ŭ���� ���� ���

	//Input
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (nullptr != Subsystem)
	{
		Subsystem->ClearAllMappings();
		if (CharacterControlData->InputMappingContext)
		{
			Subsystem->AddMappingContext(CharacterControlData->InputMappingContext, 0);
		}
		//Subsystem->RemoveMappingContext(DefaultMappingContext);
	}

	//Camera
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
}

void AAABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float MovementVectorsizeSquared = MovementVector.SquaredLength();
	if (MovementVectorsizeSquared > 1.0f)
	{
		//ũ�⸦ 1�� ����
		MovementVector.Normalize();
		MovementVectorsizeSquared = 1.0f;
	}
	else
	{

	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorsizeSquared);
}

void AAABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);

}

void AAABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AAABCharacterPlayer::ChangeControl()
{
	//���� ĳ���� ��Ʈ�� Ÿ���� �����̸� ����� ����� ���ͷ� ��ȯ
	if (CurrentCharacterControlType == ECharacterControlType::Quater)
	{
		CurrentCharacterControlType = ECharacterControlType::Shoulder;
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		CurrentCharacterControlType = ECharacterControlType::Quater;
	}

	SetCharacterControl(CurrentCharacterControlType);

}

void AAABCharacterPlayer::SetCharacterControl(ECharacterControlType ControlType)
{
	UABCharacterControllDataAsset* NewCharacterControlData = CharacterControlManager[ControlType];
	SetCharacterControlData(NewCharacterControlData);

}

void AAABCharacterPlayer::Attack()
{
	ProcessComboCommand();
}
