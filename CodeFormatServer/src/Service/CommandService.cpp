#include "CommandService.h"
#include "FormatService.h"
#include "Util/format.h"
#include "Util/Url.h"
#include "LanguageServer.h"
#include "ConfigService.h"

CommandService::CommandService(LanguageServer *owner)
        : Service(owner) {
}

void CommandService::Start() {
}

bool CommandService::Initialize() {
    CommandProtocol("emmylua.reformat.me", &CommandService::Reformat);
    CommandProtocol("emmylua.spell.correct", &CommandService::SpellCorrect);
    return true;
}

std::vector<std::string> CommandService::GetCommands() {
    std::vector<std::string> results;

    for (auto it: _handles) {
        results.push_back(it.first);
    }

    return results;
}

std::string CommandService::GetCommand(Command command) {
    switch (command) {
        case Command::Reformat:
            return "emmylua.reformat.me";
        case Command::SpellCorrect:
            return "emmylua.spell.correct";
        case Command::SpellAddDict:
            return "emmylua.spell.addDict";
    }

    return "";
}

void CommandService::Dispatch(std::string_view command, std::shared_ptr<lsp::ExecuteCommandParams> params) {
    std::string cmd(command);
    auto it = _handles.find(cmd);
    if (it != _handles.end()) {
        it->second(params);
    }
}

void CommandService::Reformat(std::shared_ptr<lsp::ExecuteCommandParams> params) {
    if (params->arguments.size() < 2) {
        return;
    }

    std::string uri = params->arguments[0];
    lsp::Range range;

    range.Deserialize(params->arguments[1]);

    auto applyParams = std::make_shared<lsp::ApplyWorkspaceEditParams>();


    auto &vfs = _owner->GetVFS();
    auto vFile = vfs.GetVirtualFile(uri);
    if (vFile.IsNull()) {
        return;
    }

    auto opSyntaxTree = vFile.GetSyntaxTree(vfs);
    if (!opSyntaxTree.has_value()) {
        return;
    }

    auto &syntaxTree = opSyntaxTree.value();

    if (syntaxTree.HasError()) {
        return;
    }

    LuaStyle &luaStyle = _owner->GetService<ConfigService>()->GetLuaStyle(uri);

    FormatRange formatRange;
    formatRange.StartLine = range.start.line;
    formatRange.EndLine = range.end.line;

    auto it = applyParams->edit.changes.emplace(uri, std::vector<lsp::TextEdit>());
    auto &change = it.first->second;
    auto &edit = change.emplace_back();

    edit.newText = _owner->GetService<FormatService>()->RangeFormat(syntaxTree, luaStyle, formatRange);

    edit.range = lsp::Range(
            lsp::Position(formatRange.StartLine, formatRange.StartCol),
            lsp::Position(formatRange.EndLine + 1, formatRange.EndCol)
    );

    _owner-> SendRequest("workspace/applyEdit", applyParams);
}

void CommandService::SpellCorrect(std::shared_ptr<lsp::ExecuteCommandParams> params) {
    if (params->arguments.size() < 3) {
        return;
    }

    std::string uri = params->arguments[0];
    lsp::Range range;

    range.Deserialize(params->arguments[1]);

    std::string newText = params->arguments[2];

    auto applyParams = std::make_shared<lsp::ApplyWorkspaceEditParams>();
    auto it = applyParams->edit.changes.emplace(uri, std::vector<lsp::TextEdit>());
    auto &change = it.first->second;

    auto &edit = change.emplace_back();

    edit.newText = newText;

    edit.range = range;

    _owner-> SendRequest("workspace/applyEdit", applyParams);
}

