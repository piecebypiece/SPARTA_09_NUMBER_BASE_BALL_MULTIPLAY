// NBBGameStateBase.cpp


#include "NBBGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/NBBPlayerController.h"
#include "EngineUtils.h"

void ANBBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ANBBPlayerController* NBBPC = Cast<ANBBPlayerController>(PC);
			if (IsValid(NBBPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				NBBPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

FString ANBBGameStateBase::GenerateSecretNumber()
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

bool ANBBGameStateBase::IsGuessNumberString(const FString& InNumberString)
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

FString ANBBGameStateBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
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

void ANBBGameStateBase::MulticastRPCBroadcastTurnTimeout_Implementation(const FString& PlayerName)
{
    // 클라이언트에서 이 함수가 호출되면, UI 업데이트나 채팅 메시지 출력 등을 처리
    // 예: FString::Printf(TEXT("%s님의 턴 시간이 초과되었습니다."), *PlayerName);
    // 지금은 UI를 고려하지 않으므로, 로그만 출력
    UE_LOG(LogTemp, Warning, TEXT("Player %s's turn has timed out."), *PlayerName);
}
