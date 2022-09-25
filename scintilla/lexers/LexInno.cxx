// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Inno Setup.

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "StringUtils.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

enum {
	InnoLineStateLineComment = 1,
	InnoLineStateEmptyLine = 1 << 1,
	InnoLineStateSectionHeader = 1 << 2,
	InnoLineStateLineContinuation = 1 << 3,
	InnoLineStateDefineLine = 1 << 4,
	InnoLineStatePreprocessor = 1 << 5,
	InnoLineStateCodeSection = 1 << 6,
};

//KeywordIndex++Autogenerated -- start of section automatically generated
enum {
	KeywordIndex_Keyword = 3,
	KeywordIndex_Type = 5,
	KeywordIndex_PredefinedVariable = 6,
	KeywordIndex_PascalKeyword = 8,
	KeywordIndex_PascalType = 9,
	KeywordIndex_PascalConstant = 11,
};
//KeywordIndex--Autogenerated -- end of section automatically generated

enum class InnoParameterState {
	None,
	Key,
	Assign,
	Value,
};

enum class PreprocessorKind {
	None,
	Init,
	Pragma,
	Include,
	Message,
};

constexpr bool IsExpansionStartChar(int ch) noexcept {
	return IsLowerCase(ch) || ch == '%' || ch == '#' || ch == '\\';
}

void ColouriseInnoDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int visibleChars = 0;
	int chPrevNonWhite = 0;
	int chBeforeIdentifier = 0;
	int outerState = SCE_INNO_DEFAULT;
	InnoParameterState paramState = InnoParameterState::None;
	PreprocessorKind ppKind = PreprocessorKind::None;
	bool checkParameter = false;
	int lineState = 0;
	int lineStatePrev = 0;
	int expansionLevel = 0;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		lineStatePrev = styler.GetLineState(sc.currentLine - 1);
		lineState = lineStatePrev & InnoLineStateCodeSection;
		if (lineStatePrev & InnoLineStateLineContinuation) {
			lineState |= lineStatePrev & InnoLineStatePreprocessor;
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_INNO_OPERATOR:
			sc.SetState(SCE_INNO_DEFAULT);
			break;

		case SCE_INNO_SECTION:
			if (sc.atLineStart) {
				sc.SetState(SCE_INNO_DEFAULT);
			} else if (sc.ch == ']') {
				char s[8];
				sc.GetCurrentLowered(s, sizeof(s));
				if (StrEqual(s + 1, "code")) {
					lineState |= InnoLineStateCodeSection;
				}
			}
			break;

		case SCE_INNO_STRING_DQ:
			if (sc.atLineStart) {
				sc.SetState(SCE_INNO_DEFAULT);
			} else if (ppKind == PreprocessorKind::Include) {
				if (sc.ch == '\"') {
					ppKind = PreprocessorKind::None;
					sc.ForwardSetState(SCE_INNO_DEFAULT);
				}
			} else if (sc.ch == '\"' || sc.ch == '{') {
				if (sc.ch == sc.chNext) {
					sc.Forward();
				} else if (sc.ch == '\"') {
					sc.ForwardSetState(SCE_INNO_DEFAULT);
				} else if (IsExpansionStartChar(sc.chNext)) {
					++expansionLevel;
					outerState = SCE_INNO_STRING_DQ;
					sc.SetState(SCE_INNO_INLINE_EXPANSION);
				}
			} else if (sc.ch == '%') {
				if (sc.chNext == '%') {
					sc.Forward();
				} else if (sc.chNext == 'n' || IsADigit(sc.chNext)) {
					outerState = SCE_INNO_STRING_DQ;
					sc.SetState(SCE_INNO_PLACEHOLDER);
					sc.Forward();
				}
			}
			break;

		case SCE_INNO_PLACEHOLDER:
			sc.SetState(outerState);
			continue;

		case SCE_INNO_PARAMETER:
			if (sc.ch == '.' && IsIdentifierStart(sc.chNext)) {
				sc.SetState(SCE_INNO_OPERATOR);
				sc.ForwardSetState(SCE_INNO_PARAMETER);
			} else if (sc.ch == '=' || sc.ch == ':' || IsASpaceOrTab(sc.ch)) {
				if (UnsafeLower(chBeforeIdentifier) == 'c') {
					char s[12];
					sc.GetCurrentLowered(s, sizeof(s));
					if (StrEqualsAny(s, "check", "components")) {
						checkParameter = true;
					}
				}
				if (IsASpaceOrTab(sc.ch)) {
					paramState = InnoParameterState::Assign;
					sc.SetState(SCE_INNO_DEFAULT);
				} else {
					paramState = InnoParameterState::Value;
					sc.SetState(SCE_INNO_OPERATOR);
					sc.ForwardSetState(SCE_INNO_DEFAULT);
				}
			} else if (!IsIdentifierChar(sc.ch)) {
				paramState = InnoParameterState::Value;
				sc.ChangeState(SCE_INNO_DEFAULT);
			}
			break;

		case SCE_INNO_INLINE_EXPANSION:
			if (sc.atLineStart) {
				sc.SetState(SCE_INNO_DEFAULT);
				break;
			}
			switch (sc.ch) {
			case '{':
				++expansionLevel;
				break;

			case '}':
				--expansionLevel;
				if (expansionLevel == 0) {
					sc.ForwardSetState(outerState);
					continue;
				}
				break;

			case '\"':
				if (outerState == SCE_INNO_STRING_DQ) {
					expansionLevel = 0;
					sc.ChangeState(SCE_INNO_STRING_DQ);
					continue;
				}
				break;

			case '\'':
				if (outerState == SCE_INNO_STRING_SQ) {
					expansionLevel = 0;
					sc.ChangeState(SCE_INNO_STRING_SQ);
					continue;
				}
				break;
			}
			break;

		case SCE_INNO_PREPROCESSOR:
		case SCE_INNO_NUMBER:
		case SCE_INNO_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				if (sc.state != SCE_INNO_NUMBER) {
					char s[128];
					sc.GetCurrentLowered(s, sizeof(s));
					if (ppKind == PreprocessorKind::Init) {
						if (sc.state == SCE_INNO_IDENTIFIER) {
							sc.ChangeState((outerState == SCE_INNO_DEFAULT) ? SCE_INNO_PREPROCESSOR : SCE_INNO_PREPROCESSOR_WORD);
						}
						if (sc.LengthCurrent() > 1) {
							if (outerState != SCE_INNO_DEFAULT) {
								sc.SetState(outerState);
								outerState = SCE_INNO_DEFAULT;
								ppKind = PreprocessorKind::None;
								continue;
							}
							const char *p = s;
							if (*p == '#')  {
								++p;
							}
							if (StrEqual(p, "include")) {
								ppKind = PreprocessorKind::Include;
							} else if (StrEqual(p, "error")) {
								ppKind = PreprocessorKind::Message;
							} else if (StrEqual(p, "pragma")) {
								ppKind = PreprocessorKind::Pragma;
							} else {
								ppKind = PreprocessorKind::None;
								if (StrEqual(p, "define")) {
									lineState |= InnoLineStateDefineLine;
								}
							}
						} else if (!IsASpaceOrTab(sc.ch)) {
							ppKind = PreprocessorKind::None;
							if (outerState != SCE_INNO_DEFAULT) {
								sc.SetState(outerState);
								outerState = SCE_INNO_DEFAULT;
								continue;
							}
						}
					} else if (ppKind == PreprocessorKind::Pragma) {
						// TODO: track whether C-style string literals (with escape sequences) is enabled or not.
						ppKind = PreprocessorKind::None;
						sc.ChangeState(SCE_INNO_PREPROCESSOR_WORD);
					} else if (lineState & InnoLineStateCodeSection) {
						if (keywordLists[KeywordIndex_PascalKeyword]->InList(s)) {
							sc.ChangeState(SCE_INNO_PASCAL_KEYWORD);
						} else if (chBeforeIdentifier == ':' || keywordLists[KeywordIndex_PascalType]->InList(s)) {
							sc.ChangeState(SCE_INNO_PASCAL_TYPE);
						} else if (keywordLists[KeywordIndex_PascalConstant]->InList(s)) {
							sc.ChangeState(SCE_INNO_CONSTANT);
						}
					} else {
						if (keywordLists[KeywordIndex_Keyword]->InList(s)) {
							sc.ChangeState(SCE_INNO_KEYWORD);
						} else if (keywordLists[KeywordIndex_Type]->InList(s)) {
							sc.ChangeState(SCE_INNO_PASCAL_TYPE);
						}
					}
					if (sc.state == SCE_INNO_IDENTIFIER) {
						if (sc.ch == '(') {
							sc.ChangeState(SCE_INNO_FUNCTION);
						} else if (keywordLists[KeywordIndex_PredefinedVariable]->InList(s)) {
							sc.ChangeState(SCE_INNO_CONSTANT);
						}
					}
				}
				sc.SetState(SCE_INNO_DEFAULT);
			}
			break;

		case SCE_INNO_STRING_SQ:
			if (sc.atLineStart) {
				sc.SetState(SCE_INNO_DEFAULT);
			} else if (sc.ch == '\'' || sc.ch == '{') {
				if (sc.ch == sc.chNext) {
					sc.Forward();
				} else if (sc.ch == '\'') {
					sc.ForwardSetState(SCE_INNO_DEFAULT);
				} else if (IsExpansionStartChar(sc.chNext)) {
					++expansionLevel;
					outerState = SCE_INNO_STRING_SQ;
					sc.SetState(SCE_INNO_INLINE_EXPANSION);
				}
			}
			break;

		case SCE_INNO_STRING_ANGLE:
			if (sc.atLineStart) {
				sc.SetState(SCE_INNO_DEFAULT);
			} else if (sc.ch == '>') {
				sc.ForwardSetState(SCE_INNO_DEFAULT);
			}
			break;

		case SCE_INNO_COMMENT:
		case SCE_INNO_PREPROCESSOR_MESSAGE:
			if (sc.atLineStart) {
				if (!(lineStatePrev & InnoLineStateLineContinuation)) {
					sc.SetState(SCE_INNO_DEFAULT);
				}
			}
			break;

		case SCE_INNO_ISPP_COMMENTBLOCK:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_INNO_DEFAULT);
			}
			break;

		case SCE_INNO_PASCAL_COMMENTPAREN:
			if (sc.Match('*', ')')) {
				sc.Forward();
				sc.ForwardSetState(SCE_INNO_DEFAULT);
			}
			break;

		case SCE_INNO_PASCAL_COMMENTBRACE:
			if (sc.ch == '}') {
				sc.ForwardSetState(SCE_INNO_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_INNO_DEFAULT) {
			if (ppKind == PreprocessorKind::Message && !isspacechar(sc.ch)) {
				sc.SetState(SCE_INNO_PREPROCESSOR_MESSAGE);
			} else if (ppKind == PreprocessorKind::Include && sc.ch == '<') {
				ppKind = PreprocessorKind::None;
				sc.SetState(SCE_INNO_STRING_ANGLE);
			} else if (sc.ch == '[' && visibleChars == 0) {
				lineState &= ~InnoLineStateCodeSection;
				lineState |= InnoLineStateSectionHeader;
				sc.SetState(SCE_INNO_SECTION);
			} else if (sc.ch == ';' && visibleChars == 0) {
				lineState &= ~InnoLineStateCodeSection;
				if (!(lineState & InnoLineStatePreprocessor)) {
					lineState |= InnoLineStateLineComment;
				}
				sc.SetState(SCE_INNO_COMMENT);
			} else if (sc.ch == '#' && visibleChars == 0 && !IsADigit(sc.chNext)) {
				lineState |= InnoLineStatePreprocessor;
				outerState = SCE_INNO_DEFAULT;
				ppKind = PreprocessorKind::Init;
				sc.SetState(SCE_INNO_PREPROCESSOR);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_INNO_STRING_DQ);
			} else if (sc.Match('/', '/') && visibleChars == 0) {
				if (!(lineState & InnoLineStatePreprocessor)) {
					lineState |= InnoLineStateLineComment;
				}
				sc.SetState(SCE_INNO_COMMENT);
			} else if (lineState & InnoLineStatePreprocessor) {
				if (sc.ch == ';' || sc.Match('/', '/')) {
					sc.SetState(SCE_INNO_COMMENT);
				} else if (sc.Match('/', '*')) {
					sc.SetState(SCE_INNO_ISPP_COMMENTBLOCK);
					sc.Forward();
				} else if (sc.ch == '\'') {
					sc.SetState(SCE_INNO_STRING_SQ);
				} else if (IsIdentifierStart(sc.ch)) {
					chBeforeIdentifier = chPrevNonWhite;
					sc.SetState(SCE_INNO_IDENTIFIER);
				} else if (IsADigit(sc.ch)) {
					sc.SetState(SCE_INNO_NUMBER);
				} else if (isoperator(sc.ch)) {
					sc.SetState(SCE_INNO_OPERATOR);
				}
			} else if ((lineState & InnoLineStateCodeSection) != 0 || checkParameter) {
				if (sc.Match('/', '/')) {
					sc.SetState(SCE_INNO_COMMENT);
				} else if (sc.Match('(', '*')) {
					sc.SetState(SCE_INNO_PASCAL_COMMENTPAREN);
					sc.Forward();
				} else if (sc.ch == '{') {
					sc.SetState(SCE_INNO_PASCAL_COMMENTBRACE);
				} else if (sc.ch == '\'') {
					sc.SetState(SCE_INNO_STRING_SQ);
				} else if (IsIdentifierStart(sc.ch)) {
					chBeforeIdentifier = chPrevNonWhite;
					sc.SetState(SCE_INNO_IDENTIFIER);
				} else if (IsADigit(sc.ch) || ((sc.ch == '&' || sc.ch == '#' || sc.ch == '$') && IsADigit(sc.chNext))) {
					sc.SetState(SCE_INNO_NUMBER);
				} else if (isoperator(sc.ch) || sc.ch == '@' || sc.ch == '#') {
					if (checkParameter && sc.ch == ';') {
						checkParameter = false;
						paramState = InnoParameterState::Key;
					}
					sc.SetState(SCE_INNO_OPERATOR);
				}
			} else if ((visibleChars == 0 || paramState == InnoParameterState::Key) && IsIdentifierStart(sc.ch)) {
				checkParameter = false;
				chBeforeIdentifier = sc.ch;
				sc.SetState(SCE_INNO_PARAMETER);
			} else if (paramState == InnoParameterState::Assign) {
				if (sc.ch == '=' || sc.ch == ':') {
					paramState = InnoParameterState::Value;
					sc.SetState(SCE_INNO_OPERATOR);
				} else if (!IsASpaceOrTab(sc.ch)) {
					paramState = InnoParameterState::Value;
					goto labelParamValue;
				}
			} else if (paramState == InnoParameterState::Value) {
labelParamValue:
				if (sc.ch == ';') {
					paramState = InnoParameterState::Key;
					sc.SetState(SCE_INNO_OPERATOR);
				} else if (sc.ch == '{') {
					if (sc.chNext == '{') {
						sc.Forward();
					} else if (IsExpansionStartChar(sc.chNext)) {
						++expansionLevel;
						outerState = SCE_INNO_DEFAULT;
						sc.SetState(SCE_INNO_INLINE_EXPANSION);
						if (sc.chNext == '#') {
							outerState = SCE_INNO_INLINE_EXPANSION;
							ppKind = PreprocessorKind::Init;
							sc.ForwardSetState(SCE_INNO_PREPROCESSOR);
						}
					}
				} else if (sc.ch == '%') {
					if (sc.chNext == '%') {
						sc.Forward();
					} else if (sc.chNext == 'n' || IsADigit(sc.chNext)) {
						outerState = SCE_INNO_DEFAULT;
						sc.SetState(SCE_INNO_PLACEHOLDER);
						sc.Forward();
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			chPrevNonWhite = sc.ch;
			++visibleChars;
		}
		if (sc.atLineEnd) {
			if (visibleChars == 0) {
				lineState |= InnoLineStateEmptyLine;
			} else if (lineState & InnoLineStatePreprocessor) {
				if (sc.LineEndsWith('\\')) {
					lineState &= ~InnoLineStateDefineLine;
					lineState |= InnoLineStateLineContinuation;
				}
			}
			styler.SetLineState(sc.currentLine, lineState);

			expansionLevel = 0;
			visibleChars = 0;
			chPrevNonWhite = 0;
			paramState = InnoParameterState::None;
			ppKind = PreprocessorKind::None;
			checkParameter = false;
			lineStatePrev = lineState;
			lineState &= InnoLineStateCodeSection;
			if (lineStatePrev & InnoLineStateLineContinuation) {
				lineState |= InnoLineStatePreprocessor;
			}
		}
		sc.Forward();
	}

	sc.Complete();
}

void FoldInnoDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineStatePrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineStatePrev = styler.GetLineState(lineCurrent - 1);
	}

	int levelNext = levelCurrent;
	int lineState = styler.GetLineState(lineCurrent);
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	char buf[12]; // interface
	constexpr int MaxFoldWordLength = sizeof(buf) - 1;
	int wordLen = 0;

	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_INNO_PASCAL_KEYWORD:
		case SCE_INNO_PREPROCESSOR:
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = UnsafeLower(styler[i]);
			}
			if (styleNext != style) {
				buf[wordLen] = '\0';
				wordLen = 0;
				if (style == SCE_INNO_PREPROCESSOR) {
					const char *p = buf;
					if (*p == '#') {
						++p;
					}
					if (StrStartsWith(p, "if") || StrEqual(p, "sub")) {
						levelNext++;
					} else if (StrStartsWith(p, "end")) {
						levelNext--;
					}
				} else {
					if (StrEqualsAny(buf, "begin", "case", "class", "try", "record", "interface")) {
						levelNext++;
					} else if (StrEqual(buf, "end")) {
						levelNext--;
					}
				}
			}
			break;

		case SCE_INNO_PASCAL_COMMENTBRACE:
		case SCE_INNO_PASCAL_COMMENTPAREN:
			if (style != stylePrev) {
				levelNext++;
			} else if (style != styleNext) {
				levelNext--;
			}
			break;
		}

		if (i == lineEndPos) {
			const int lineStateNext = styler.GetLineState(lineCurrent + 1);
			if (lineState & InnoLineStateSectionHeader) {
				levelCurrent = SC_FOLDLEVELBASE;
				levelNext = SC_FOLDLEVELBASE + 1;
				if (lineStatePrev & (InnoLineStateEmptyLine | InnoLineStateSectionHeader)) {
					styler.SetLevel(lineCurrent - 1, SC_FOLDLEVELBASE | (SC_FOLDLEVELBASE << 16));
				}
			} else if (lineState & InnoLineStateLineComment) {
				levelNext += (lineStateNext & InnoLineStateLineComment) - (lineStatePrev & InnoLineStateLineComment);
			} else if (lineState & InnoLineStateDefineLine) {
				levelNext += ((lineStateNext & InnoLineStateDefineLine) >> 3) - ((lineStatePrev & InnoLineStateDefineLine) >> 3);
			} else if ((lineState | lineStatePrev) & InnoLineStateLineContinuation) {
				levelNext += ((lineState & InnoLineStateLineContinuation) >> 2) - ((lineStatePrev & InnoLineStateLineContinuation) >> 2);
			}

			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}

			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineEndPos = sci::min(lineStartNext, endPos) - 1;
			levelCurrent = levelNext;
			lineStatePrev = lineState;
			lineState = lineStateNext;
		}
	}
}

}

LexerModule lmInnoSetup(SCLEX_INNOSETUP, ColouriseInnoDoc, "inno", FoldInnoDoc);
