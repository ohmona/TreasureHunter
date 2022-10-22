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

	SetMassScale(Mass);

	if (StaticMeshComponent->GetStaticMesh() != nullptr) {
		DefaultMaterials.SetNum(StaticMeshComponent->GetMaterials().Num());
		DefaultMaterials = StaticMeshComponent->GetMaterials();
	}
}

// Called every frame
void ATHProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATHProp::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	bOverlapping = true;
}

void ATHProp::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	bOverlapping = true;
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

// Change glowing states
void ATHProp::ChangeGlow()
{
	if (GlowMaterial == nullptr) return;

	if (!bGlowing) { // enable glow
		bGlowing = true;
		for (int32 i = 0; i < DefaultMaterials.Num(); i++) {
			StaticMeshComponent->SetMaterial(i, GlowMaterial);
		}
	}
	else if(bGlowing) { // disable glow
		// Remove glow only if selected time was long enough
		if (time_selected >= minimal_glowing_time) {
			for (int32 i = 0; i < DefaultMaterials.Num(); i++) {
				StaticMeshComponent->SetMaterial(i, DefaultMaterials[i]);
			}
			bGlowing = false;
			StopCountSelectedTime();
		}
		else {
			FTimerHandle GlowRemoveTimer;
			GetWorld()->GetTimerManager().SetTimer(GlowRemoveTimer, this, &ATHProp::ChangeGlow, 1.0f, false, minimal_glowing_time - time_selected);
		}
	}
}

// Change Mass
void ATHProp::SetMassScale(float NewMass)
{
	if (!StaticMeshComponent) return;

	FBodyInstance* BodyInst = StaticMeshComponent->GetBodyInstance();

	if (!BodyInst) return;

	// New Scale
	BodyInst->SetMassScale(NewMass * 5);

	// Update
	BodyInst->UpdateMassProperties();
}

// Add force to specific direction
void ATHProp::AddCustomForce(FVector direction, int32 force)
{
	float MeshMass = StaticMeshComponent->GetMass();
	int32 amplitude_multiplier = 500;

	// Apply force
	StaticMeshComponent->AddForce(direction * force * amplitude_multiplier * MeshMass);
}

// Count selected time
void ATHProp::CountSelectedTime()
{
	time_selected += 0.1f;
}

// Start counting selected time
void ATHProp::StartCountSelectedTime()
{
	GetWorld()->GetTimerManager().SetTimer(SelectTimer, this, &ATHProp::CountSelectedTime, 0.1f, true);
}

// Stop counting selected time and reset
void ATHProp::StopCountSelectedTime()
{
	GetWorld()->GetTimerManager().ClearTimer(SelectTimer);
	time_selected = 0.0f;
}

bool ATHProp::IsStucked()
{
	return bOverlapping;
}