// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APIWidgetBase.h"
#include "HTTP/HTTPRequestManager.h"

void UAPIWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	HTTPManager = NewObject<UHTTPRequestManager>(this);
	HTTPManager->SetAPIData(APIData);

	OnHTTPManagerSet();
}

