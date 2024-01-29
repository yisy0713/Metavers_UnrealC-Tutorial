// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABCharacterControllDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterControllDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UABCharacterControllDataAsset();

public:
	//Pawn
	UPROPERTY(EditAnywhere, Category =Pawn)
	uint8 bUseControlRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint8 bUseControlRotationPitch : 1;

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint8 bUseControlRotationRoll : 1;

	//CharacterMovement
	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	FRotator RotationRate;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bUseControllerDesiredRotation : 1;//�ٶ󺸴� ������ �ٲ� �׹ٲ����

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint8 bOrientRotationToMovement : 1;//�����̴� �������� ĳ������ ȸ���� ���� ���� ���� �������ؼ� �������µ�?

	//input
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	//Camera
	UPROPERTY(EditAnywhere, Category = Camera)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, Category = Camera)
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere, Category = Camera)
	uint8 bUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, Category = Camera)
	uint8 bDoCollisionTest : 1;

	UPROPERTY(EditAnywhere, Category = Camera)
	uint8 bInheritYaw : 1;

	UPROPERTY(EditAnywhere, Category = Camera)
	uint8 bInheritPitch : 1;

	UPROPERTY(EditAnywhere, Category = Camera)
	uint8 bInheritRoll : 1;




	

};
