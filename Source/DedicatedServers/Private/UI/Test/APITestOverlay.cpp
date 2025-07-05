// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Test/APITestOverlay.h"
#include "HTTP/HTTPRequestManager.h"

void UAPITestOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	HTTPManager = NewObject<UHTTPRequestManager>(this);
	HTTPManager->SetAPIData(APIData);

	OnHTTPManagerSet();
}

