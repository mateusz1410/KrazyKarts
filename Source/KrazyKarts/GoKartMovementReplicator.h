// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKart.h"
#include "GoKartMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GoKartMovementReplicator.generated.h"

//UE4 struct
USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FGoKartMove LastMove;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FTransform Transform;
};
 //CPP struct
struct FHermiteCubicSpline
{
	FVector StartLocation;
	FVector StartDerivative;
	FVector TargetLocation;
	FVector TargetDerivative;

	FVector InterpolateLocation(float LerpRatio) const
	{ 
		return FMath::CubicInterp(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio); 
	}

	FVector InterpolateDerivative(float LerpRatio) const
	{
		return FMath::CubicInterpDerivative(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
	}

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	TArray<FGoKartMove> UnacknowledgeMoves;

	float ClientTimeSinceUpdate;
	float ClientTimeBetweenLastUpdate;

	FTransform ClientStartTransform;
	FVector ClientStartVelocity;

	float ClientSimulatedTime;

	void ClearAcknowledgeMoves(FGoKartMove LastMove);
	void UpdateServerState(const FGoKartMove& Move);

	void ClientTick(float DeltaTime);
	void InterpolateRotation(float LerpRatio);
	FHermiteCubicSpline CreateSpline();

	void InterpolateVelocity(const FHermiteCubicSpline &Spline, float LerpRatio);
	void InterpolateLocation(const FHermiteCubicSpline &Spline, float LerpRatio);
	float GetVelocityToDerivative();

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	void SimulatedProxy_OnRep_ServerState();
	void Autonomous_OnRep_ServerState();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	UPROPERTY()
	class UGoKartMovementComponent* MovementComponent;

	UPROPERTY()
	class USceneComponent*  MeshOffsetRoot;

	UFUNCTION(BlueprintCallable) // MeshOffsetRoot is created in BP, set pointer to it
	void SetMeshOffsetRoot(USceneComponent* Root) { MeshOffsetRoot = Root; }
};
