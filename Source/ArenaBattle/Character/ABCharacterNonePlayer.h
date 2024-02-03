// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AABCharacterBase.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterNonePlayer.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacterNonePlayer : public AAABCharacterBase, public IABAnimationAttackInterface
{
	GENERATED_BODY()
	
public:
	// 공격 충돌 판정
	virtual void AttackHitCheck(AttackType AttackType) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
};
