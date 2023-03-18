#include "pch.h"
#include "FileMenu.h"
#include "Window.h"
#include <commdlg.h>

namespace
{
    constexpr wchar_t theScenesDir[]{ L"assets\\scenes" };

    bool GetOpenFilePath(fs::path& aPath)
    {
        TCHAR file[MAX_PATH]{};

        OPENFILENAME open{};
        open.lStructSize = sizeof(OPENFILENAME);
        open.hwndOwner = Window::Get();
        open.lpstrFilter = L"All Files (*.*)\0*.*\0Scene Files (*.scene;*.json)\0*.scene;*.json\0";
        open.nFilterIndex = 2;
        open.lpstrFile = file;
        open.nMaxFile = MAX_PATH;
        open.lpstrInitialDir = theScenesDir;
        open.Flags = OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;

        bool ok = GetOpenFileName(&open);

        if (ok)
            aPath = file;

        return ok;
    }
}

FileCommand ImGui::FileMenu(fs::path& aPath)
{
    FileCommand cmd = FileCommand::None;

    /*
    * Menu items and submenus
    */

    if (MenuItem("New Scene"))
        cmd = FileCommand::NewScene;

    /*if (BeginMenu("Open Scene"))
    {
        for (const auto& entry : fs::directory_iterator{ "assets/scenes" })
        {
            if (!entry.is_regular_file())
                continue;
            if (entry.path().extension() != ".json")
                continue;
            if (MenuItem(entry.path().stem().string().c_str()))
            {
                
                aPath = entry.path();
                cmd = FileCommand::OpenScene;
            }
        }
        EndMenu();
    }*/

    if (MenuItem("Open Scene"))
    {
        if (GetOpenFilePath(aPath))
            cmd = FileCommand::OpenScene;
    }

    Separator();

    if (MenuItem("Save"))
        cmd = FileCommand::Save;

    if (MenuItem("Save As..."))
        cmd = FileCommand::SaveAs;

    Separator();

    if (MenuItem("Exit"))
        cmd = FileCommand::Exit;

    /*
    * Popups
    */

    if (cmd == FileCommand::SaveAs)
    {
        OpenPopup("Save As...");
        cmd = FileCommand::None;
    }

    SetNextWindowPos(GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f, 0.5f });

    if (BeginPopupModal("Save As...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::string name{};

        InputText("Name", &name);

        if (Button("Save"))
        {
            Debug::Println(name);
            CloseCurrentPopup();
        }

        SameLine();

        if (Button("Cancel"))
            CloseCurrentPopup();

        EndPopup();
    }

    return cmd;
}
