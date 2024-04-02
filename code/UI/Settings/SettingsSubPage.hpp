#ifndef _COSMOS_SETTINGSSUBPAGE_
#define _COSMOS_SETTINGSSUBPAGE_

#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Ctrl/PushButton.hpp>
#include <game/UI/Page/Menu/Menu.hpp>
#include <game/UI/Page/Menu/VSSettings.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuText.hpp>
#include <UI/Settings/SettingsBasePage.hpp>
#include <Settings/UserData.hpp>
#include <Controller/MiscController.hpp>
#include <main.hpp>

#define SUBPAGECOUNT 2

extern u32 VSSettings_sInstance;


namespace CosmosUI
{
    typedef struct SettingPageOption{
        u8 optionCount;
        bool isBool;
    } SettingPageOption;

    typedef struct SettingPageDefinition{
        u8 settingCount;
        SettingPageOption settings[8];
    } SettingPageDefinition;

    void Page_Refocus(Pages::Menu * page);

    class TextUpDownValueControlPlus : public TextUpDownValueControl
    {
        public:
        TextUpDownValueControlPlus(){};
        void SetMessage(u32 id) override;
        u32 id;
    };
    class SettingSubPage : public Pages::Menu
    {
        public:
        SettingSubPage::SettingSubPage(SettingPageDefinition * definition, u32 pageId, u32 index);
        void EmptySelect(UpDownControl *control, u32 u);
        ~SettingSubPage() override;
        void OnInit() override;
        void OnActivate() override;
        int GetRuntimeTypeInfo() const override;
        int GetActivePlayerBitfield() const override;
        int GetPlayerBitfield() const override;
        ManipulatorManager *GetManipulatorManager() override;
        UIControl *CreateExternalControl(u32 id) override;
        void OnExternalButtonSelect(PushButton  *button, u32 param_2) override;
        UIControl *CreateControl(u32 id) override;
        void SetButtonHandlers(PushButton *pushButton) override;
        void OnUpdate() override;
        u32 pageIndex;
        u32 scrollersCount;

        SettingPageDefinition pageDefinition;

        SettingsBasePage * basePage;
        UpDownControl * upDownControls;


        private:
        static void SaveSettings();
        void OnBackPress(u32 slotId);
        void OnSaveButtonClick(PushButton * button, u32 hudSlotId);
        void OnUpDownClick(UpDownControl *upDownControl, u32 hudSlotId);
        void OnUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId);
        void OnMainUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId);
        void OnMainUpDownDeselect(UpDownControl *upDownControl, u32 hudSlotId);
        void OnMainUpDownChange(UpDownControl *upDownControl, u32 hudSlotId, u32 optionId);


        void OnTextChange(TextUpDownValueControl::TextControl *text, u32 optionId);
        void SaveAndQuit(PushButton *button);

        u32 mainControlId;

        TextUpDownValueControlPlus * textUpDownPlus;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onUpDownClickHandler; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onUpDownSelectHandler; 
        
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onMainUpDownSelectHandler; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onMainUpDownDeselectHandler; 
        PtmfHolder_3A<Page, void, UpDownControl*, u32, u32> onMainUpDownChangeHandler;

        PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl*, u32> onTextChangeHandler; 
        u8 optionsPerScroller[8];
        PtmfHolder_2A<Menu, void, PushButton *, u32> onRightButtonClickHandler;
        PtmfHolder_2A<Menu, void, PushButton *, u32> onLeftButtonClickHandler;

        UpDownControl * menuUpDownControls;
        TextUpDownValueControlPlus * menuTextUpDownPlus;
        bool disableControl;

    };


} // namespace CosmosUI


#endif