// NBBPawn.cpp


#include "NBBPawn.h"
#include "NumberBB.h"


void ANBBPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetModeString = NumberBBFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NBBPawn::BeginPlay() %s [%s]"), *NumberBBFunctionLibrary::GetNetModeString(this), *NetModeString);
	NumberBBFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ANBBPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetModeString = NumberBBFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NBBPawn::PossessedBy() %s [%s]"), *NumberBBFunctionLibrary::GetNetModeString(this), *NetModeString);
	NumberBBFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
