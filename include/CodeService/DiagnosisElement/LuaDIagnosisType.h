#pragma once

enum class LuaDiagnosisType
{
	DiagnosisElement,
	MaxLineLengthDiagnosis,
	IndentDiagnosis,
	LineLayoutDiagnosis,
	SpaceDiagnosis,
	EqSymbolAlignDiagnosis,
	// Œ¥ µœ÷
	NameStyleDiagnosis,
	KeepTrailingEmptyLine,
};