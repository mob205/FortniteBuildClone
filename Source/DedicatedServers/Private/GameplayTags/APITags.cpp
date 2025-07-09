#include "GameplayTags/APITags.h"

namespace APITags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FindOrCreateGameSession, "API.FindOrCreateGameSession", "Retrieves a game session, creating a new one if none exist.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreatePlayerSession, "API.CreatePlayerSession", "Creates a new player session");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignUp, "API.Portal.SignUp", "Sign up for a new player account");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ConfirmSignUp, "API.Portal.ConfirmSignUp", "Confirms a new account using verification code");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignIn, "API.Portal.SignIn", "Retrieves auth tokens for user");
}