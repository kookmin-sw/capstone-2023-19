#include "Trunk.h"

// Sets default values
ATrunk::ATrunk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));


	// Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("StaticMesh'/Game/SM_Trunk.SM_Trunk'"));
	if (SM.Succeeded())
	{
		Mesh->SetStaticMesh(SM.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid basic cylinder mesh"));
	}

	// Mesh
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	//if (SM.Succeeded())
	//{
	//	Mesh->SetStaticMesh(SM.Object);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Invalid basic cylinder mesh"));
	//}

	//// Material
	////TrunkMaterialPath = GetTrunkMaterialPath();
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> Mat(TEXT("MaterialInstanceConstant'/Game/BarkPack/Materials/oak/oak_I.oak_I'"));
	//if (Mat.Succeeded())
	//{
	//	Mesh->SetMaterial(0, Mat.Object);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Invalid material"));
	//}

	// starting rotate
	FRotator rotator = FRotator(0.f, -90.f, 0.f);
	SetActorRotation(rotator);
}

// Called when the game starts or when spawned
void ATrunk::BeginPlay()
{
	Super::BeginPlay();

	TrunkMaterialPath = GetTrunkMaterialPath();
}

// Called every frame
void ATrunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

wchar_t* ATrunk::GetTrunkMaterialPath() const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), TrunkMaterialPath);
	switch (TrunkType)
	{
		case TrunkMaterialType::Beech_I:
		{
			return TEXT("MaterialInstanceConstant'/Game/BarkPack/Materials/beech/beech_I.beech_I'");
		}
		case TrunkMaterialType::Beech_II:
		{
			return TEXT("MaterialInstanceConstant'/Game/BarkPack/Materials/beech/beech_II.beech_II'");
		}
	}

	return TEXT("");
}

// Public
void ATrunk::SetTrunkMaterial(TrunkMaterialType type)
{
	TrunkType = type;
}