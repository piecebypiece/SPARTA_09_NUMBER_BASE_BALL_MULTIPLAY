// NBBPlayerState.cpp


#include "NBBPlayerState.h"

#include "NBBPlayerController.h"
#include "Game/NBBGameStateBase.h"
#include "Net/UnrealNetwork.h"

ANBBPlayerState::ANBBPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
	, TurnStartTime(-1.f)
{
	bReplicates = true;
}

void ANBBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);

	DOREPLIFETIME(ANBBPlayerState, TurnStartTime);
}

FString ANBBPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}

float ANBBPlayerState::GetRemainingTurnTime() const
{
    ANBBGameStateBase* NBBGameState = GetWorld()->GetGameState<ANBBGameStateBase>();
    if (NBBGameState && TurnStartTime > 0.f)
    {
        // (현재 서버 시간 - 내 턴 시작 시간)을 전체 턴 시간에서 빼서 남은 시간을 계산
        const float ElapsedTime = NBBGameState->GetServerWorldTimeSeconds() - TurnStartTime;
        return FMath::Max(0.f, NBBGameState->TurnTime - ElapsedTime);
    }
    return 0.f;
}
