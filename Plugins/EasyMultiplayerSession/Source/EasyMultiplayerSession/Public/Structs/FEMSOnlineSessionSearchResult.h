#pragma once

#include "CoreMinimal.h"
#include "FEMSOnlineSessionSearchResult.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FEMSOnlineSessionSearchResult {
	GENERATED_BODY()

public:
	FString SessionId;
	FString OwningUserName;
	int32 PingInMs;
	int32 NumberOfPublicConnections;
	int32 NumberOfPrivateConnections;

public:
	bool IsValid() const {
		return this->SessionId.Len() > 0;
	}
};
