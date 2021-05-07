#include <aer/managers/menu_manager.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <aer/aer_engine.h>
//TODO: modelmanager.countmodeltoload/modelmanager.CountAllofModel

namespace neko::aer
{
MenuManager::MenuManager(AerEngine& engine) : engine_(engine)
{
    
}

void MenuManager::Init()
{

    const auto& config = BasicEngine::GetInstance()->GetConfig();
    startTextUi_ = UiText(FontLoaded::ROBOTO, "Start", Vec2i(Vec2f(0.0f, 1.0f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    optionsTextUi_ = UiText(FontLoaded::ROBOTO, "Options", Vec2i(Vec2f(0.0f, 0.75f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    highscoreTextUi_ = UiText(FontLoaded::ROBOTO, "Credits", Vec2i(Vec2f(0.0f, 0.5f) * Vec2f(config.windowSize)), UiAnchor::BOTTOM, 0, 2.0f, Color::grey);
    menuBackGroundUI = UiImage(config.dataRootPath + menuBackgroundUIPath_, Vec2i(Vec2f().zero), config.windowSize, UiAnchor::CENTER, 0, Color::white);
    menuBackGroundUI.SetEnable(false);
    logoUi_ = UiImage(config.dataRootPath + "sprites/ui/centered/logo.png", Vec2i(Vec2f(0.0f, 0.5f) * Vec2f(config.windowSize)), Vec2u(Vec2f(1000, 500)*0.5f), UiAnchor::CENTER, 0, Color::white);
    menuStatus_ = MenuStatus::SLEEP;
    creditsStatus_ = CreditsStatus::LEADS;

    creditsSebUiNameText_ = UiText(FontLoaded::ROBOTO, creditsSebNameText_, Vec2i(Vec2f(-0.5, 0.6) * Vec2f(config.windowSize)), UiAnchor::CENTER,0, 1.0f, creditsColor_);
    creditsSebUiLPText_ = UiText(FontLoaded::ROBOTO, creditsSebLPText_, Vec2i(Vec2f(-0.5, 0.5) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsSebUiGPText_ = UiText(FontLoaded::ROBOTO, creditsSebGPText_, Vec2i(Vec2f(-0.5, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsSebUiGDText_ = UiText(FontLoaded::ROBOTO, creditsSebGDText_, Vec2i(Vec2f(-0.5, 0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
	
    creditsSimonUiNameText_ = UiText(FontLoaded::ROBOTO, creditsSimonNameText_, Vec2i(Vec2f(0.5, 0.6) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsSimonUiLTText_ = UiText(FontLoaded::ROBOTO, creditsSimonLTText_, Vec2i(Vec2f(0.5, 0.5) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsSimonUiLEText_ = UiText(FontLoaded::ROBOTO, creditsSimonLEText_, Vec2i(Vec2f(0.5, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);


    creditsStephenUiNameText_ = UiText(FontLoaded::ROBOTO, creditsStephenNameText_, Vec2i(Vec2f(-0.5, -0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsStephenUiDOText_ = UiText(FontLoaded::ROBOTO, creditsStephenDOText_, Vec2i(Vec2f(-0.5, -0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditStephenUiGDsText_ = UiText(FontLoaded::ROBOTO, creditStephenGDsText_, Vec2i(Vec2f(-0.5, -0.5) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsStephenUiGPText_ = UiText(FontLoaded::ROBOTO, creditsStephenGPText_, Vec2i(Vec2f(-0.5, -0.6) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);

    creditsLucaUiNameText_ = UiText(FontLoaded::ROBOTO, creditsLucaNameText_, Vec2i(Vec2f(0.5, -0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsLucaUiPText_ = UiText(FontLoaded::ROBOTO, creditsLucaPText_, Vec2i(Vec2f(0.5, -0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsLucaUiLPText_ = UiText(FontLoaded::ROBOTO, creditsLucaLPText_, Vec2i(Vec2f(0.5, -0.5) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsLucaUiGPText_ = UiText(FontLoaded::ROBOTO, creditsLucaGPText_, Vec2i(Vec2f(0.5, -0.6) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);

    creditsArtsUiText_ = UiText(FontLoaded::LOBSTER, creditsArtsText_, Vec2i(Vec2f(0, 0.7) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, creditsColor_);

    creditsUiJKText_ = UiText(FontLoaded::ROBOTO, creditsJKText_, Vec2i(Vec2f(-0.6, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiHCText_ = UiText(FontLoaded::ROBOTO, creditsHCText_, Vec2i(Vec2f(-0.6, 0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiEMText_ = UiText(FontLoaded::ROBOTO, creditsEMText_, Vec2i(Vec2f(-0.6, 0.2) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiCBText_ = UiText(FontLoaded::ROBOTO, creditsCBText_, Vec2i(Vec2f(-0.6, 0.1) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiMCText_ = UiText(FontLoaded::ROBOTO, creditsMCText_, Vec2i(Vec2f(-0.6, 0.0) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
	
    creditsUiMGText_ = UiText(FontLoaded::ROBOTO, creditsMGText_, Vec2i(Vec2f(0.6, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiLMText_ = UiText(FontLoaded::ROBOTO, creditsLMText_, Vec2i(Vec2f(0.6, 0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiTMText_ = UiText(FontLoaded::ROBOTO, creditsTMText_, Vec2i(Vec2f(0.6, 0.2) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiNZText_ = UiText(FontLoaded::ROBOTO, creditsNZText_, Vec2i(Vec2f(0.6, 0.1) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiNVText_ = UiText(FontLoaded::ROBOTO, creditsNVText_, Vec2i(Vec2f(0.6, 0.0) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);

    creditsToolsUiText_ = UiText(FontLoaded::LOBSTER, creditsToolsText_, Vec2i(Vec2f(0, 0.7) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, creditsColor_);

    creditsUiDVAText_ = UiText(FontLoaded::ROBOTO, creditsDVAText_, Vec2i(Vec2f(-0.6, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiWPText_ = UiText(FontLoaded::ROBOTO, creditsWPText_, Vec2i(Vec2f(-0.6, 0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiLDText_ = UiText(FontLoaded::ROBOTO, creditsLDText_, Vec2i(Vec2f(-0.6, 0.2) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiVDText_ = UiText(FontLoaded::ROBOTO, creditsVDText_, Vec2i(Vec2f(-0.6, 0.1) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiGJText_ = UiText(FontLoaded::ROBOTO, creditsGJText_, Vec2i(Vec2f(-0.6, 0.0) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
	
    creditsUiSSText_ = UiText(FontLoaded::ROBOTO, creditsSSText_, Vec2i(Vec2f(0.6, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiMSText_ = UiText(FontLoaded::ROBOTO, creditsMSText_, Vec2i(Vec2f(0.6, 0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiBCText_ = UiText(FontLoaded::ROBOTO, creditsBCText_, Vec2i(Vec2f(0.6, 0.2) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiANText_ = UiText(FontLoaded::ROBOTO, creditsANText_, Vec2i(Vec2f(0.6, 0.1) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);

    creditsAudioUiText_ = UiText(FontLoaded::LOBSTER, creditsAudioText_, Vec2i(Vec2f(0, 0.7) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, creditsColor_);

    creditsUiLTText_ = UiText(FontLoaded::ROBOTO, creditsLTText_, Vec2i(Vec2f(-0.6, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsUiLJText_ = UiText(FontLoaded::ROBOTO, creditsLJText_, Vec2i(Vec2f(0.6, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);

    creditsThanksUiText_ = UiText(FontLoaded::LOBSTER, creditsThanksText_, Vec2i(Vec2f(0, 0.7) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, creditsColor_);
    creditsEliasUiText_ = UiText(FontLoaded::ROBOTO, creditsEliasText_, Vec2i(Vec2f(0.0, 0.4) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsFredUiText_ = UiText(FontLoaded::ROBOTO, creditsFredText_, Vec2i(Vec2f(0.0, 0.3) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsSylvainUiText_ = UiText(FontLoaded::ROBOTO, creditsSylvainText_, Vec2i(Vec2f(0.0, 0.2) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);
    creditsTesterUiText_ = UiText(FontLoaded::ROBOTO, creditsTestersText_, Vec2i(Vec2f(0.0, 0.1) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 1.0f, creditsColor_);

    loadingText_ = UiText(FontLoaded::ROBOTO, loading_ + "0 %", Vec2i::zero, UiAnchor::CENTER, 0, 4.0f, Color::white);
	//TODO: Create text for others
	
    useMenu = false;

    //Audio
    ComponentManagerContainer& cContainer = engine_.GetComponentManagerContainer();
    bipAudioEntity_ = cContainer.entityManager.CreateEntity();

    cContainer.transform3dManager.AddComponent(bipAudioEntity_);
    cContainer.audioManager.AddComponent(bipAudioEntity_);
    cContainer.audioManager.SetEventName(bipAudioEntity_, "menu/menu_bleep");
    cContainer.audioManager.SetPlayOnWakeUp(bipAudioEntity_, false);
    cContainer.audioManager.SetMaxDistance(bipAudioEntity_, 40.0f);
    cContainer.audioManager.SetVolume(bipAudioEntity_, 100.0f);
    cContainer.audioManager.Init();

    selectAudioEntity_ = cContainer.entityManager.CreateEntity();

    cContainer.transform3dManager.AddComponent(selectAudioEntity_);
    cContainer.audioManager.AddComponent(selectAudioEntity_);
    cContainer.audioManager.SetEventName(selectAudioEntity_, "menu/menu_next");
    cContainer.audioManager.SetPlayOnWakeUp(selectAudioEntity_, false);
    cContainer.audioManager.SetMaxDistance(selectAudioEntity_, 40.0f);
    cContainer.audioManager.SetVolume(selectAudioEntity_, 100.0f);
    cContainer.audioManager.Init();

    for (int i = 0; i < 4; i++)
    {
        joinUi_[i] = UiText(FontLoaded::ROBOTO, "Join", Vec2i((Vec2f(0.0f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        leftArrowUi_[i] = UiText(FontLoaded::ROBOTO, "<-", Vec2i((Vec2f(-0.2f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        rightArrowUi_[i] = UiText(FontLoaded::ROBOTO, "->", Vec2i((Vec2f(+0.2f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);
        // selectedShipUi_[i] = ;
        // selectedColorUi_[i] = ;
        Color4 colorBlue = Color::blue;
        Color4 colorGreen = Color::green;
        Color4 colorRed = Color::red;
        Color4 colorYellow = Color::yellow;
        colorBlue = Color4(colorBlue.x * 0.5f, colorBlue.y * 0.5f, colorBlue.z * 0.5f, 1.0);
        colorGreen = Color4(colorGreen.x * 0.5f, colorGreen.y * 0.5f, colorGreen.z * 0.5f, 1.0);
        colorRed = Color4(colorRed.x * 0.5f, colorRed.y * 0.5f, colorRed.z * 0.5f, 1.0);
        colorYellow = Color4(colorYellow.x * 0.5f, colorYellow.y * 0.5f, colorYellow.z * 0.5f, 1.0);

        colorBlueUi_[i] = UiImage(config.dataRootPath + Rosso1UIPath_, Vec2i((Vec2f(-0.15f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorBlue);
        colorGreenUi_[i] = UiImage(config.dataRootPath + Rosso1UIPath_, Vec2i((Vec2f(-0.05f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorGreen);
        colorRedUi_[i] = UiImage(config.dataRootPath + Rosso1UIPath_, Vec2i((Vec2f(0.05f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorRed);
        colorYellowUi_[i] = UiImage(config.dataRootPath + Rosso1UIPath_, Vec2i((Vec2f(0.15f, -0.5f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), Vec2u(colorsSize, colorsSize), UiAnchor::CENTER, 0, colorYellow);
        readyUi_[i] = UiText(FontLoaded::ROBOTO, "Ready", Vec2i((Vec2f(0.0f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)), UiAnchor::CENTER, 0, 2.0f, Color::white);


        selectionBackgroundUI_[i] = UiImage(config.dataRootPath + blueprint1UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);
        readyBackground_[i] = UiImage(config.dataRootPath + ReadyBackgroundUIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);
        readyText_[i] = UiImage(config.dataRootPath + ReadyTextUIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);
        joinText_[i] = UiImage(config.dataRootPath + pressStartTextUIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(Vec2f(config.windowSize)*0.5f), UiAnchor::CENTER, 0, Color::white);

        rosso1UI_[i] = UiImage(config.dataRootPath + Rosso1UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))),Vec2u(1600 * shipModelsUiMultiplier, 1600 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        rosso2UI_[i] = UiImage(config.dataRootPath + Rosso2UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1600 * shipModelsUiMultiplier, 1600 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        rosso3UI_[i] = UiImage(config.dataRootPath + Rosso3UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1600 * shipModelsUiMultiplier, 1600 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        rosso4UI_[i] = UiImage(config.dataRootPath + Rosso4UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1600 * shipModelsUiMultiplier, 1600 * shipModelsUiMultiplier), UiAnchor::CENTER,0, Color::white);
        cortese1UI_[i] = UiImage(config.dataRootPath + Cortese1UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1600 * shipModelsUiMultiplier, 1600 * shipModelsUiMultiplier), UiAnchor::CENTER, 0, Color::white);
        cortese2UI_[i] = UiImage(config.dataRootPath + Cortese2UIPath_, Vec2i((playerScreenOffsets[i] * Vec2f(config.windowSize))), Vec2u(1600 * shipModelsUiMultiplier, 1600 * shipModelsUiMultiplier), UiAnchor::CENTER, 0, Color::white);


    	shipSkins[i] = SelectedModel::ROSSO_1;
        //arrowsUi_[i] = UiImage(config.dataRootPath);
    }

    auto& uiManager = UiManagerLocator::get();
    uiManager.AddUiText(&creditsSebUiNameText_);
    uiManager.AddUiText(&creditsSebUiLPText_);
    uiManager.AddUiText(&creditsSebUiGPText_);
    uiManager.AddUiText(&creditsSebUiGDText_);

    uiManager.AddUiText(&creditsSimonUiNameText_);
    uiManager.AddUiText(&creditsSimonUiLEText_);
    uiManager.AddUiText(&creditsSimonUiLTText_);

    uiManager.AddUiText(&creditsStephenUiNameText_);
    uiManager.AddUiText(&creditsStephenUiDOText_);
    uiManager.AddUiText(&creditsStephenUiGPText_);
    uiManager.AddUiText(&creditStephenUiGDsText_);

    uiManager.AddUiText(&creditsLucaUiNameText_);
    uiManager.AddUiText(&creditsLucaUiPText_);
    uiManager.AddUiText(&creditsLucaUiGPText_);
    uiManager.AddUiText(&creditsLucaUiLPText_);

    uiManager.AddUiText(&creditsArtsUiText_);

    uiManager.AddUiText(&creditsUiJKText_);
    uiManager.AddUiText(&creditsUiHCText_);
    uiManager.AddUiText(&creditsUiEMText_);
    uiManager.AddUiText(&creditsUiCBText_);
    uiManager.AddUiText(&creditsUiMCText_);
    uiManager.AddUiText(&creditsUiMGText_);
    uiManager.AddUiText(&creditsUiLMText_);
    uiManager.AddUiText(&creditsUiTMText_);
    uiManager.AddUiText(&creditsUiNZText_);
    uiManager.AddUiText(&creditsUiNVText_);
	
    uiManager.AddUiText(&creditsToolsUiText_);
	
    uiManager.AddUiText(&creditsUiDVAText_);
    uiManager.AddUiText(&creditsUiWPText_);
    uiManager.AddUiText(&creditsUiLDText_);
    uiManager.AddUiText(&creditsUiVDText_);
    uiManager.AddUiText(&creditsUiGJText_);
    uiManager.AddUiText(&creditsUiSSText_);
    uiManager.AddUiText(&creditsUiMSText_);
    uiManager.AddUiText(&creditsUiBCText_);
    uiManager.AddUiText(&creditsUiANText_);

	
    uiManager.AddUiText(&creditsAudioUiText_);

    uiManager.AddUiText(&creditsUiLTText_);
    uiManager.AddUiText(&creditsUiLJText_);

    uiManager.AddUiText(&creditsThanksUiText_);
    uiManager.AddUiText(&creditsEliasUiText_);
    uiManager.AddUiText(&creditsFredUiText_);
    uiManager.AddUiText(&creditsSylvainUiText_);
    uiManager.AddUiText(&creditsTesterUiText_);

    uiManager.AddUiText(&loadingText_);
	
    //TODO add ui for others

	
    creditsSebUiNameText_.SetEnable(false);
    creditsSebUiLPText_.SetEnable(false);
    creditsSebUiGPText_.SetEnable(false);
    creditsSebUiGDText_.SetEnable(false);

	//TODO: Set enable others
    creditsSimonUiNameText_.SetEnable(false);
    creditsSimonUiLTText_.SetEnable(false);
    creditsSimonUiLEText_.SetEnable(false);

    creditsStephenUiNameText_.SetEnable(false);
    creditsStephenUiDOText_.SetEnable(false);
    creditStephenUiGDsText_.SetEnable(false);
    creditsStephenUiGPText_.SetEnable(false);

    creditsLucaUiNameText_.SetEnable(false);
    creditsLucaUiPText_.SetEnable(false);
    creditsLucaUiLPText_.SetEnable(false);
    creditsLucaUiGPText_.SetEnable(false);

    creditsArtsUiText_.SetEnable(false);

    creditsUiJKText_.SetEnable(false);
    creditsUiHCText_.SetEnable(false);
    creditsUiEMText_.SetEnable(false);
    creditsUiCBText_.SetEnable(false);
    creditsUiMCText_.SetEnable(false);
    creditsUiMGText_.SetEnable(false);
    creditsUiLMText_.SetEnable(false);
    creditsUiTMText_.SetEnable(false);
    creditsUiNZText_.SetEnable(false);
    creditsUiNVText_.SetEnable(false);

    creditsToolsUiText_.SetEnable(false);

    creditsUiDVAText_.SetEnable(false);
    creditsUiWPText_.SetEnable(false);
    creditsUiLDText_.SetEnable(false);
    creditsUiVDText_.SetEnable(false);
    creditsUiGJText_.SetEnable(false);
    creditsUiSSText_.SetEnable(false);
    creditsUiMSText_.SetEnable(false);
    creditsUiBCText_.SetEnable(false);
    creditsUiANText_.SetEnable(false);

    creditsAudioUiText_.SetEnable(false);

    creditsUiLTText_.SetEnable(false);
    creditsUiLJText_.SetEnable(false);

    creditsThanksUiText_.SetEnable(false);
    creditsEliasUiText_.SetEnable(false);
    creditsFredUiText_.SetEnable(false);
    creditsSylvainUiText_.SetEnable(false);
    creditsTesterUiText_.SetEnable(false);

    loadingText_.SetEnable(false);
	
    uiManager.AddUiImage(&menuBackGroundUI);
    uiManager.AddUiImage(&logoUi_);
    logoUi_.SetEnable(false);
    uiManager.AddUiText(&startTextUi_);
    startTextUi_.SetEnable(false);
    uiManager.AddUiText(&optionsTextUi_);
    optionsTextUi_.SetEnable(false);
    uiManager.AddUiText(&highscoreTextUi_);
    highscoreTextUi_.SetEnable(false);
    for (int i = 0; i < 4; i++)
    {
        uiManager.AddUiText(&joinUi_[i]);
        joinUi_[i].SetEnable(false);
        uiManager.AddUiText(&leftArrowUi_[i]);
        leftArrowUi_[i].SetEnable(false);
        uiManager.AddUiText(&rightArrowUi_[i]);
        rightArrowUi_[i].SetEnable(false);
        uiManager.AddUiText(&readyUi_[i]);
        readyUi_[i].SetEnable(false);
        uiManager.AddUiImage(&colorBlueUi_[i]);
        uiManager.AddUiImage(&colorGreenUi_[i]);
        uiManager.AddUiImage(&colorRedUi_[i]);
        uiManager.AddUiImage(&colorYellowUi_[i]);
        colorBlueUi_[i].SetEnable(false);

        rosso1UI_[i].SetEnable(false);
        rosso2UI_[i].SetEnable(false);
        rosso3UI_[i].SetEnable(false);
        rosso4UI_[i].SetEnable(false);
        cortese1UI_[i].SetEnable(false);
        cortese2UI_[i].SetEnable(false);

        uiManager.AddUiImage(&selectionBackgroundUI_[i]);
        uiManager.AddUiImage(&readyBackground_[i]);
        uiManager.AddUiImage(&readyText_[i]);
        uiManager.AddUiImage(&joinText_[i]);

        uiManager.AddUiImage(&rosso1UI_[i]);
        uiManager.AddUiImage(&rosso2UI_[i]);
        uiManager.AddUiImage(&rosso3UI_[i]);
        uiManager.AddUiImage(&rosso4UI_[i]);

        uiManager.AddUiImage(&cortese1UI_[i]);
        uiManager.AddUiImage(&cortese2UI_[i]);

        readyBackground_[i].SetEnable(false);
        readyText_[i].SetEnable(false);
        joinText_[i].SetEnable(false);
        selectionBackgroundUI_[i].SetEnable(false);

        hasStartedSceneLoading = false;
    }
}

void MenuManager::Update(seconds dt)
{
    if (useMenu)
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();


        auto& inputlocator = sdl::InputLocator::get();
        menuBackGroundUI.SetEnable(false);
        logoUi_.SetEnable(false);
        startTextUi_.SetEnable(false);
        optionsTextUi_.SetEnable(false);
        highscoreTextUi_.SetEnable(false);

        creditsSebUiNameText_.SetEnable(false);
        creditsSebUiLPText_.SetEnable(false);
        creditsSebUiGPText_.SetEnable(false);
        creditsSebUiGDText_.SetEnable(false);
    	
        creditsSimonUiNameText_.SetEnable(false);
        creditsSimonUiLTText_.SetEnable(false);
        creditsSimonUiLEText_.SetEnable(false);

        creditsStephenUiNameText_.SetEnable(false);
        creditsStephenUiDOText_.SetEnable(false);
        creditStephenUiGDsText_.SetEnable(false);
        creditsStephenUiGPText_.SetEnable(false);

        creditsLucaUiNameText_.SetEnable(false);
        creditsLucaUiPText_.SetEnable(false);
        creditsLucaUiLPText_.SetEnable(false);
        creditsLucaUiGPText_.SetEnable(false);

        creditsArtsUiText_.SetEnable(false);

        creditsUiJKText_.SetEnable(false);
        creditsUiHCText_.SetEnable(false);
        creditsUiEMText_.SetEnable(false);
        creditsUiCBText_.SetEnable(false);
        creditsUiMCText_.SetEnable(false);
        creditsUiMGText_.SetEnable(false);
        creditsUiLMText_.SetEnable(false);
        creditsUiTMText_.SetEnable(false);
        creditsUiNZText_.SetEnable(false);
        creditsUiNVText_.SetEnable(false);

        creditsToolsUiText_.SetEnable(false);

        creditsUiDVAText_.SetEnable(false);
        creditsUiWPText_.SetEnable(false);
        creditsUiLDText_.SetEnable(false);
        creditsUiVDText_.SetEnable(false);
        creditsUiGJText_.SetEnable(false);
        creditsUiSSText_.SetEnable(false);
        creditsUiMSText_.SetEnable(false);
        creditsUiBCText_.SetEnable(false);
        creditsUiANText_.SetEnable(false);

        creditsAudioUiText_.SetEnable(false);

        creditsUiLTText_.SetEnable(false);
        creditsUiLJText_.SetEnable(false);

        creditsThanksUiText_.SetEnable(false);
        creditsEliasUiText_.SetEnable(false);
        creditsFredUiText_.SetEnable(false);
        creditsSylvainUiText_.SetEnable(false);
        creditsTesterUiText_.SetEnable(false);

        loadingText_.SetEnable(false);

        lastMenuPointing_ = mainMenuPointing_;
        lastMenuStatus_ = menuStatus_;

        for (int i = 0; i < kMaxPlayer_; i++)
        {
            lastSelectedSkins_[i] = shipSkins[i];
        }

        for (int i = 0; i < 4; i++)
        {
            joinUi_[i].SetEnable(false);
            leftArrowUi_[i].SetEnable(false);
            rightArrowUi_[i].SetEnable(false);
            // selectedShipUi_[i] = ;
            // selectedColorUi_[i] = ;
            colorBlueUi_[i].SetColor(Color::clear);
            colorGreenUi_[i].SetColor(Color::clear);
            colorRedUi_[i].SetColor(Color::clear);
            colorYellowUi_[i].SetColor(Color::clear);
            readyUi_[i].SetText("");
            colorBlueUi_[i].SetEnable(false);
            // colorGreenUi_[i] = ;
            // colorRedUi_[i] = ;
            // colorYellowUi_[i] = ;
            readyUi_[i].SetEnable(false);

            readyBackground_[i].SetEnable(false);
            readyText_[i].SetEnable(false);
            joinText_[i].SetEnable(false);
            selectionBackgroundUI_[i].SetEnable(false);

            rosso1UI_[i].SetEnable(false);
            rosso2UI_[i].SetEnable(false);
            rosso3UI_[i].SetEnable(false);
            rosso4UI_[i].SetEnable(false);

            cortese1UI_[i].SetEnable(false);
            cortese2UI_[i].SetEnable(false);
        }


        switch (menuStatus_)
        {
        case MenuStatus::MENU:
            startTextUi_.SetText("Start");
            optionsTextUi_.SetText("Options");
            highscoreTextUi_.SetText("Credits");
            logoUi_.SetEnable(true);
            startTextUi_.SetEnable(true);
            //optionsTextUi_.SetEnable(true);
            highscoreTextUi_.SetEnable(true);
            menuBackGroundUI.SetEnable(true);
            menuBackGroundUI.SetSize(config.windowSize);
            switch (mainMenuPointing_)
            {
            case MainMenuPointing::START:
                startTextUi_.SetColor(Color::white);
                optionsTextUi_.SetColor(Color::grey);
                highscoreTextUi_.SetColor(Color::grey);

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) >= 0.7 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) < 0.7)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) <= -0.7 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) > -0.7)
                {
                    isDpadUp_[0] = false;
                }

                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    menuStatus_ = MenuStatus::SELECTION;
                    for (int i = 0; i < 4; i++)
                    {
                        selectionPointing_[i] = SelectionPointing::JOIN;
                    }
                    selectionPointing_[0] = SelectionPointing::SHIP_TYPE;
                }

                break;
            case MainMenuPointing::HIGH_SCORE:
                startTextUi_.SetColor(Color::grey);
                optionsTextUi_.SetColor(Color::grey);
                highscoreTextUi_.SetColor(Color::white);
                menuBackGroundUI.SetEnable(true);
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) >= 0.7 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) < 0.7)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) <= -0.7 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) > -0.7)
                {
                    isDpadUp_[0] = false;
                }

                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    menuStatus_ = MenuStatus::CREDITS;
                }

                break;
            /*case MainMenuPointing::OPTIONS:
                startTextUi_.SetColor(Color::grey);
                optionsTextUi_.SetColor(Color::white);
                highscoreTextUi_.SetColor(Color::grey);
                menuBackGroundUI.SetEnable(true);
                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) >= 0.7 && !isDpadDown_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::HIGH_SCORE;
                    isDpadDown_[0] = true;
                }
                else if (isDpadDown_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) < 0.7)
                {
                    isDpadDown_[0] = false;
                }

                if (inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) <= -0.7 && !isDpadUp_[0])
                {
                    mainMenuPointing_ = MainMenuPointing::START;
                    isDpadUp_[0] = true;
                }
                else if (isDpadUp_[0] && inputlocator.GetControllerAxis(0, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS) > -0.7)
                {
                    isDpadUp_[0] = false;
                }

                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    menuStatus_ = MenuStatus::OPTIONS;
                }

                break;*/
            case MainMenuPointing::SLEEP:
                break;
            }
            break;
        case MenuStatus::OPTIONS:
            if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
            {
                menuStatus_ = MenuStatus::MENU;
            }
            break;
        case MenuStatus::CREDITS:

            menuBackGroundUI.SetEnable(true);

            switch (creditsStatus_)
            {
            case CreditsStatus::LEADS:
                creditsSebUiNameText_.SetEnable(true);
                creditsSebUiLPText_.SetEnable(true);
                creditsSebUiGPText_.SetEnable(true);
                creditsSebUiGDText_.SetEnable(true);

                creditsSimonUiNameText_.SetEnable(true);
                creditsSimonUiLTText_.SetEnable(true);
                creditsSimonUiLEText_.SetEnable(true);

                creditsStephenUiNameText_.SetEnable(true);
                creditsStephenUiDOText_.SetEnable(true);
                creditStephenUiGDsText_.SetEnable(true);
                creditsStephenUiGPText_.SetEnable(true);

                creditsLucaUiNameText_.SetEnable(true);
                creditsLucaUiPText_.SetEnable(true);
                creditsLucaUiLPText_.SetEnable(true);
                creditsLucaUiGPText_.SetEnable(true);
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::ARTS;
                }
                break;
            case CreditsStatus::ARTS:
                creditsArtsUiText_.SetEnable(true);

                creditsUiJKText_.SetEnable(true);
                creditsUiHCText_.SetEnable(true);
                creditsUiEMText_.SetEnable(true);
                creditsUiCBText_.SetEnable(true);
                creditsUiMCText_.SetEnable(true);
                creditsUiMGText_.SetEnable(true);
                creditsUiLMText_.SetEnable(true);
                creditsUiTMText_.SetEnable(true);
                creditsUiNZText_.SetEnable(true);
                creditsUiNVText_.SetEnable(true);


                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::AUDIO;
                }
                break;
            case CreditsStatus::AUDIO:

                creditsAudioUiText_.SetEnable(true);

                creditsUiLTText_.SetEnable(true);
                creditsUiLJText_.SetEnable(true);
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::TOOLS;
                }
                break;
            case CreditsStatus::TOOLS:
                creditsToolsUiText_.SetEnable(true);

                creditsUiDVAText_.SetEnable(true);
                creditsUiWPText_.SetEnable(true);
                creditsUiLDText_.SetEnable(true);
                creditsUiVDText_.SetEnable(true);
                creditsUiGJText_.SetEnable(true);
                creditsUiSSText_.SetEnable(true);
                creditsUiMSText_.SetEnable(true);
                creditsUiBCText_.SetEnable(true);
                creditsUiANText_.SetEnable(true);

                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::SPECIAL_THANKS;
                }
                break;
            case CreditsStatus::EXTRAS:
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::SPECIAL_THANKS;
                }
                break;
            case CreditsStatus::SPECIAL_THANKS:
                creditsThanksUiText_.SetEnable(true);
                creditsEliasUiText_.SetEnable(true);
                creditsFredUiText_.SetEnable(true);
                creditsSylvainUiText_.SetEnable(true);
                creditsTesterUiText_.SetEnable(true);
                if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                {
                    creditsStatus_ = CreditsStatus::LEADS;
                    menuStatus_ = MenuStatus::MENU;
                }
                break;
            }
            if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
            {
                creditsStatus_ = CreditsStatus::LEADS;
                menuStatus_ = MenuStatus::MENU;
            }
            break;
        case MenuStatus::SELECTION:
            for (int i = 0; i < kMaxPlayer_; i++)
            {
                colorBlueUi_[i].SetColor(Color4(Color::blue.x, Color::blue.y, Color::blue.z, 0.5f));
                colorGreenUi_[i].SetColor(Color4(Color::green.x, Color::green.y, Color::green.z, 0.5f));
                colorRedUi_[i].SetColor(Color4(Color::red.x, Color::red.y, Color::red.z, 0.5f));
                colorYellowUi_[i].SetColor(Color4(Color::yellow.x, Color::yellow.y, Color::yellow.z, 0.5f));
                selectionBackgroundUI_[i].SetEnable(true);
                selectionBackgroundUI_[i].SetSize(Vec2u(Vec2f(config.windowSize) * 0.5f));
                selectionBackgroundUI_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));


                switch (selectionPointing_[i])
                {
                case SelectionPointing::JOIN:
                    //joinUi_[i].SetText("Press A to Join");
                    //joinUi_[i].SetEnable(true);

                    joinText_[i].SetEnable(true);
                    joinText_[i].SetSize(Vec2u(Vec2f(config.windowSize) * 0.5f));
                    joinText_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));

                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::SHIP_TYPE;
                    }

                    break;
                case SelectionPointing::SHIP_TYPE:
                    leftArrowUi_[i].SetText("<-");
                    leftArrowUi_[i].SetEnable(true);
                    rightArrowUi_[i].SetText("->");
                    rightArrowUi_[i].SetEnable(true);
                    rightArrowUi_[i].SetPosition(Vec2i((Vec2f(0.3f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)));
                    leftArrowUi_[i].SetPosition(Vec2i((Vec2f(-0.3f, 0.0f) + playerScreenOffsets[i]) * Vec2f(config.windowSize)));

                    switch (shipSkins[i])
                    {
                    case SelectedModel::ROSSO_1:
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_2;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::CORTESE_2;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso1UI_[i].SetEnable(true);
                        break;
                    case SelectedModel::ROSSO_2:
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_3;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_1;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso2UI_[i].SetEnable(true);
                        break;
                    case SelectedModel::ROSSO_3:
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_4;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_2;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso3UI_[i].SetEnable(true);
                        break;
                    case SelectedModel::ROSSO_4:
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
                        {
                            shipSkins[i] = SelectedModel::CORTESE_1;
                            isDpadLeft_[i] = true;
                        }
                        else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
                        {
                            isDpadLeft_[i] = false;
                        }
                        if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
                        {
                            shipSkins[i] = SelectedModel::ROSSO_3;
                            isDpadRight_[i] = true;
                        }
                        else if (isDpadRight_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
                        {
                            isDpadRight_[i] = false;
                        }
                        rosso4UI_[i].SetEnable(true);
                        break;
	                case SelectedModel::CORTESE_1:
	                    if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
	                    {
	                        shipSkins[i] = SelectedModel::CORTESE_2;
	                        isDpadLeft_[i] = true;
	                    }
	                    else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
	                    {
	                        isDpadLeft_[i] = false;
	                    }
	                    if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
	                    {
	                        shipSkins[i] = SelectedModel::ROSSO_4;
	                        isDpadRight_[i] = true;
	                    }
	                    else if (isDpadRight_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
	                    {
	                        isDpadRight_[i] = false;
	                    }
	                    cortese1UI_[i].SetEnable(true);
	                    break;
			        case SelectedModel::CORTESE_2:
			            if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= -0.7 && !isDpadLeft_[i])
			            {
			                shipSkins[i] = SelectedModel::ROSSO_1;
			                isDpadLeft_[i] = true;
			            }
			            else if (isDpadLeft_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= -0.7)
			            {
			                isDpadLeft_[i] = false;
			            }
			            if (inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) >= 0.7 && !isDpadRight_[i])
			            {
			                shipSkins[i] = SelectedModel::CORTESE_1;
			                isDpadRight_[i] = true;
			            }
			            else if (isDpadRight_[i] && inputlocator.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS) <= 0.7)
			            {
			                isDpadRight_[i] = false;
			            }
                        cortese2UI_[i].SetEnable(true);
			            break;
            }
                    rosso1UI_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));
                    rosso2UI_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));
                    rosso3UI_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));
                    rosso4UI_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));
                    cortese1UI_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));
                    cortese2UI_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));

                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN && i == 0)
                    {
                        menuStatus_ = MenuStatus::MENU;
                    }
                    else if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::JOIN;
                    }
                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::READY;
                    }
                    break;
                case SelectionPointing::READY:
                    //readyUi_[i].SetText("Ready");
                    //readyUi_[i].SetEnable(true);

                    readyBackground_[i].SetEnable(true);
                    readyBackground_[i].SetSize(Vec2u(Vec2f(config.windowSize) * 0.5f));
                    readyBackground_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));
                    readyText_[i].SetEnable(true);
                    readyText_[i].SetSize(Vec2u(Vec2f(config.windowSize) * 0.5f));
                    readyText_[i].SetPosition(Vec2i(playerScreenOffsets[i] * Vec2f(config.windowSize)));

                    if (inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN)
                    {
                        selectionPointing_[i] = SelectionPointing::SHIP_TYPE;
                    }
                    if (inputlocator.GetControllerButtonState(0, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN && i == 0)
                    {
                        playerCount = 0;
                        bool oneIsReady = true;
                        for (int i = 0; i < 4; i++)
                        {
                            if (selectionPointing_[i] == SelectionPointing::READY && oneIsReady)
                            {
                                oneIsReady = true;
                                playerCount++;
                            }
                            else if (selectionPointing_[i] == SelectionPointing::READY && !oneIsReady)
                            {
                                playerCount = 0;
                            } else {
                                oneIsReady = false;
                            }
                        }
                        if (playerCount > 0)
                        {
                            // menuBackGroundUI.SetEnable(false);
                            // engine_.GetComponentManagerContainer().gameManager.StartGameManager(playerCount, shipSkins);
                            // //engine_.GetComponentManagerContainer().gameManager.StartGameManager(2);
                            menuStatus_ = MenuStatus::LOADING;
                        }
                    }
                    break;
                }
            }
            break;
        case MenuStatus::LOADING:

            if (!hasStartedSceneLoading)
            {
                engine_.GetResourceManagerContainer().modelManager.LoadModel(config.dataRootPath + "models/terrain_collider_v3/terrain_collider_v3.obj");
                engine_.GetComponentManagerContainer().sceneManager.LoadScene(
                    config.dataRootPath + "scenes/LevelDesign05-05WP.aerscene");
                Camera3D* camera = GizmosLocator::get().GetCamera();
                camera->fovY = degree_t(80.0f);
                camera->nearPlane = 0.1f;
                camera->farPlane = 1'000'000.0f;
                engine_.GetCameras().SetCameras(*camera);
                hasStartedSceneLoading = true;

                // const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
                // engine_.GetComponentManagerContainer().sceneManager.LoadScene(
                //     config.dataRootPath + "scenes/LevelDesign05-04WP.aerscene");
                // Camera3D* camera = GizmosLocator::get().GetCamera();
                // camera->fovY = degree_t(80.0f);
                // camera->nearPlane = 0.1f;
                // camera->farPlane = 1'000'000.0f;
                // engine_.GetCameras().SetCameras(*camera);
            }
            engine_.GetComponentManagerContainer().playerManager.LoadShipModels();
            loadingText_.SetText("Loading " + std::to_string(int((float(engine_.GetResourceManagerContainer().modelManager.CountModelLoaded()) / float(engine_.GetResourceManagerContainer().modelManager.CountOfAllModel())) + (float(engine_.GetResourceManagerContainer().textureManager.CountTextureLoaded()) / float(engine_.GetResourceManagerContainer().textureManager.CountAllTexture())) * 100)) + "%");
        	
            loadingText_.SetEnable(true);


        	if (engine_.GetResourceManagerContainer().modelManager.CountModelNotLoaded() == 0)
            {
	            if (
                    engine_.GetResourceManagerContainer().textureManager.CountTextureNotLoaded() == 0) 
	            {
                    menuBackGroundUI.SetEnable(false);
                    engine_.GetComponentManagerContainer().gameManager.StartGameManager(playerCount, shipSkins);
                    //engine_.GetComponentManagerContainer().gameManager.StartGameManager(2);
                    playerCount = 0;
	            	menuStatus_ = MenuStatus::SLEEP;
	            }
            }

        	
            menuBackGroundUI.SetEnable(true);
            menuBackGroundUI.SetSize(config.windowSize);
            break;
        case MenuStatus::SLEEP:
            break;
        }
    }
    auto& inputlocator = sdl::InputLocator::get();

    for (int i = 0; i < kMaxPlayer_; i++)
    {
        if ((inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN ||
            inputlocator.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::DOWN) && (menuStatus_ != MenuStatus::LOADING && menuStatus_ != MenuStatus::SLEEP))
        {
            PlaySelectSound();
        }

        if(lastSelectedSkins_[i] != shipSkins[i]) {
            PlayBipSound();
        }
    }   

    if((menuStatus_ == lastMenuStatus_) && (mainMenuPointing_ != lastMenuPointing_)) 
    {
        PlayBipSound();       
    }


}

void MenuManager::Render()
{

}

void MenuManager::Destroy()
{
    
}



void MenuManager::StartMenu()
{
    menuStatus_ = MenuStatus::MENU;
    mainMenuPointing_ = MainMenuPointing::START;
    menuBackGroundUI.SetEnable(true);

    useMenu = true;

}

void MenuManager::PressStartButton()
{
    
}

void MenuManager::PressOptionsButton()
{
    
}

void MenuManager::PressHighscoreButton()
{
    
}

void MenuManager::PlayerJoin()
{
    
}

void MenuManager::BeginGame()
{
    
}

void MenuManager::SaveSoundOptions()
{
    
}

void MenuManager::PlayBipSound()
{
    ComponentManagerContainer& cContainer = engine_.GetComponentManagerContainer();
    if (cContainer.gameManager.GetGamestate() == GameState::RACING) return;
    cContainer.audioManager.Play(bipAudioEntity_);
}

void MenuManager::PlaySelectSound()
{
    ComponentManagerContainer& cContainer = engine_.GetComponentManagerContainer();
    if (cContainer.gameManager.GetGamestate() == GameState::RACING) return;
    cContainer.audioManager.Play(selectAudioEntity_);
}



}