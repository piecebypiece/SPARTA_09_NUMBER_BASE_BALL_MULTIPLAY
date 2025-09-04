
#include "Game/NBBGameModeBase.h"
#include "NBBGameStateBase.h"
#include "Player/NBBPlayerController.h"
#include "EngineUtils.h"
#include "Player/NBBPlayerState.h"

void ANBBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ANBBPlayerController* NBBPlayerController = Cast<ANBBPlayerController>(NewPlayer);
	if (IsValid(NBBPlayerController) == true)
	{
		NBBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(NBBPlayerController);

		ANBBPlayerState* NBBPS = NBBPlayerController->GetPlayerState<ANBBPlayerState>();
		if (IsValid(NBBPS) == true)
		{
			NBBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ANBBGameStateBase* NBBGameStateBase = GetGameState<ANBBGameStateBase>();
		if (IsValid(NBBGameStateBase) == true)
		{
			NBBGameStateBase->MulticastRPCBroadcastLoginMessage(NBBPS->PlayerNameString);
		}
	}
}

FString ANBBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ANBBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ANBBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ANBBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
    CurrentPlayerIndex = -1;
}

void ANBBGameModeBase::PrintChatMessageString(ANBBPlayerController* InChattingPlayerController,
	const FString& InChatMessageString)
{
    // 현재 턴인 플레이어만 추측 가능
    if (GameState->PlayerArray.IsValidIndex(CurrentPlayerIndex))
    {
        ANBBPlayerState* CurrentPlayerState = Cast<ANBBPlayerState>(GameState->PlayerArray[CurrentPlayerIndex]);
        if (CurrentPlayerState != InChattingPlayerController->GetPlayerState<ANBBPlayerState>())
        {
            // 턴이 아닌 플레이어의 채팅은 그냥 출력만 함
            for (TActorIterator<ANBBPlayerController> It(GetWorld()); It; ++It)
            {
                ANBBPlayerController* NBBPlayerController = *It;
                if (IsValid(NBBPlayerController) == true)
                {
                    NBBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
                }
            }
            return;
        }
    }

	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController->GetPlayerState<ANBBPlayerState>());

		for (TActorIterator<ANBBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBBPlayerController* NBBPlayerController = *It;
			if (IsValid(NBBPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				NBBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController->GetPlayerState<ANBBPlayerState>(), StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<ANBBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBBPlayerController* NBBPlayerController = *It;
			if (IsValid(NBBPlayerController) == true)
			{
				NBBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void ANBBGameModeBase::IncreaseGuessCount(ANBBPlayerState* InPlayerState)
{
	if (IsValid(InPlayerState) == true)
	{
		InPlayerState->CurrentGuessCount++;
	}

    // 예정되어 있던 시간 초과 타이머를 취소
    GetWorldTimerManager().ClearTimer(TurnTimeoutHandle);

    // 다음 플레이어 턴 시작
    StartNextPlayerTurn();
}

void ANBBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& NBBPlayerController : AllPlayerControllers)
	{
		ANBBPlayerState* NBBPS = NBBPlayerController->GetPlayerState<ANBBPlayerState>();
		if (IsValid(NBBPS) == true)
		{
			NBBPS->CurrentGuessCount = 0;
			NBBPS->TurnStartTime = -1.f;
		}
	}
}

void ANBBGameModeBase::JudgeGame(ANBBPlayerState* InPlayerState, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		for (const auto& NBBPlayerController : AllPlayerControllers)
		{
			if (IsValid(InPlayerState) == true)
			{
				FString CombinedMessageString = InPlayerState->PlayerNameString + TEXT(" has won the game.");
				NBBPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBBPlayerController : AllPlayerControllers)
		{
			ANBBPlayerState* NBBPS = NBBPlayerController->GetPlayerState<ANBBPlayerState>();
			if (IsValid(NBBPS) == true)
			{
				if (NBBPS->CurrentGuessCount < NBBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& NBBPlayerController : AllPlayerControllers)
			{
				NBBPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}

void ANBBGameModeBase::OnTurnTimeout()
{
    if (GameState->PlayerArray.Num() == 0 || !GameState->PlayerArray.IsValidIndex(CurrentPlayerIndex)) return;

    ANBBPlayerState* TimedOutPlayerState = Cast<ANBBPlayerState>(GameState->PlayerArray[CurrentPlayerIndex]);
    if (TimedOutPlayerState)
    {
        // 모든 클라이언트에게 시간 초과 메시지 전송
        ANBBGameStateBase* NBBGameState = GetGameState<ANBBGameStateBase>();
        if (NBBGameState)
        {
            NBBGameState->MulticastRPCBroadcastTurnTimeout(TimedOutPlayerState->GetPlayerName());
        }
    }

    // 시간 초과로 인한 횟수 차감 및 다음 턴 시작
    IncreaseGuessCount(TimedOutPlayerState);
}

void ANBBGameModeBase::StartNextPlayerTurn()
{
    if (GameState->PlayerArray.Num() == 0) return;
	
	ANBBPlayerState* CurrentPS = Cast<ANBBPlayerState>(GameState->PlayerArray[CurrentPlayerIndex]);
	if (CurrentPS)
	{	// 현재 턴이었던 사람 시간 초기화 와 턴이 아닌 것을 알림
		CurrentPS->TurnStartTime = -1.f;
		auto PC = Cast<ANBBPlayerController>( CurrentPS->GetPlayerController());
		PC->NotificationText = FText::FromString(TEXT("Not your turn"));
	}
	
    // 순환적으로 다음 플레이어 인덱스 계산
    CurrentPlayerIndex = (CurrentPlayerIndex + 1) % GameState->PlayerArray.Num();

	ANBBPlayerState* NextPlayerState = Cast<ANBBPlayerState>(GameState->PlayerArray[CurrentPlayerIndex]);
    ANBBGameStateBase* NBBGameState = GetGameState<ANBBGameStateBase>();

    if (NextPlayerState && NBBGameState)
    {
        // 다음 플레이어의 턴 시작 시간을 현재 서버 시간으로 기록
        NextPlayerState->TurnStartTime = NBBGameState->GetServerWorldTimeSeconds();

        // 정확히 TurnTime초 후에 OnTurnTimeout 함수를 호출하는 일회성 타이머 설정
        GetWorldTimerManager().SetTimer(TurnTimeoutHandle, this,
        	&ANBBGameModeBase::OnTurnTimeout, NBBGameState->TurnTime, false);
    }
}