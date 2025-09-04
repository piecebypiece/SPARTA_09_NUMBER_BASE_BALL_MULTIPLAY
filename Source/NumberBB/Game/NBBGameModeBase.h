// NBBGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBBGameModeBase.generated.h"

class ANBBPlayerState;
class ANBBPlayerController;

/**
 *
 */
UCLASS()
class NUMBERBB_API ANBBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ANBBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ANBBPlayerState* InPlayerState);

	void ResetGame();

	void JudgeGame(ANBBPlayerState* InPlayerState, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ANBBPlayerController>> AllPlayerControllers;

private:
    // 시간 초과가 발생했을 때 호출될 함수
    void OnTurnTimeout();

    // 다음 플레이어의 턴을 시작시키는 함수
    void StartNextPlayerTurn();

    // 현재 턴의 시간 초과를 관리하는 타이머 핸들
    FTimerHandle TurnTimeoutHandle;

    // 현재 턴인 플레이어의 인덱스
    int32 CurrentPlayerIndex = -1;
};
