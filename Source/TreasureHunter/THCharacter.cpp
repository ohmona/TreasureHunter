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

	// Initializing transform each tick
	current_transform = GetTransform();

	/** Check facing actor and if it's not selecting prop, reset */
	if (IsSelecting()) {

		// Get facing actor
		AActor* facing = DoLinetrace(max_range).GetActor();

		// Check facing actor isn't selecting actor
		if (facing != Cast<AActor>(Prop)) {
			// Disable glow of selecting prop
			Prop->ChangeGlow();

			// Clear selecting prop
			ClearProp();
			bSelecting = false;
		}
	}

	/** Make holding prop to follow the player's view */
	if (IsHolding()) {
		if (!HoldProp(DeltaTime)) {
			// If something went wrong, break the hold
			BP_OnHoldBreak();
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
FHitResult ATHCharacter::DoLinetrace(float range, AActor* ignoring)
{
	FVector rayLocation;
	FRotator rayRotation;
	FVector endTrace = FVector::ZeroVector;

	APlayerController* const playerController = GetWorld()->GetFirstPlayerController();

	if (playerController)
	{
		playerController->GetPlayerViewPoint(rayLocation, rayRotation);
		endTrace = rayLocation + (rayRotation.Vector() * range);
	}

	FCollisionQueryParams traceParams(SCENE_QUERY_STAT(GetActorInAim), true, GetInstigator());
	FHitResult hit(ForceInit);

	if (ignoring != nullptr) {
		traceParams.AddIgnoredActor(ignoring);
	}

	GetWorld()->LineTraceSingleByChannel(hit, rayLocation, endTrace, ECC_WorldDynamic, traceParams);

	return hit;
}

FHitResult ATHCharacter::DoLinetrace(float range)
{
	return DoLinetrace(range, nullptr);
}

// Check whether player is selecting something
bool ATHCharacter::IsSelecting()
{
	return Prop != nullptr && !bHolding;
}

// Check whether player is holding something
bool ATHCharacter::IsHolding()
{
	return Prop != nullptr && bHolding;
}

// Clear selecting prop
void ATHCharacter::ClearProp()
{
	Prop = nullptr;
}

// Hold prop
bool ATHCharacter::HoldProp(float DeltaTime)
{
	// prevent from bug abuse
	if (GetControlRotation().Pitch < 310 && GetControlRotation().Pitch > 270) {
		HoldRelease();
		return false;
	}

	// Get prop's transform
	FTransform prop_transform = Prop->GetTransform();

	const float error_range = 80;

	// Get actor at the behind of holding prop
	FHitResult hit = DoLinetrace(select_range + error_range, Prop);
	FHitResult hit_behind = DoLinetrace(select_range + error_range + 150, Prop);

	FVector Destination;
	float distance = hit.Distance;
	float correction = 0;

	// Check if there's blocking
	if (hit.IsValidBlockingHit()) {
		// Destinate to facing location with distance to blocking object
		Destination = current_transform.GetLocation()
			+ (GetController()->GetControlRotation().Vector() * (distance - 50)); // 나는 빡대가리
		Destination.Z += (error_range + 10);
	}
	else { 
		// Set correction if prop is too close to other object
		if (hit_behind.IsValidBlockingHit()) {
			correction = 30;
		}

		// Default location where prop should be normally
		Destination = current_transform.GetLocation()
			+ (GetController()->GetControlRotation().Vector() * default_distance);
		Destination.Z += correction;
	}

	// Get player's front vector
	FTransform player_transform;
	player_transform.SetLocation(Destination);
	player_transform.SetRotation(GetControlRotation().Quaternion());

	// Do lerp
	FVector dest_loc;
	FQuat dest_rot;
	dest_loc = FMath::Lerp(prop_transform.GetLocation(), player_transform.GetLocation(), DeltaTime * move_time);
	dest_rot = FMath::Lerp(prop_transform.GetRotation(), player_transform.GetRotation(), DeltaTime * move_time);

	// Apply transform
	FTransform destination;
	destination.SetLocation(dest_loc);
	destination.SetRotation(dest_rot);

	FHitResult Hit;
	Prop->StaticMeshComponent->SetWorldTransform(destination, true, &Hit);
	Prop->StaticMeshComponent->SetAllPhysicsPosition(dest_loc);
	Prop->StaticMeshComponent->SetAllPhysicsLinearVelocity(dest_loc);
	Prop->StaticMeshComponent->SetAllPhysicsAngularVelocityInRadians(dest_rot.Vector());

	return true;
}

void ATHCharacter::MoveForward(float AxisValue) { AddMovementInput(GetActorForwardVector(), AxisValue); }

void ATHCharacter::MoveRight(float AxisValue) { AddMovementInput(GetActorRightVector(), AxisValue); }

void ATHCharacter::Turn(float AxisValue) { AddControllerYawInput(AxisValue); }

void ATHCharacter::Lookup(float AxisValue) { AddControllerPitchInput(AxisValue); }

void ATHCharacter::JumpPress() { bPressedJump = true; }

void ATHCharacter::JumpRelease() { bPressedJump = false; }

void ATHCharacter::CrouchPress() { Crouch(true); }

void ATHCharacter::CrouchRelease() { UnCrouch(true); }

void ATHCharacter::SprintPress() { Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = walk_speed * sprint_factor; }

void ATHCharacter::SprintRelease() { Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = walk_speed; }

void ATHCharacter::SelectPress()
{
	// Initialy get the actor front of the player
	FHitResult FrontActor = DoLinetrace(select_range);

	// Cast front actor to prop object
	ATHProp* prop_temp = Cast<ATHProp>(FrontActor.GetActor());
	if (prop_temp != nullptr) {
		// Run code only if the prop is selectable, isn't glowing and the player isn't holding
		if (prop_temp->bSelectable && !prop_temp->bGlowing && !bHolding) {
			Prop = prop_temp;
			Prop->ChangeGlow();
			Prop->StartCountSelectedTime();
			bSelecting = true;
		}
	}
}

void ATHCharacter::SelectRelease()
{
	/** Finish Selecting */
	if (IsSelecting()) {
		// Disable glow of selecting prop
		Prop->ChangeGlow();

		ClearProp();
		bSelecting = false;
	}
}

void ATHCharacter::HoldPress()
{
	// Initialy get the actor front of the player
	FHitResult FrontActor = DoLinetrace(select_range);

	// Cast front actor to prop object
	ATHProp* prop_temp = Cast<ATHProp>(FrontActor.GetActor());
	if (prop_temp != nullptr) {
		// run code only prop is holdable
		if (prop_temp->bHoldable) {
			Prop = prop_temp;
			Prop->StaticMeshComponent->SetEnableGravity(false);
			bHolding = true;
		}
	}
}

void ATHCharacter::HoldRelease()
{
	/** Finish Holding */
	if (IsHolding()) {
		Prop->StaticMeshComponent->SetEnableGravity(true);

		Prop->StaticMeshComponent->SetSimulatePhysics(false);
		Prop->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Prop->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Prop->StaticMeshComponent->SetSimulatePhysics(true);

		ClearProp();
		bHolding = false;
	}
}