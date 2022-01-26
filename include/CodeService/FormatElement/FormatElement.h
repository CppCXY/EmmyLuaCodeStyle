#pragma once

#include "LuaParser/LuaAstNode/LuaAstNode.h"
#include "FormatElementType.h"
#include "SerializeContext.h"
#include "DiagnosisContext.h"
#include "CodeService/LanguageTranslator.h"
#include "CodeService/LuaFormatRange.h"


class FormatElement : public std::enable_shared_from_this<FormatElement>
{
public:
	using ChildContainer = std::vector<std::shared_ptr<FormatElement>>;
	using ChildIterator = ChildContainer::iterator;

	explicit FormatElement(TextRange range = TextRange());
	virtual ~FormatElement();

	virtual FormatElementType GetType();

	virtual void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent);

	virtual void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent);

	void Format(SerializeContext& ctx);

	void DiagnosisCodeStyle(DiagnosisContext& ctx);

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

	void CopyFrom(std::shared_ptr<FormatElement> node);

protected:
	static int GetLastValidOffset(ChildIterator& it, FormatElement& parent);
	static int GetNextValidOffset(ChildIterator& it, FormatElement& parent);

	static int GetLastValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent);
	static int GetNextValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent);

	void AddTextRange(TextRange range);

	TextRange _textRange;

	ChildContainer _children;
};
