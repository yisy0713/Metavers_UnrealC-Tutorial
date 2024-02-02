// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AABCharacterBase.generated.h"

UENUM()//이걸 붙여야 언리얼 시스템 내에 들어감
enum class ECharacterControlType : uint8 //enum문 생성
{
	Shoulder,
	Quater
};

UCLASS()//이게 있어야 언리얼 시스템내에 들어감
class ARENABATTLE_API AAABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAABCharacterBase();

protected:
	virtual void SetCharacterControlData(const class UABCharacterControllDataAsset* CharacterControlData);
	void ProcessComboCommand();

	void ComboActionBegin();
	void ComboActionEnd(UAnimMontage* TargetMontage, bool bIsInterruped);

	void SetComboCheckTimer();
	void ComboCheck();

	void ComboCheck2();

protected:
	//맵에서 키값으로 이넘으로 값을 데이터셋으로 사용하겠다, 변수명은 다음처럼
	TMap< ECharacterControlType, class UABCharacterControllDataAsset*> CharacterControlManager; // 생성자가 호출될떄 같이 메모리 할당

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackData)
	TObjectPtr<class UABComboAttackDataAsset> ComboAttackData;

	uint8 CurrentComboCount;

	FTimerHandle ComboTimerHandle;
	FTimerHandle ComboTimerHandle2;

	bool HasNextComboCommand = false;
	

};
