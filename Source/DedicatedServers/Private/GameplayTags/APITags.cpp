#include "GameplayTags/APITags.h"

namespace APITags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FindOrCreateGameSession, "API.FindOrCreateGameSession", "Retrieves a game session, creating a new one if none exist.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreatePlayerSession, "API.CreatePlayerSession", "Creates a new player session");
}