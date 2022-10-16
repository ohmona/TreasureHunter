// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <TreasureHunter/THProp.h>
#include "GameFramework/Character.h"
#include "THCharacter.generated.h"

UCLASS()
class TREASUREHUNTER_API ATHCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATHCharacter();

	// Values for control
	float walk_speed = 500;
	float sprint_factor = 1.2;
	float select_range = 2500;

	// Whether player is holding a prop
	bool bHolding = false;

	// Whether player is selecting a prop
	bool bSelecting = false;

	// Selecting prop
	ATHProp* Prop;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Get the actor front of the player
	FHitResult DoLinetrace();

	// Check whether player is selecting something
	bool IsSelecting();

	// Clear selecting prop
	void ClearProp();

public: // Input functions
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void Lookup(float AxisValue);

	void JumpPress();
	void JumpRelease();
	void CrouchPress();
	void CrouchRelease();
	void SprintPress();
	void SprintRelease();
	void SelectPress();
	void SelectRelease();
	void HoldPress();
	void HoldRelease();
};
