// NBBGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBBGameStateBase.generated.h"

class ANBBPlayerController;

/**
 *
 */
UCLASS()
class NUMBERBB_API ANBBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	

public:
	ANBBGameStateBase()
		: bIsRunningGame(false)
	{
	}
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);
	const float TurnTime = 7.f;


	UPROPERTY(Replicated)
	bool bIsRunningGame;
};
