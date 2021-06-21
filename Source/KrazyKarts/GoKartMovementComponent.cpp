// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementComponent.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Mass = 1000; //kg
	MaxDrivingForce = 10000;//Newton
	//MaxDegreesPerSecond = 90; // max turn value per sec
	MinTurningRadius = 10.f;
	DragCoefficient = 16;
	RollingResistanceCoefficient = 0.015;
	// ...
}


// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}

	// ...
}

void UGoKartMovementComponent::SimulateMove(const FGoKartMove& Move)
{

	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle; //Throttle -1 to 1 
	if (Mass == 0)
	{
		return;
	}

	Force += GetAirResistance(); //air resistance
	Force += GetRollingResistance();

	FVector Acceleration = Force / Mass; //force put on car to move / car mass

	Velocity = Velocity + Acceleration * Move.DeltaTime; // increase velocity in time 

	ApplyRotaion(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocationFromVelocity(Move.DeltaTime); //call on server and client

}

FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.Time = GetWorld()->TimeSeconds; // AGameStateBase::GetServerWorldTimeSeconds();

	return Move;
}

FVector UGoKartMovementComponent::GetAirResistance()
{
	//Speed = Velocity.Size(); //speed  Value in that direction
	 //in ue ther is function ... .SizeSquared() == FMath::Square(... .Size())
	//FVector AirResistance = -(FMath::Pow(Speed, 2)) *GetActorForwardVector()*DragCoefficient;

	// AirResistance = - Speed^2 * DragCoefficient
	return -Velocity.GetSafeNormal()* Velocity.SizeSquared() * DragCoefficient; //GetSafeNormal() get direction normalize vector
}

FVector UGoKartMovementComponent::GetRollingResistance()
{
	//UE_LOG(LogTemp, Warning, TEXT("Gravity Force: %f "), GetWorld()->GetGravityZ()); // get -980, G= 9.8 m/s , 980 cm/s , it going down so -980

	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100; // G, 9.8
	float NormalForce = Mass * AccelerationDueToGravity; //car mass * gravity
	return -Velocity.GetSafeNormal()* RollingResistanceCoefficient * NormalForce; // becose is slowing down move forward
}

void UGoKartMovementComponent::ApplyRotaion(float DeltaTime, float InputSteeringThrow)
{
	//float RotaionAngle = MaxDegreesPerSecond * DeltaTime * SteeringThrow; //90 * 1sec * Lef/Right(-1,1) (use when fly turn in place)
	// DotProduct neede when go reverse/back
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime; //dotproduct 1 vector in one line 0 - 90 degree angle 
	float RotaionAngle = (DeltaLocation / MinTurningRadius) * InputSteeringThrow;

	//FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotaionAngle)); // rotate around Z axis about 90

	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotaionAngle);

	Velocity = RotationDelta.RotateVector(Velocity); //rotate velocity direction about RotationDelta value, same velocity directon as car rotaion

	GetOwner()->AddActorWorldRotation(RotationDelta);
}

void UGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime; // move in the world

	FHitResult Hit;


	GetOwner()->AddActorWorldOffset(Translation, true, &Hit); //add movement, pointer cn be optional, ref no

	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector(0.f); //or FVector::ZeroVector == FVector(0.f);
	}
}
