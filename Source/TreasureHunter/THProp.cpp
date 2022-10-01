// Fill out your copyright notice in the Description page of Project Settings.


#include "THProp.h"

// Sets default values
ATHProp::ATHProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Preset = GetTransform();

	// Create and initialize Components
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	RootComponent = StaticMeshComponent;
	
}

// Called when the game starts or when spawned
void ATHProp::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->SetSimulatePhysics(bMoveable);
}

// Called every frame
void ATHProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Reset transform of prop (Reset completely)
void ATHProp::ResetTransform()
{
	SetActorTransform(Preset);
}

// Reset rotation of prop
void ATHProp::ResetRotation(bool b_only_wake_up)
{
	FQuat ResetRotation;
	FQuat PresetVal = Preset.GetRotation();
	
	// Initalize return rotation as preset rotation
	ResetRotation = PresetVal;

	if (b_only_wake_up) {
		// Set X and Y values to zero or negative zero
		FQuat CurrentRotation = GetTransform().GetRotation();
		CurrentRotation.X > 0 ? CurrentRotation.X = 0.0001 : CurrentRotation.X = -0.0001;
		CurrentRotation.Y > 0 ? CurrentRotation.Y = 0.0001 : CurrentRotation.Y = -0.0001;

		ResetRotation = CurrentRotation;
	}

	// unless destinating rotation is equal to current, apply the rotation
	if(!ResetRotation.Equals(GetTransform().GetRotation())) SetActorRotation(ResetRotation);
}