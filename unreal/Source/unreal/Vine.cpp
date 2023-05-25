// Fill out your copyright notice in the Description page of Project Settings.


#include "Vine.h"

// Sets default values
AVine::AVine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));

	// Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("StaticMesh'/Game/SM_VINE.SM_Vine'"));
	if (SM.Succeeded())
	{
		Mesh->SetStaticMesh(SM.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid vine mesh"));
	}
}

// Called when the game starts or when spawned
void AVine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

