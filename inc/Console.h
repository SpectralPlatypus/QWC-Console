#pragma once

#include "imgui.h"
#include <stdio.h>
#include <ctype.h>
#include <memory>
#include <imgui_internal.h>
#include "CLIData.h"

class ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;
    CLI* Cli;

public:
    ExampleAppConsole() :
        Cli(reinterpret_cast<CLI*>(CLISingleton))
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        AddLog("QWC Console v1.0!");
        AddLog("Toggle with ` key");
    }

    ~ExampleAppConsole()
    {
        Cli = nullptr;
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            std::free(History[i]);
    }

    void Draw(const char* title, bool* p_open)
    {
        ImGui::GetIO().WantCaptureKeyboard = true;
        ImGui::SetNextWindowPos(ImVec2{ 10, 10, });
        ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }
        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
        // You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
        // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
        //     ImGuiListClipper clipper(Items.Size);
        //     while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // However take note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
        // If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // A better implementation may store a type per-item. For the sample let's just parse the text.
            if (strstr(item, "[error]")) col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
            else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
            else if (strstr(item, "<")) col = ImColor(0.78f, 1.0f, 0.58f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::TextUnformatted(item);
            ImGui::PopStyleColor();
        }

        if (ScrollToBottom)
            ImGui::SetScrollHere();
        ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        ImGui::SetKeyboardFocusHere();

        auto callback = [](ImGuiTextEditCallbackData* data) -> int
        {
            ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
            return console->TextEditCallback(data);
        };

        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, callback, this))
        {
            char* input_end = InputBuf + strlen(InputBuf);
            while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
            if (InputBuf[0])
                ExecCommand(InputBuf);
            strcpy_s(InputBuf, "");
        }

        // Demonstrate keeping auto focus on the input box
        if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

private:
    void ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
        ScrollToBottom = true;
    }

    void AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
    {
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(_strdup(buf));
        ScrollToBottom = true;
    }

    void ExecCommand(const char* command_line)
    {

        if (!Cli->IsValidCommand(command_line))
        {
            AddLog("[error] Invalid command: '%s'\n", command_line);
            return;
        }

        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (_stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(_strdup(command_line));

        // Process command
        if (_stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }

        if (!Cli->ExecCmd(command_line))
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }
    }

    int     TextEditCallback(ImGuiTextEditCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    return 0;
                word_start--;
            }


            const char* firstWordEnd = strchr(word_start, '.');
            char firstWordBuf[64] = { 0 };
            ImVector<const char*> candidates;
            if (firstWordEnd)
            {
                auto* ptr = Cli->commandTable->head;
                while (ptr)
                {
                    if (_strnicmp(ptr->cmdName, word_start, max((int)strlen(ptr->cmdName), firstWordEnd - word_start)) == 0)
                        break;
                    ptr = ptr->next;
                }
                ++firstWordEnd;
                if (ptr)
                {
                    strncpy_s(firstWordBuf, word_start, firstWordEnd - word_start);
                }

                auto* sptr = ptr ? ptr->head : nullptr;
                while (sptr)
                {
                    if (_strnicmp(sptr->cmdName, firstWordEnd, (int)(word_end - firstWordEnd)) == 0)
                    {
                        candidates.push_back(sptr->cmdName);
                    }
                    sptr = sptr->next;
                }
            }
            else
            {
                firstWordEnd = word_start;
                auto* ptr = Cli->commandTable->head;
                while (ptr)
                {
                    if (_strnicmp(ptr->cmdName, word_start, (int)(word_end - word_start)) == 0)
                        candidates.push_back(ptr->cmdName);
                    ptr = ptr->next;
                }
            }

            if (candidates.Size == 0)
            {
                // No match
                AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
            }
            else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                data->DeleteChars((int)(firstWordEnd - data->Buf), (int)(word_end - firstWordEnd));
                data->InsertChars(data->CursorPos, candidates[0]);
                if (firstWordEnd == word_start)
                    data->InsertChars(data->CursorPos, ".");
                else
                {
                    firstWordBuf[strlen(firstWordBuf) - 1] = '\0';
                    auto* pptr = Cli->GetTopCmd(firstWordBuf)->GetSubCmd(candidates[0])->args;
                    char outputBuf[256] = { 0 };
                    char* target = &outputBuf[0];
                    target += sprintf_s(target, 256, "%s ", word_start);
                    while (pptr)
                    {
                        target += sprintf_s(target, 256 - (target - outputBuf), "<%s:%s> ", pptr->argName, pptr->GetType());
                        pptr = pptr->next;
                    }
                    AddLog("Usage: %s\n", outputBuf);
                    data->InsertChars(data->CursorPos, " ");
                }
            }
            else
            {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                int match_len = (int)(word_end - firstWordEnd);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(firstWordEnd - data->Buf), (int)(word_end - firstWordEnd));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("- %s%s\n", firstWordBuf, candidates[i]);

            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos)
            {
                data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
                data->BufDirty = true;
            }
        }
        }
        return 0;
    }
};