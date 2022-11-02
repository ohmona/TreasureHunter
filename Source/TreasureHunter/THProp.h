// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "THProp.generated.h"

UCLASS()
class TREASUREHUNTER_API ATHProp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATHProp();

	/** Properties for level-design */

	// Whether it's objective
	UPROPERTY(EditAnywhere)
		bool bSpecial = false;

	// Basically in kg
	UPROPERTY(EditAnywhere)
		float Mass = 1.0f;

	// Whether physics is applied
	UPROPERTY(EditAnywhere)
		bool bMoveable = true;

	// Whether it can be selected by player
	UPROPERTY(EditAnywhere)
		bool bSelectable = true;

	// Whether it can be held by player
	UPROPERTY(EditAnywhere)
		bool bHoldable = true;

	/** unsorted Properties */

	bool bGlowing = false;

	UPROPERTY(EditAnywhere, Category = "Component")
		UStaticMeshComponent* StaticMeshComponent;

	// Transform where prop starts
	FTransform Preset;

	TArray<UMaterialInterface*> DefaultMaterials;

	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInterface* GlowMaterial;

	// Whether it's held by player
	bool bHeld = false;

	/** Select timer */
	FTimerHandle SelectTimer;
	float time_selected = 0.0f;

	// minimal time of glowing
	float minimal_glowing_time = 0.5f;

	// whether prop is overlapping with something
	bool bOverlapping = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Reset transform of prop (Reset completely)
	UFUNCTION(BlueprintCallable)
		void ResetTransform();

	// Reset rotation of prop
	UFUNCTION(BlueprintCallable)
		void ResetRotation(bool b_only_wake_up);

	// Change Mass
	UFUNCTION(BlueprintCallable)
		void SetMassScale(float NewMass);

	// Add force to specific direction
	UFUNCTION(BlueprintCallable)
		void AddCustomForce(FVector direction, int32 force = 100);

	// Change glowing state
	UFUNCTION(BlueprintCallable)
		void ChangeGlow();

	// Count selected time
	void CountSelectedTime();

	// Start counting selected time
	void StartCountSelectedTime();

	// Stop counting selected time and reset
	void StopCountSelectedTime();

	// Check whether prop is stucked
	bool IsStucked();
};