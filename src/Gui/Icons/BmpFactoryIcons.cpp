
void RegisterIcons()
{
    Gui::BitmapFactoryInst& rclBmpFactory = Gui::BitmapFactory();
#if QT_VERSION < 0x040200
    rclBmpFactory.addXPM("document-new", document_new);
    rclBmpFactory.addXPM("document-open", document_open);
    rclBmpFactory.addXPM("document-save", document_save);
    rclBmpFactory.addXPM("document-print", document_print);
    rclBmpFactory.addXPM("edit-undo", edit_undo);
    rclBmpFactory.addXPM("edit-redo", edit_redo);
    rclBmpFactory.addXPM("edit-cut", edit_cut);
    rclBmpFactory.addXPM("edit-copy", edit_copy);
    rclBmpFactory.addXPM("edit-paste", edit_paste);

    rclBmpFactory.addXPM("view-refresh", view_refresh);

    rclBmpFactory.addXPM("view-axometric", view_axometric);
    rclBmpFactory.addXPM("view-rear", view_rear);
    rclBmpFactory.addXPM("view-bottom", view_bottom);
    rclBmpFactory.addXPM("view-front", view_front);
    rclBmpFactory.addXPM("view-left", view_left);
    rclBmpFactory.addXPM("view-right", view_right);
    rclBmpFactory.addXPM("view-top", view_top);
    rclBmpFactory.addXPM("view-zoom-all", view_zoom_all);
#endif
    rclBmpFactory.addXPM("Feature", (const char**)Feature_xpm);
    rclBmpFactory.addXPM("Document", (const char**)Document_xpm);
    rclBmpFactory.addXPM("px", px);
    rclBmpFactory.addXPM("FCIcon", FCIcon);
    rclBmpFactory.addXPM("Function", Function);
    rclBmpFactory.addXPM("Param_Bool", Param_Bool);
    rclBmpFactory.addXPM("Param_Float", Param_Float);
    rclBmpFactory.addXPM("Param_Int", Param_Int);
    rclBmpFactory.addXPM("Param_UInt", Param_UInt);
    rclBmpFactory.addXPM("Param_Text", Param_Text);
    rclBmpFactory.addXPM("Param_XML", Param_XML);
    rclBmpFactory.addXPM("Part_Box", Part_Box);
    rclBmpFactory.addXPM("PrefTree_GroupClosed", PrefTree_GroupClosed);
    rclBmpFactory.addXPM("PrefTree_GroupOpen", PrefTree_GroupOpen);
    rclBmpFactory.addXPM("RawTree_Attr", RawTree_Attr);
    rclBmpFactory.addXPM("RawTree_LabelClosed", RawTree_LabelClosed);
    rclBmpFactory.addXPM("RawTree_LabelOpen", RawTree_LabelOpen);
    rclBmpFactory.addXPM("Std_Tool1", Std_Tool1);
    rclBmpFactory.addXPM("Std_Tool10", Std_Tool10);
    rclBmpFactory.addXPM("Std_Tool11", Std_Tool11);
    rclBmpFactory.addXPM("Std_Tool12", Std_Tool12);
    rclBmpFactory.addXPM("Std_Tool2", Std_Tool2);
    rclBmpFactory.addXPM("Std_Tool3", Std_Tool3);
    rclBmpFactory.addXPM("Std_Tool4", Std_Tool4);
    rclBmpFactory.addXPM("Std_Tool5", Std_Tool5);
    rclBmpFactory.addXPM("Std_Tool6", Std_Tool6);
    rclBmpFactory.addXPM("Std_Tool7", Std_Tool7);
    rclBmpFactory.addXPM("Std_Tool8", Std_Tool8);
    rclBmpFactory.addXPM("Std_Tool9", Std_Tool9);
    rclBmpFactory.addXPM("Std_WindowCascade", Std_WindowCascade);
    rclBmpFactory.addXPM("Std_WindowNew", Std_WindowNew);
    rclBmpFactory.addXPM("Std_WindowNext", Std_WindowNext);
    rclBmpFactory.addXPM("Std_WindowPrev", Std_WindowPrev);
    rclBmpFactory.addXPM("Std_WindowTileHor", Std_WindowTileHor);
    rclBmpFactory.addXPM("Std_WindowTileVer", Std_WindowTileVer);
    rclBmpFactory.addXPM("Test1", Test1);
    rclBmpFactory.addXPM("Test2", Test2);
    rclBmpFactory.addXPM("TileHorizontal", TileHorizontal);
    rclBmpFactory.addXPM("TileVertical", TileVertical);
    rclBmpFactory.addXPM("TopLevel", TopLevel);
    rclBmpFactory.addXPM("view_comp_off", view_comp_off);
    rclBmpFactory.addXPM("view_comp_on", view_comp_on);
    rclBmpFactory.addXPM("view_fitarea", view_fitarea);
    rclBmpFactory.addXPM("view_glpan", view_glpan);
    rclBmpFactory.addXPM("view_pan", view_pan);
    rclBmpFactory.addXPM("view_reset", view_reset);
    rclBmpFactory.addXPM("view_rotate", view_rotate);
    rclBmpFactory.addXPM("view_zoom", view_zoom);
    rclBmpFactory.addXPM("WhatsThis", WhatsThis);
    rclBmpFactory.addXPM("back_pixmap", back_pixmap);
    rclBmpFactory.addXPM("forward_pixmap", forward_pixmap);
    rclBmpFactory.addXPM("home_pixmap", home_pixmap);
    rclBmpFactory.addXPM("help", help);
    rclBmpFactory.addXPM("helpopen", helpopen);
    rclBmpFactory.addXPM("colors", colors);
    rclBmpFactory.addXPM("python_small", python_small);
    rclBmpFactory.addXPM("python", python);
    rclBmpFactory.addXPM("MacroEditor", MacroEditor);
    rclBmpFactory.addXPM("PolygonPick", PolygonPick);
    rclBmpFactory.addXPM("perspective_xpm", perspective_xpm);
    rclBmpFactory.addXPM("ortho_xpm", ortho_xpm);
    rclBmpFactory.addXPM("button_up", button_up);
    rclBmpFactory.addXPM("button_down", button_down);
    rclBmpFactory.addXPM("button_left", button_left);
    rclBmpFactory.addXPM("button_right", button_right);
}
