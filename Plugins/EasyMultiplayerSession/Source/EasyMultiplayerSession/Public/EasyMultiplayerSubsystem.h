// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Enums/EEMSJoinSessionCompleteResult.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Structs/FEMSOnlineSessionSearchResult.h"
#include "EasyMultiplayerSubsystem.generated.h"

#define MATCH_TYPE FName("MatchType")

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEasyMultiplayerDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerSessionCreatedDelegate, bool, bSessionCreated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEasyMultiplayerFindSessionDelegate, const TArray<FEMSOnlineSessionSearchResult>&, sessionResult, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerJoinSessionDelegate, EEMSJoinSessionCompleteResult,  joinResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerStartSessionDeletage, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEasyMultiplayerDestroySessionDelegate, bool, bSuccess);


UCLASS()
class EASYMULTIPLAYERSESSION_API UEasyMultiplayerSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerSessionCreatedDelegate OnSessionCreatedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerFindSessionDelegate OnSessionFoundEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerJoinSessionDelegate OnSessionJoinedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerStartSessionDeletage OnSessionStartedEvent;

	UPROPERTY(BlueprintAssignable)
	FEasyMultiplayerDestroySessionDelegate OnSessionDestroyedEvent;
	
private:
	// This property can hold the configurations to override if the user wants. -Renan
	class UEMSSessionCreationSettingsPDA* SessionCreationSettingsOverride;
	
	// As I can see this is just a typedef of some sort of TMap. But this is fine. -Renan
	IOnlineSessionPtr OnlineSubsystemSessionInterface;

	// This settings are used to store the session settings of the last online session I create. -Renan
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettings;

	// This session search is used to configure and retain everything that returns from the find session. -Renan
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearch;

	// This property handles the last TArray<FOnlineSessionSearchResult> when the user calls the Find session method. -Renan
	TArray<FOnlineSessionSearchResult> CachedFindServerSearchResult;
	
	// These events or delegates are called when the functions finish their execution. They need to be constructed first -Renan
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

	// This variables are used to create the session after destroing a existing session when the user tries to
	// create a session. If there is a current existing session, I need to destroy it first. And after the destroy event is called
	// I can create a brand new session. -Renan
	bool bCreateSessionAfterDestroy { false };
	int32 CachedNumberOfPublicPlayers;
	FString CachedMatchTypeName;
	
	
public:
	UEasyMultiplayerSubsystem();

	UFUNCTION(BlueprintCallable)
	void OverrideSessionCreationSettings(UEMSSessionCreationSettingsPDA* settingsToOverride);

	/**
	 * Method that creates a new online multiplayer session. This method also fires up an event in case the session was
	 * successfully created or not. You can bind a Custom Event Listeners to the OnSessionCreatedEvent to handle any use case.
	 * @param numberOfPublicConnections Number of public available slot for players to join.
	 * @param matchTypeName Name of the Session Match Type
	 */
	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 numberOfPublicConnections = 16, FString matchTypeName = "FreeForAll");

	/**
	 * Method to look for available sessions. This method broadcast a delegate with all the found sessions. You can bind a
	 * Custom Event Listener to the OnSessionFoundEvent to handle the find session result.
	 * @param maxOnlineSessionsSearchResult Max number of sessions to return.
	 */
	UFUNCTION(BlueprintCallable)
	void FindSession(int32 maxOnlineSessionsSearchResult);

	/**
	 * Method used to join a session
	 * @param onlineSessionSearchResult The session result that can be found using EasyMultiplayerSession::FindSessions method
	 */
	UFUNCTION(BlueprintCallable)
	void JoinSession(const FEMSOnlineSessionSearchResult& onlineSessionSearchResult);

	UFUNCTION(BlueprintCallable)
	void ConnectToJoinedSession();
	
	UFUNCTION(BlueprintCallable)
	void StartSession();

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void OpenGameLevelAsHostServer(FString pathToLobby);

protected:
	// Internal callbacks for the events that ared called by the session funcions. -Renan
	void OnCreateSessionEventListenerCallback(FName createdSessionName, bool bWasSuccessfullCreated);
	void OnFindSessionEventListenerCallback(bool bWasSuccessfull);
	void OnJoinSessionEventListenerCallback(FName joinedSessionName, EOnJoinSessionCompleteResult::Type joinResultType);
	void OnStartSessionEventListenerCallback(FName sessionName, bool bWasSuccessful);
	void OnDestroySessionEventListenerCallback(FName sessionName, bool bWasSuccessful);

private:
	bool IsOnlineSubsystemInterfaceValid() const;
	FEMSOnlineSessionSearchResult ConvertSessionResult(const FOnlineSessionSearchResult& sessionResult);
	EEMSJoinSessionCompleteResult ConvertJoinResult(const EOnJoinSessionCompleteResult::Type joinResultType);
	FOnlineSessionSearchResult GetRealSessionDataFromSessionPlaceholderData(FEMSOnlineSessionSearchResult sessionToFind);
};
