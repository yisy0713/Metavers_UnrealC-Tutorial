// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AABCharacterBase.generated.h"

UENUM()//�̰� �ٿ��� �𸮾� �ý��� ���� ��
enum class ECharacterControlType : uint8 //enum�� ����
{
	Shoulder,
	Quater
};

UCLASS()//�̰� �־�� �𸮾� �ý��۳��� ��
class ARENABATTLE_API AAABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAABCharacterBase();

protected:
	virtual void SetCharacterControlData(const class UABCharacterControllDataAsset* CharacterControlData);

protected:
	//�ʿ��� Ű������ �̳����� ���� �����ͼ����� ����ϰڴ�, �������� ����ó��
	TMap< ECharacterControlType, class UABCharacterControllDataAsset*> CharacterControlManager; // �����ڰ� ȣ��ɋ� ���� �޸� �Ҵ�

};
