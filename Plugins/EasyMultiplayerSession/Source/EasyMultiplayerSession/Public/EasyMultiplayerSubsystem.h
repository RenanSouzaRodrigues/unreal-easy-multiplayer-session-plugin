// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "EasyMultiplayerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEasyMultiplayerDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerSessionCreatedDelegate, bool, bSessionCreated);

UCLASS()
class EASYMULTIPLAYERSESSION_API UEasyMultiplayerSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerSessionCreatedDelegate OnSessionCreatedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerDelegate OnSessionFoundEvent;
	
	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerDelegate OnSessionJoinedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerDelegate OnSessionStartedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerDelegate OnSessionDestroyedEvent;
	
	
private:
	// As I can see this is just a typedef of some sort of TMap. But this is fine. -Renan
	IOnlineSessionPtr onlineSubsystemSessionInterface;

	// This settings are used to store the session settings of the last online session I create. -Renan
	TSharedPtr<FOnlineSessionSettings> lastOnlineSessionSettings;

	// These events or delegates are called when the functions finish their execution. -Renan
	FOnCreateSessionCompleteDelegate OnCreateSessionEvent;
	FOnFindSessionsCompleteDelegate OnFindSessionEvent;
	FOnJoinSessionCompleteDelegate OnJoinSessionEvent;
	FOnStartSessionCompleteDelegate OnStartSessionEvent;
	FOnDestroySessionCompleteDelegate OnDestroySessionEvent;

	// These delegate handles will be used when the main delegates are not needed anymore. -Renan
	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle FindSessionDelegateHandle;
	FDelegateHandle JoinSessionDelegateHandle;
	FDelegateHandle StartSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;
	
public:
	UEasyMultiplayerSubsystem();

	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 numberOfPublicConnections, FString matchType);

	UFUNCTION(BlueprintCallable)
	void FindSession(int32 maxOnlineSessionsSearchResult);

	UFUNCTION(BlueprintCallable)
	void JoinSession(const FOnlineSessionSearchResult& onlineSessionResult);

	UFUNCTION(BlueprintCallable)
	void StartSession();

	UFUNCTION(BlueprintCallable)
	void DestroySession();

protected:
	// Internal callbacks for the events that ared called by the session funcions. -Renan
	void OnCreateSessionEventListenerCallback(FName createdSessionName, bool bWasSuccessfullCreated);
	void OnFindSessionEventListenerCallback(bool bWasSuccessfull);
	void OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType);
	void OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful);
	void OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful);
};
