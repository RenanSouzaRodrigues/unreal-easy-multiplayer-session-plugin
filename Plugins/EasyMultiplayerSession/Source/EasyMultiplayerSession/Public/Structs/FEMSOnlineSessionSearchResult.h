#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "FEMSOnlineSessionSearchResult.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FEMSOnlineSessionSearchResult {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString SessionId;

	UPROPERTY(BlueprintReadOnly)
	FString OwningUserName;

	UPROPERTY(BlueprintReadOnly)
	int32 PingInMs;

	UPROPERTY(BlueprintReadOnly)
	int32 NumberOfPublicConnections;

	UPROPERTY(BlueprintReadOnly)
	int32 NumberOfPrivateConnections;

	UPROPERTY(BlueprintReadOnly)
	FString MatchType;

	FOnlineSessionSearchResult OriginalSearchResult;

public:
	bool IsValid() const {
		return this->OriginalSearchResult.IsValid();
	}
};
