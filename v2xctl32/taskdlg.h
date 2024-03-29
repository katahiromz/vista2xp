#ifndef TASKDLG_H_
#define TASKDLG_H_

#include <pshpack1.h>

#define TD_WARNING_ICON MAKEINTRESOURCEW (-1)
#define TD_ERROR_ICON MAKEINTRESOURCEW (-2)
#define TD_INFORMATION_ICON MAKEINTRESOURCEW (-3)
#define TD_SHIELD_ICON MAKEINTRESOURCEW (-4)

enum _TASKDIALOG_COMMON_BUTTON_FLAGS
{
    TDCBF_OK_BUTTON = 0x1,
    TDCBF_YES_BUTTON = 0x2,
    TDCBF_NO_BUTTON = 0x4,
    TDCBF_CANCEL_BUTTON = 0x8,
    TDCBF_RETRY_BUTTON = 0x10,
    TDCBF_CLOSE_BUTTON = 0x20
};
typedef int TASKDIALOG_COMMON_BUTTON_FLAGS;

typedef HRESULT (CALLBACK *PFTASKDIALOGCALLBACK)(HWND, UINT, WPARAM, LPARAM, LONG_PTR);

enum _TASKDIALOG_FLAGS {
    TDF_ENABLE_HYPERLINKS = 0x1,
    TDF_USE_HICON_MAIN = 0x2,
    TDF_USE_HICON_FOOTER = 0x4,
    TDF_ALLOW_DIALOG_CANCELLATION = 0x8,
    TDF_USE_COMMAND_LINKS = 0x10,
    TDF_USE_COMMAND_LINKS_NO_ICON = 0x20,
    TDF_EXPAND_FOOTER_AREA = 0x40,
    TDF_EXPANDED_BY_DEFAULT = 0x80,
    TDF_VERIFICATION_FLAG_CHECKED = 0x100,
    TDF_SHOW_PROGRESS_BAR = 0x0200,
    TDF_SHOW_MARQUEE_PROGRESS_BAR = 0x0400,
    TDF_CALLBACK_TIMER = 0x0800,
    TDF_POSITION_RELATIVE_TO_WINDOW = 0x1000,
    TDF_RTL_LAYOUT = 0x2000,
    TDF_NO_DEFAULT_RADIO_BUTTON = 0x4000,
    TDF_CAN_BE_MINIMIZED = 0x8000,
    TDF_NO_SET_FOREGROUND = 0x10000,
    TDF_SIZE_TO_CONTENT = 0x1000000
};
typedef int TASKDIALOG_FLAGS;

typedef enum _TASKDIALOG_MESSAGES {
    TDM_NAVIGATE_PAGE = WM_USER+101,
    TDM_CLICK_BUTTON = WM_USER+102,
    TDM_SET_MARQUEE_PROGRESS_BAR = WM_USER+103,
    TDM_SET_PROGRESS_BAR_STATE = WM_USER+104,
    TDM_SET_PROGRESS_BAR_RANGE = WM_USER+105,
    TDM_SET_PROGRESS_BAR_POS = WM_USER+106,
    TDM_SET_PROGRESS_BAR_MARQUEE = WM_USER+107,
    TDM_SET_ELEMENT_TEXT = WM_USER+108,
    TDM_CLICK_RADIO_BUTTON = WM_USER+110,
    TDM_ENABLE_BUTTON = WM_USER+111,
    TDM_ENABLE_RADIO_BUTTON = WM_USER+112,
    TDM_CLICK_VERIFICATION = WM_USER+113,
    TDM_UPDATE_ELEMENT_TEXT = WM_USER+114,
    TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE = WM_USER+115,
    TDM_UPDATE_ICON = WM_USER+116
} TASKDIALOG_MESSAGES;

typedef enum _TASKDIALOG_NOTIFICATIONS {
    TDN_CREATED = 0,
    TDN_NAVIGATED = 1,
    TDN_BUTTON_CLICKED = 2,
    TDN_HYPERLINK_CLICKED = 3,
    TDN_TIMER = 4,
    TDN_DESTROYED = 5,
    TDN_RADIO_BUTTON_CLICKED = 6,
    TDN_DIALOG_CONSTRUCTED = 7,
    TDN_VERIFICATION_CLICKED = 8,
    TDN_HELP = 9,
    TDN_EXPANDO_BUTTON_CLICKED = 10
} TASKDIALOG_NOTIFICATIONS;

typedef struct _TASKDIALOG_BUTTON {
    int nButtonID;
    PCWSTR pszButtonText;
} TASKDIALOG_BUTTON;

typedef enum _TASKDIALOG_ELEMENTS {
    TDE_CONTENT,
    TDE_EXPANDED_INFORMATION,
    TDE_FOOTER,
    TDE_MAIN_INSTRUCTION
} TASKDIALOG_ELEMENTS;

typedef enum _TASKDIALOG_ICON_ELEMENTS {
    TDIE_ICON_MAIN,
    TDIE_ICON_FOOTER
} TASKDIALOG_ICON_ELEMENTS;

typedef struct _TASKDIALOGCONFIG {
    UINT cbSize;
    HWND hwndParent;
    HINSTANCE hInstance;
    TASKDIALOG_FLAGS dwFlags;
    TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons;
    PCWSTR pszWindowTitle;
    union {
      HICON hMainIcon;
      PCWSTR pszMainIcon;
    } DUMMYUNIONNAME;
    PCWSTR pszMainInstruction;
    PCWSTR pszContent;
    UINT cButtons;
    const TASKDIALOG_BUTTON *pButtons;
    int nDefaultButton;
    UINT cRadioButtons;
    const TASKDIALOG_BUTTON *pRadioButtons;
    int nDefaultRadioButton;
    PCWSTR pszVerificationText;
    PCWSTR pszExpandedInformation;
    PCWSTR pszExpandedControlText;
    PCWSTR pszCollapsedControlText;
    union {
      HICON hFooterIcon;
      PCWSTR pszFooterIcon;
    } DUMMYUNIONNAME2;
    PCWSTR pszFooter;
    PFTASKDIALOGCALLBACK pfCallback;
    LONG_PTR lpCallbackData;
    UINT cxWidth;
} TASKDIALOGCONFIG;

#include <poppack.h>

#endif  /* ndef TASKDLG_H_ */
