// Fill out your copyright notice in the Description page of Project Settings.


#include "Trunk.h"

// Sets default values
ATrunk::ATrunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("StaticMesh'/Game/SM_Trunk.SM_Trunk'"));

	if (SM.Succeeded())
	{
		Mesh->SetStaticMesh(SM.Object);
	}

	FRotator rotator = FRotator(0.f, -90.f, 0.f);
	SetActorRotation(rotator);
}

// Called when the game starts or when spawned
void ATrunk::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("%f %f %F"), GetActorRotation().Roll, GetActorRotation().Pitch, GetActorRotation().Yaw);
	UE_LOG(LogTemp, Warning, TEXT("%f %f %f %f"), GetActorRotation().Quaternion().W, GetActorRotation().Quaternion().X, GetActorRotation().Quaternion().Y, GetActorRotation().Quaternion().Z);
}

