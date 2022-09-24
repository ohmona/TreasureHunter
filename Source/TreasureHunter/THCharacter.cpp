// Fill out your copyright notice in the Description page of Project Settings.


#include "THCharacter.h"
#include <Engine/Classes/GameFramework/CharacterMovementComponent.h>

// Sets default values
ATHCharacter::ATHCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Cast<UCharacterMovementComponent>(GetMovementComponent())->AirControl = 1;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->GravityScale = 1.2;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->BrakingFrictionFactor = 1;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->bUseSeparateBrakingFriction = true;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->NavAgentProps.bCanCrouch = true;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = walk_speed;
}

// Called when the game starts or when spawned
void ATHCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &ATHCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATHCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &ATHCharacter::Turn);
	InputComponent->BindAxis("Lookup", this, &ATHCharacter::Lookup);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ATHCharacter::JumpPress);
	InputComponent->BindAction("Jump", IE_Released, this, &ATHCharacter::JumpRelease);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ATHCharacter::CrouchPress);
	InputComponent->BindAction("Crouch", IE_Released, this, &ATHCharacter::CrouchRelease);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ATHCharacter::SprintPress);
	InputComponent->BindAction("Sprint", IE_Released, this, &ATHCharacter::SprintRelease);
}

void ATHCharacter::MoveForward(float AxisValue){ AddMovementInput(GetActorForwardVector(), AxisValue); }

void ATHCharacter::MoveRight(float AxisValue) { AddMovementInput(GetActorRightVector(), AxisValue); }

void ATHCharacter::Turn(float AxisValue){ AddControllerYawInput(AxisValue); }

void ATHCharacter::Lookup(float AxisValue) { AddControllerPitchInput(AxisValue); }

void ATHCharacter::JumpPress() { bPressedJump = true; }

void ATHCharacter::JumpRelease() { bPressedJump = false; }

void ATHCharacter::CrouchPress() { Crouch(true); }

void ATHCharacter::CrouchRelease() { UnCrouch(true); }

void ATHCharacter::SprintPress() { Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = walk_speed * sprint_factor; }

void ATHCharacter::SprintRelease() { Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = walk_speed; }
