#pragma once

#include "LuaParser/LuaAstNode/LuaAstNode.h"
#include "FormatElementType.h"
#include "SerializeContext.h"
#include "DiagnosisContext.h"
#include "CodeService/LanguageTranslator.h"
#include "CodeService/LuaFormatRange.h"
#include "CodeService/LinterDescription.h"


class FormatElement : public std::enable_shared_from_this<FormatElement>
{
public:
	using ChildContainer = std::vector<std::shared_ptr<FormatElement>>;
	using ChildIterator = ChildContainer::iterator;

	explicit FormatElement(TextRange range = TextRange());
	virtual ~FormatElement();

	virtual FormatElementType GetType();

	virtual void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent);

	virtual void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent);

	void Format(SerializeContext& ctx);

	void DiagnoseCodeStyle(DiagnosisContext& ctx);

	TextRange GetTextRange();

	void AddChild(std::shared_ptr<FormatElement> child);

	void AddChildren(ChildContainer& children);

	ChildContainer& GetChildren();

	bool HasValidTextRange() const;

	std::shared_ptr<FormatElement> LastValidElement() const;

	template <class T, class ... ARGS>
	void Add(ARGS ...args)
	{
		AddChild(std::make_shared<T>(std::forward<ARGS>(args)...));
	}

	template <class T, LinterDescription Description ,class ... ARGS>
	void Add(ARGS ...args)
	{
		auto child = std::make_shared<T>(std::forward<ARGS>(args)...);
		child->SetDescription(Description);
		AddChild(child);
	}

	void CopyFrom(std::shared_ptr<FormatElement> node);

	void SetDescription(LinterDescription description);

	void Reset();
protected:
	static int GetLastValidOffset(ChildIterator& it, FormatElement& parent);
	static int GetNextValidOffset(ChildIterator& it, FormatElement& parent);

	static int GetLastValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent);
	static int GetNextValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent);

	void GeneralIndentDiagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent);
	void AddTextRange(TextRange range);

	TextRange _textRange;

	LinterDescription _description;

	ChildContainer _children;
};
