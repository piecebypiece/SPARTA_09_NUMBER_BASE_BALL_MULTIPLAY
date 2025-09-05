// NBBPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBBPlayerController.generated.h"

class UNBBChatInput;
class UUserWidget;

/**
 *
 */
UCLASS()
class NUMBERBB_API ANBBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANBBPlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNBBChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UNBBChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> TimerTextWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> TimerTextWidgetInstance;
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;

	// UFUNCTION(BlueprintCallable)
	// void OnChangeTurnTime();

};
