#include "pch.h"
#include "FileMenu.h"
#include "Window.h"

namespace
{
    constexpr wchar_t theFilter[]{ L"Scene Files (*.scene;*.json)\0*.scene;*.json\0All Files (*.*)\0*.*\0" };
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

FileCommand ImGui::FileMenuShortcut()
{
    if (IsKeyDown(ImGuiKey_LeftCtrl))
    {
        if (IsKeyDown(ImGuiKey_LeftShift))
        {
            if (IsKeyPressed(ImGuiKey_S))
                return FileCommand::SaveAs;
        }
        if (IsKeyPressed(ImGuiKey_N))
            return FileCommand::NewScene;
        if (IsKeyPressed(ImGuiKey_O))
            return FileCommand::OpenScene;
        if (IsKeyPressed(ImGuiKey_S))
           return FileCommand::Save;
    }
    return FileCommand::None;
}

FileCommand ImGui::FileMenu(fs::path& aPath)
{
    FileCommand cmd = FileCommand::None;

    if (MenuItem("New Scene", "Ctrl+N"))
        cmd = FileCommand::NewScene;

    if (MenuItem("Open Scene", "Ctrl+O"))
    {
        if (GetOpenPath(aPath))
            cmd = FileCommand::OpenScene;
    }

    Separator();

    if (MenuItem("Save", "Ctrl+S"))
    {
        if (fs::exists(aPath))
        {
            cmd = FileCommand::Save;
        }
        else
        {
            if (GetSavePath(aPath))
                cmd = FileCommand::SaveAs;
        }
    }

    if (MenuItem("Save As...", "Ctrl+Shift+S"))
    {
        if (GetSavePath(aPath))
            cmd = FileCommand::SaveAs;
    }

    Separator();

    if (MenuItem("Exit"))
        cmd = FileCommand::Exit;

    return cmd;
}
