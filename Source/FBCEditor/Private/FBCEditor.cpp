#include "FBCEditor.h"
#include "PropertyEditorModule.h"
#include "BitGridCustomization.h"

#define LOCTEXT_NAMESPACE "FFBCEditorModule"

void FFBCEditorModule::StartupModule()
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    PropertyEditorModule.RegisterCustomPropertyTypeLayout(
        "BitGrid",
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBitGridCustomization::MakeInstance)
	);

	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FFBCEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FFBCEditorModule, FBCEditor)