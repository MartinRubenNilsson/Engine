#include "pch.h"
#include "Menus.h"
#include "Window.h"

#define FILTER(aDisplayStr, aPatternStr) L ## aDisplayStr " (" aPatternStr ")\0" aPatternStr "\0"

namespace
{
    constexpr wchar_t theFilter[]
    {
        FILTER("Scene Files", "*.scene;*.json")
        FILTER("All Files", "*.*")
    };
   
    constexpr wchar_t theInitialDir[]{ L"assets\\scenes" };

    bool GetOpenPath(fs::path& aPath)
    {
        TCHAR file[MAX_PATH]{};

        OPENFILENAME name{};
        name.lStructSize = sizeof(OPENFILENAME);
        name.hwndOwner = Window::Get();
        name.lpstrFilter = theFilter;
        name.nFilterIndex = 1;
        name.lpstrFile = file;
        name.nMaxFile = MAX_PATH;
        name.lpstrInitialDir = theInitialDir;
        name.Flags = OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;

        bool ok = GetOpenFileName(&name);

        if (ok)
            aPath = file;

        return ok;
    }

    bool GetSavePath(fs::path& aPath)
    {
        TCHAR file[MAX_PATH]{};

        OPENFILENAME name{};
        name.lStructSize = sizeof(OPENFILENAME);
        name.hwndOwner = Window::Get();
        name.lpstrFilter = theFilter;
        name.nFilterIndex = 1;
        name.lpstrFile = file;
        name.nMaxFile = MAX_PATH;
        name.lpstrInitialDir = theInitialDir;
        name.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

        bool ok = GetSaveFileName(&name);

        if (ok)
            aPath = file;

        return ok;
    }
}

/*
* namespace ImGui
*/

void ImGui::Shortcut(MenuCommand& cmd)
{
    if (IsKeyDown(ImGuiMod_Ctrl))
    {
        if (IsKeyPressed(ImGuiKey_N))
            cmd = MenuCommand::NewScene;
        if (IsKeyPressed(ImGuiKey_O))
            cmd = MenuCommand::OpenScene;
        if (IsKeyDown(ImGuiMod_Shift))
        {
            if (IsKeyPressed(ImGuiKey_S))
                cmd = MenuCommand::SaveAs;
        }
        else
        {
            if (IsKeyPressed(ImGuiKey_S))
                cmd = MenuCommand::Save;
        }
    }

    /*
    * If a file open/save dialog is opened, ImGui's input keys will not be cleared
    * and the shortcut triggers repetitively. Therefore we clear manually. 
    */

    if (cmd != MenuCommand::None)
        GetIO().ClearInputKeys();
}

void ImGui::FileMenu(MenuCommand& cmd)
{
    if (MenuItem("New Scene", "Ctrl+N"))
        cmd = MenuCommand::NewScene;
    if (MenuItem("Open Scene", "Ctrl+O"))
        cmd = MenuCommand::OpenScene;

    Separator();

    if (MenuItem("Save", "Ctrl+S"))
        cmd = MenuCommand::Save;
    if (MenuItem("Save As...", "Ctrl+Shift+S"))
        cmd = MenuCommand::SaveAs;

    Separator();

    if (MenuItem("Exit"))
        cmd = MenuCommand::Exit;
}

void ImGui::MainMenu(MenuCommand& cmd)
{
    if (BeginMenu("File"))
    {
        FileMenu(cmd);
        EndMenu();
    }
}

void ImGui::GetPath(MenuCommand& cmd, fs::path& aPath)
{
    bool ok = true;

    switch (cmd)
    {
    case MenuCommand::OpenScene:
        ok = GetOpenPath(aPath);
        break;
    case MenuCommand::Save:
        if (fs::exists(aPath))
            break;
        [[fallthrough]];
    case MenuCommand::NewScene:
        [[fallthrough]];
    case MenuCommand::SaveAs:
        ok = GetSavePath(aPath);
        break;
    }

    if (!ok)
        cmd = MenuCommand::None;
}
