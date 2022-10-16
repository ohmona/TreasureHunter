// Fill out your copyright notice in the Description page of Project Settings.


#include "THCharacter.h"
#include <Engine/Classes/GameFramework/CharacterMovementComponent.h>

// Sets default values
ATHCharacter::ATHCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Cast<UCharacterMovementComponent>(GetMovementComponent())->AirControl = 1;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->GravityScale = 1.1;
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

	/** Check facing actor and if it's not selecting prop, reset */
	if (IsSelecting()) {

		// Get facing actor
		AActor* facing = DoLinetrace().GetActor();

		// Check facing actor isn't selecting actor
		if (facing != Cast<AActor>(Prop)) {
			// Disable glow of selecting prop
			Prop->ChangeGlow();

			// Clear selecting prop
			ClearProp();
			bSelecting = false;
		}
	}
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
	InputComponent->BindAction("Select", IE_Pressed, this, &ATHCharacter::SelectPress);
	InputComponent->BindAction("Select", IE_Released, this, &ATHCharacter::SelectRelease);
	InputComponent->BindAction("Hold", IE_Pressed, this, &ATHCharacter::HoldPress);
	InputComponent->BindAction("Hold", IE_Released, this, &ATHCharacter::HoldRelease);
}

// Get the actor front of the player
FHitResult ATHCharacter::DoLinetrace()
{
	FVector rayLocation;
	FRotator rayRotation;
	FVector endTrace = FVector::ZeroVector;

	APlayerController* const playerController = GetWorld()->GetFirstPlayerController();

	if (playerController)
	{
		playerController->GetPlayerViewPoint(rayLocation, rayRotation);
		endTrace = rayLocation + (rayRotation.Vector() * select_range);
	}

	FCollisionQueryParams traceParams(SCENE_QUERY_STAT(GetActorInAim), true, GetInstigator());
	FHitResult hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(hit, rayLocation, endTrace, ECC_Visibility, traceParams);

	return hit;
}

// Check whether player is selecting something
bool ATHCharacter::IsSelecting()
{
	return Prop != nullptr;
}

// Clear selecting prop
void ATHCharacter::ClearProp()
{
	Prop = nullptr;
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

void ATHCharacter::SelectPress() 
{
	UE_LOG(LogTemp, Log, TEXT("Select press"))

	// Initialy get the actor front of the player
	FHitResult FrontActor = DoLinetrace();

	// Cast front actor to prop object
	ATHProp* prop_temp = Cast<ATHProp>(FrontActor.GetActor());
	if (prop_temp != nullptr) {
		// Run code only if the prop is selectable and isn't glowing
		if (prop_temp->bSelectable && !prop_temp->bGlowing) {
			Prop = prop_temp;
			Prop->ChangeGlow();
			bSelecting = true;
		}
	}
}

void ATHCharacter::SelectRelease() {

	/** Finish Selecting */
	if (IsSelecting()) {
		// Disable glow of selecting prop
		Prop->ChangeGlow();

		ClearProp();
		bSelecting = false;
	}
}

void ATHCharacter::HoldPress() {}

void ATHCharacter::HoldRelease() {}