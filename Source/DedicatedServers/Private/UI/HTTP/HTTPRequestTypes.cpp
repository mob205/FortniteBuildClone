#include "HTTPRequestTypes.h"
#include "DedicatedServers/DedicatedServersLogs.h"

void FDSMetaData::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("MetaData:"));

	UE_LOG(LogDedicatedServers, Log, TEXT("httpStatusCode: %d:"), httpStatusCode);

	UE_LOG(LogDedicatedServers, Log, TEXT("requestId: %s"), *requestId);
	
	UE_LOG(LogDedicatedServers, Log, TEXT("attempts: %d:"), attempts);

	UE_LOG(LogDedicatedServers, Log, TEXT("totalRetryDelay: %f"), totalRetryDelay);

}

void FDSListFleetsResponse::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("ListFleetResponse:"));
	for (const auto& FleetId : FleetIds)
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("FleetId: %s"), *FleetId);
	}
	if (!NextToken.IsEmpty())
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("NextToken: %s"), *NextToken);
	}
}
