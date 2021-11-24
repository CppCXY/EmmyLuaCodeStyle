#pragma once

#include "LuaParser/LuaAstNode.h"
#include "FormatElementType.h"
#include "FormatContext.h"
#include "DiagnosisContext.h"
#include "CodeService/LanguageTranslator.h"
#include "CodeService/LuaFormatRange.h"

class FormatElement
{
public:
	explicit FormatElement(TextRange range = TextRange());
	virtual ~FormatElement();

	virtual FormatElementType GetType();
	/*
	 * 智能指针在多态方面支持不够方便
	 */
	virtual void Serialize(FormatContext& ctx, int position, FormatElement& parent);

	/*
	 * 智能指针在多态方面支持不够方便
	 */
	virtual void Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent);

	void Format(FormatContext& ctx);

	void DiagnosisCodeStyle(DiagnosisContext& ctx);

	TextRange GetTextRange();

	void AddChild(std::shared_ptr<FormatElement> child);

	void AddChildren(std::vector<std::shared_ptr<FormatElement>>& children);

	std::vector<std::shared_ptr<FormatElement>>& GetChildren();

	bool HasValidTextRange() const;

	std::shared_ptr<FormatElement> LastValidElement() const;

	template <class T, class ... ARGS>
	void Add(ARGS ...args)
	{
		AddChild(std::make_shared<T>(std::forward<ARGS>(args)...));
	}


protected:
	void addTextRange(TextRange range);

	int getLastValidLine(FormatContext& ctx, int position, FormatElement& parent);
	int getNextValidLine(FormatContext& ctx, int position, FormatElement& parent);

	int getLastValidOffset(int position, FormatElement& parent);
	int getNextValidOffset(int position, FormatElement& parent);

	TextRange _textRange;

	std::vector<std::shared_ptr<FormatElement>> _children;
};
