// NBBGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBBGameStateBase.generated.h"

class ACXPlayerController;

/**
 *
 */
UCLASS()
class NUMBERBB_API ANBBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

	UFUNCTION(NetMulticast, Reliable)
    void MulticastRPCBroadcastTurnTimeout(const FString& PlayerName);

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	const float TurnTime = 7.f;
};
