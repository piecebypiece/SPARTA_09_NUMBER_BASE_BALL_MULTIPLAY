// NBBPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBBPlayerState.generated.h"

/**
 *
 */
UCLASS()
class NUMBERBB_API ANBBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANBBPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString();

public:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	UPROPERTY(Replicated)
	float TurnStartTime;

	// UFUNCTION()
	// void OnRep_TurnStartTime();

	UFUNCTION(BlueprintPure, Category = "Game")
	float GetRemainingTurnTime() const;
};
