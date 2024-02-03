// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonePlayer.h"

void AABCharacterNonePlayer::AttackHitCheck(AttackType AttackType)
{
}

float AABCharacterNonePlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// HP

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Dead
	SetDead();

	return 0.0f;
}