// NBBPlayerController.cpp


#include "Player/NBBPlayerController.h"

#include "UI/NBBChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NumberBB/NumberBB.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/NBBGameModeBase.h"
#include "Game/NBBGameStateBase.h"
#include "NBBPlayerState.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"

ANBBPlayerController::ANBBPlayerController()
{
	bReplicates = true;
}

void ANBBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UNBBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
	if (IsValid(TimerTextWidgetClass) == true)
	{
		TimerTextWidgetInstance = CreateWidget<UUserWidget>(this, TimerTextWidgetClass);
		if (IsValid(TimerTextWidgetInstance) == true)
		{
			TimerTextWidgetInstance->AddToViewport();
		}
	}
}

void ANBBPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocalController() == false)
	{
		return;
	}

	auto NBBPS = GetPlayerState<ANBBPlayerState>();
	ANBBGameStateBase* NBBGameState = GetWorld()->GetGameState<ANBBGameStateBase>();
	if (NBBPS == nullptr || NBBGameState == nullptr)
	{
		return;
	}

	if (NBBPS->TurnStartTime > 0.f)
	{
		FString Formatted = FString::Printf(TEXT("%.2f sec"), NBBPS->GetRemainingTurnTime());
		if (UTextBlock* TimerText = Cast<UTextBlock>(TimerTextWidgetInstance->GetWidgetFromName(TEXT("TimerText"))))
		{
			TimerText->SetText(FText::FromString(Formatted));
		}
		
	}
}

void ANBBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ANBBPlayerState* PS = GetPlayerState<ANBBPlayerState>();
		if (IsValid(PS) == true)
		{
			FString CombinedMessageString = PS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ANBBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	NumberBBFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ANBBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ANBBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ANBBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ANBBGameModeBase* NBBGM = Cast<ANBBGameModeBase>(GM);
		if (IsValid(NBBGM) == true)
		{
			NBBGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
