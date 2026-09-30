SET(SOURCES components/ui_comp_flowratevar.c
    components/ui_comp.c
    screens/ui_Main_Screen.c
    ui_theme_manager.c
    ui_themes.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_icn_battery_png.c
    images/ui_img_img_switch_bg_png.c
    images/ui_img_cable_png.c
    images/ui_img_irveso_logo_png.c
    images/ui_img_159246385.c
    images/ui_img_1643428095.c
    images/ui_img_890971674.c
    fonts/ui_font_Big_Number.c
    fonts/ui_font_Bold_Font.c
    fonts/ui_font_Font1.c)

add_library(ui ${SOURCES})
