// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
//! Lexer for Verilog, SystemVerilog.

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

struct EscapeSequence {
	int digitsLeft = 0;
	bool hex = false;

	// highlight any character as escape sequence.
	void resetEscapeState(int chNext) noexcept {
		digitsLeft = 1;
		if (chNext == 'x') {
			digitsLeft = 3;
			hex = true;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			hex = false;
		}
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsOctalOrHex(ch, hex);
	}
};

enum {
	VerilogLineStateMaskLineComment = 1,
	VerilogLineStateMaskDirective = 1 << 1,
	// extern, typedef, import, export, pure, bind
	VerilogLineStateMaskDeclaration = 1 << 2,
	// (* attribute *)
	VerilogLineStateMaskAttribute = 1 << 3,
	VerilogLineStateLineContinuation = 1 << 4,
};

//KeywordIndex++Autogenerated -- start of section automatically generated
enum {
	KeywordIndex_Keyword = 0,
	KeywordIndex_CodeFolding = 1,
	KeywordIndex_DataType = 2,
	KeywordIndex_Directive = 3,
};
//KeywordIndex--Autogenerated -- end of section automatically generated

enum class KeywordType {
	None,
	Interface,		// interface class
	Virtual,		// virtual interface
	DisableWait,	// disable fork, wait fork
	Rand,			// rand join
	Scope,			// $scope scope_type $end
	Module,			// module identifier
	Macro,			// `define identifier
};

constexpr bool IsVWordChar(int ch) noexcept {
	return IsIdentifierChar(ch) || ch == '$';
}

constexpr bool IsVWordCharEx(int ch, int state) noexcept {
	return (state == SCE_V_ESCAPE_IDENTIFIER) ? IsAGraphic(ch) : IsVWordChar(ch);
}

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_V_TASKMARKER;
}

constexpr bool IsIdentifierStyle(int state) noexcept {
	return state > SCE_V_IDENTIFIER && state <= SCE_V_KEYWORD;
}

constexpr bool IsFormatSpecifier(char ch) noexcept {
	return AnyOf(ch, 'b', 'B',
					'c', 'C',
					'd', 'D',
					'e', 'E',
					'f', 'F',
					'g', 'G',
					'h', 'H',
					'l', 'L',
					'm', 'M',
					'o', 'O',
					'p', 'P',
					's', 'S',
					't', 'T',
					'u', 'U',
					'v', 'V',
					'x', 'X');
}

inline Sci_Position CheckFormatSpecifier(const StyleContext &sc, LexAccessor &styler, bool insideUrl) noexcept {
	if (sc.chNext == '%') {
		return 2;
	}
	if (insideUrl && IsHexDigit(sc.chNext)) {
		// percent encoded URL string
		return 0;
	}
	if (IsASpaceOrTab(sc.chNext) && IsADigit(sc.chPrev)) {
		// ignore word after percent: "5% x"
		return 0;
	}

	Sci_PositionU pos = sc.currentPos + 1;
	char ch = static_cast<char>(sc.chNext);
	// width
	while (IsADigit(ch)) {
		ch = styler[++pos];
	}
	// .precision
	if (ch == '.') {
		ch = styler[++pos];
		if (ch == '*') {
			ch = styler[++pos];
		} else {
			while (IsADigit(ch)) {
				ch = styler[++pos];
			}
		}
	}
	// format specification
	if (IsFormatSpecifier(ch)) {
		return pos - sc.currentPos + 1;
	}
	return 0;
}

inline bool FindClockingEvent(LexAccessor &styler, Sci_PositionU startPos, Sci_PositionU endPos) noexcept {
	// clocking [clocking_identifier] clocking_event
	enum class ClockingState {
		BeforeIdentifier,
		Identifier,
		EscapeIdentifier,
		AfterIdentifier,
	};

	ClockingState state = ClockingState::BeforeIdentifier;
	while (startPos < endPos) {
		const uint8_t ch = styler[startPos++];
		if (state == ClockingState::BeforeIdentifier && ch > ' ') {
			state = (ch == '\\') ? ClockingState::EscapeIdentifier : ClockingState::Identifier;
		}
		if ((state == ClockingState::Identifier && !IsVWordChar(ch))
			|| (state == ClockingState::EscapeIdentifier && !IsAGraphic(ch))) {
			state = ClockingState::AfterIdentifier;
		}
		if (state == ClockingState::AfterIdentifier) {
			if (ch == '@') {
				return true;
			}
			if (ch > ' ') {
				break;
			}
		}
	}
	return false;
}

void ColouriseVerilogDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineState = 0;
	int parenCount = 0;

	bool insideUrl = false;
	bool angleQuote = false; // `include <path>
	bool backtickQuote = false; // `" `"
	int visibleChars = 0;
	KeywordType kwType = KeywordType::None;
	int chBeforeIdentifier = 0;
	int chPrevNonWhite = 0;
	int stylePrevNonWhite = SCE_V_DEFAULT;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		8: lineState
		4: kwType
		20: parenCount
		*/
		parenCount = lineState >> 12;
		kwType = static_cast<KeywordType>((lineState >> 8) & 15);
		lineState &= VerilogLineStateMaskDeclaration | VerilogLineStateMaskAttribute | VerilogLineStateLineContinuation;
	}
	if (startPos != 0 && IsSpaceEquiv(initStyle)) {
		LookbackNonWhite(styler, startPos, SCE_V_TASKMARKER, chPrevNonWhite, stylePrevNonWhite);
	}

	while (sc.More()) {
		if (sc.atLineStart) {
			lineState &= VerilogLineStateMaskDeclaration | VerilogLineStateMaskAttribute | VerilogLineStateLineContinuation;
			visibleChars = 0;
			angleQuote = false;
			if (kwType == KeywordType::Macro) {
				kwType = KeywordType::None;
			}
		}

		switch (sc.state) {
		case SCE_V_OPERATOR:
			sc.SetState(SCE_V_DEFAULT);
			break;

		case SCE_V_NUMBER:
			if (!(IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext) || sc.ch == '?' || sc.ch == '\'')) {
				sc.SetState(SCE_V_DEFAULT);
			}
			break;

		case SCE_V_STRING:
			if (sc.atLineStart) {
				if (lineState & VerilogLineStateLineContinuation) {
					lineState &= ~VerilogLineStateLineContinuation;
				} else {
					sc.SetState(SCE_V_DEFAULT);
					break;
				}
			}
			if (sc.ch == (angleQuote ? '>' : '\"')) {
				angleQuote = false;
				sc.ForwardSetState(SCE_V_DEFAULT);
			} else if (!angleQuote) {
				if (sc.ch == '\\') {
					if (IsEOLChar(sc.chNext)) {
						lineState |= VerilogLineStateLineContinuation;
					} else {
						escSeq.resetEscapeState(sc.chNext);
						sc.SetState(SCE_V_ESCAPECHAR);
						sc.Forward();
					}
				} else if (sc.ch == '%') {
					const Sci_Position length = CheckFormatSpecifier(sc, styler, insideUrl);
					if (length != 0) {
						sc.SetState(SCE_V_FORMAT_SPECIFIER);
						sc.Advance(length);
						sc.SetState(SCE_V_STRING);
						continue;
					}
				} else if (sc.Match(':', '/', '/') && IsLowerCase(sc.chPrev)) {
					insideUrl = true;
				} else if (insideUrl && IsInvalidUrlChar(sc.ch)) {
					insideUrl = false;
				} else if (backtickQuote && sc.Match('`', '\\', '`', '\"')) {
					escSeq.digitsLeft = 1;
					sc.SetState(SCE_V_ESCAPECHAR);
					sc.Advance(3);
				}
			}
			break;

		case SCE_V_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(SCE_V_STRING);
				continue;
			}
			break;

		case SCE_V_IDENTIFIER:
		case SCE_V_ESCAPE_IDENTIFIER:
		case SCE_V_DIRECTIVE:
		case SCE_V_SYSTEM_TASK:
			if (!IsVWordCharEx(sc.ch, sc.state)) {
				if (sc.state == SCE_V_ESCAPE_IDENTIFIER && sc.ch < ' ') {
					sc.Forward();
				}

				const int chNext = sc.GetDocNextChar();
				const KeywordType prevWord = kwType;
				kwType = KeywordType::None;
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				switch (sc.state) {
				case SCE_V_SYSTEM_TASK:
					if (chNext != '(' && StrEqual(s, "$scope")) {
						kwType = KeywordType::Scope;
					} else if (StrEqual(s, "$comment")) {
						sc.ChangeState(SCE_V_COMMENTBLOCKVSD);
					}
					break;

				case SCE_V_DIRECTIVE:
					lineState |= VerilogLineStateMaskDirective;
					if (StrEqual(s + 1, "include")) {
						angleQuote = chNext == '<';
					} else if (StrEqualsAny(s + 1, "define", "undef", "ifdef", "ifndef")) {
						kwType = KeywordType::Macro;
					} else if (!keywordLists[KeywordIndex_Directive].InList(s + 1)) {
						lineState &= ~VerilogLineStateMaskDirective;
						sc.ChangeState(SCE_V_MACRO);
					} else if (StrStartsWith(s + 1, "__")) { // __FILE__, __LINE__
						lineState &= ~VerilogLineStateMaskDirective;
					}
					break;

				default:
					if (s[0] == '@') {
						sc.ChangeState(SCE_V_EVENT);
					} else if (s[0] == ':') {
						sc.ChangeState(SCE_V_LABEL);
					} else if (prevWord == KeywordType::Macro) {
						sc.ChangeState(SCE_V_MACRO);
					} else if (sc.state == SCE_V_IDENTIFIER && chBeforeIdentifier != '#') {
						// # delay, ## delay
						if (keywordLists[KeywordIndex_DataType].InList(s)) {
							sc.ChangeState(SCE_V_DATATYPE);
						} else if (keywordLists[KeywordIndex_CodeFolding].InList(s)) {
							bool fold = false;
							if (parenCount == 0 && lineState == 0 && prevWord != KeywordType::Scope
								&& !(chBeforeIdentifier == '.' || chBeforeIdentifier == ':')) {
								fold = true;
								if (StrEqual(s, "fork")) {
									// IEEE 1800 A.6.5 Timing control statements
									fold = prevWord != KeywordType::DisableWait;
								} else if (StrEqual(s, "join")) {
									// IEEE 1800 A.6.12 Randsequence
									fold = prevWord != KeywordType::Rand;
								} else if (StrEqual(s, "class")) {
									fold = prevWord != KeywordType::Interface;
								} else if (StrEqual(s, "clocking")) {
									fold = chNext == '@' || FindClockingEvent(styler, sc.currentPos, sc.lineStartNext);
								} else if (StrEqualsAny(s, "property", "sequence")) {
									// IEEE 1800 A.2.10 Assertion declarations
									fold = chNext != '(';
								} else if (StrEqual(s, "interface")) {
									// IEEE 1800 A.1.3 Module parameters and ports
									// interface_port_header ::= interface [.modport_identifier]
									// IEEE 1800 A.2.2 Declaration data types
									// data_type ::= virtual [interface] interface_identifier [parameter_value_assignment] [.modport_identifier]
									kwType = KeywordType::Interface;
									fold = chNext != '.' && prevWord != KeywordType::Virtual;
								} else if (StrEqualsAny(s, "module", "macromodule")) {
									kwType = KeywordType::Module;
								}
							}
							sc.ChangeState(fold ? SCE_V_FOLDING_KEYWORD : SCE_V_KEYWORD);
						} else if (keywordLists[KeywordIndex_Keyword].InList(s)) {
							sc.ChangeState(SCE_V_KEYWORD);
							if (parenCount == 0 && !(chBeforeIdentifier == '.' || chBeforeIdentifier == ':')) {
								if (StrEqual(s, "rand")) {
									kwType = KeywordType::Rand;
								} else if (StrEqualsAny(s, "wait", "disable")) {
									kwType = KeywordType::DisableWait;
								} else if (StrEqual(s, "virtual")) {
									kwType = KeywordType::Virtual;
								} else if (StrEqualsAny(s, "pure", "bind", "extern", "typedef", "import", "export")) {
									lineState |= VerilogLineStateMaskDeclaration;
								}
							}
						}
					}
					if (sc.state == SCE_V_IDENTIFIER || sc.state == SCE_V_ESCAPE_IDENTIFIER) {
						if (parenCount == 0 && sc.ch == ':' && sc.chNext != ':' && visibleChars == sc.LengthCurrent()) {
							sc.ChangeState(SCE_V_LABEL);
						} else if (lineState & VerilogLineStateMaskAttribute) {
							if (chBeforeIdentifier != '=') {
								sc.ChangeState(SCE_V_ATTRIBUTE);
							}
						} else if (prevWord == KeywordType::Module) {
							sc.ChangeState(SCE_V_MODULE);
						} else if (chNext == '(') {
							sc.ChangeState(SCE_V_FUNCTION);
						}
					}
					break;
				}

				if (sc.state != SCE_V_COMMENTBLOCKVSD) {
					stylePrevNonWhite = sc.state;
					sc.SetState(SCE_V_DEFAULT);
				}
			}
			break;

		case SCE_V_COMMENTLINE:
		case SCE_V_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_V_DEFAULT);
			}
			break;

		case SCE_V_COMMENTBLOCK:
		case SCE_V_COMMENTBLOCKDOC:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_V_DEFAULT);
			}
			break;

		case SCE_V_COMMENTBLOCKVSD:
			if (sc.Match('$', 'e')) {
				char s[4]{};
				styler.GetRange(sc.currentPos + 2, sc.currentPos + 5, s, sizeof(s));
				if (s[0] == 'n' && s[1] == 'd' && !IsVWordChar(s[2])) {
					sc.Advance(3); // $end
					sc.ForwardSetState(SCE_V_DEFAULT);
				}
			}
			break;
		}

		if (sc.state == SCE_V_DEFAULT) {
			if (sc.ch == '/' && (sc.chNext == '/' || sc.chNext == '*')) {
				sc.SetState((sc.chNext == '/') ? SCE_V_COMMENTLINE : SCE_V_COMMENTBLOCK);
				sc.Forward();
				if (sc.ch == '/') {
					if (visibleChars == 0) {
						lineState |= VerilogLineStateMaskLineComment;
					}
					if (sc.chNext == '!') {
						sc.ChangeState(SCE_V_COMMENTLINEDOC);
					}
				} else {
					if (sc.chNext == '!' || sc.chNext == '*') {
						sc.ChangeState(SCE_V_COMMENTBLOCKDOC);
					}
				}
			} else if (sc.ch == '\"') {
				insideUrl = false;
				angleQuote = false;
				backtickQuote = sc.chPrev == '`' && (lineState & VerilogLineStateMaskDirective);
				sc.SetState(SCE_V_STRING);
			} else if (sc.ch == '<') {
				sc.SetState(angleQuote ? SCE_V_STRING : SCE_V_OPERATOR);
			} else if (sc.ch == '\'') {
				sc.SetState(IsAlpha(sc.chNext) ? SCE_V_NUMBER : SCE_V_OPERATOR);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_V_NUMBER);
			} else if ((sc.ch == '`' || sc.ch == '$') && IsIdentifierStart(sc.chNext)) {
				sc.SetState((sc.ch == '`') ? SCE_V_DIRECTIVE : SCE_V_SYSTEM_TASK);
			} else if (IsIdentifierStart(sc.ch) || ((sc.ch == '@' || (sc.ch == ':' && stylePrevNonWhite == SCE_V_FOLDING_KEYWORD))
				&& (sc.chNext == '\\' || IsIdentifierStart(sc.chNext)))) {
				chBeforeIdentifier = chPrevNonWhite;
				if (!IsIdentifierStyle(stylePrevNonWhite)) {
					kwType = KeywordType::None;
				}
				sc.SetState((sc.chNext == '\\' && (sc.ch == '@' || sc.ch == ':')) ? SCE_V_ESCAPE_IDENTIFIER : SCE_V_IDENTIFIER);
			} else if (sc.ch == '\\' && IsAGraphic(sc.chNext)) {
				sc.SetState(SCE_V_ESCAPE_IDENTIFIER);
			} else if (IsAGraphic(sc.ch)) {
				kwType = KeywordType::None;
				sc.SetState(SCE_V_OPERATOR);
				if (sc.ch == ';') {
					lineState &= ~VerilogLineStateMaskDeclaration;
				} else if (sc.Match('(', '*')) {
					++parenCount;
					lineState |= VerilogLineStateMaskAttribute;
					sc.Forward();
				} else if (sc.Match('*', ')')) {
					lineState &= ~VerilogLineStateMaskAttribute;
				} else if (sc.ch == '(' || sc.ch == '[' || sc.ch == '{') {
					++parenCount;
				} else if (sc.ch == ')' || sc.ch == ']' || sc.ch == '}') {
					if (parenCount > 0) {
						--parenCount;
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
			if (!IsSpaceEquiv(sc.state)) {
				chPrevNonWhite = sc.ch;
				stylePrevNonWhite = sc.state;
			}
		}
		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineState | (static_cast<int>(kwType) << 8) | (parenCount << 12));
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & VerilogLineStateMaskLineComment;
}

void FoldVerilogDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = GetLineCommentState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = GetLineCommentState(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	lineStartNext = sci::min(lineStartNext, endPos);

	char buf[16]; // celldefine
	constexpr int MaxFoldWordLength = sizeof(buf) - 1;
	int wordLen = 0;

	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	while (startPos < endPos) {
		const int stylePrev = style;
		style = styleNext;
		const char ch = styler[startPos++];
		styleNext = styler.StyleAt(startPos);

		switch (style) {
		case SCE_V_COMMENTBLOCK:
		case SCE_V_COMMENTBLOCKDOC:
		case SCE_V_COMMENTBLOCKVSD:
			if (style != stylePrev) {
				levelNext++;
			} else if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_V_DIRECTIVE:
		case SCE_V_FOLDING_KEYWORD:
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = ch;
			}
			if (styleNext != style) {
				buf[wordLen] = '\0';
				wordLen = 0;
				if (style == SCE_V_DIRECTIVE) {
					if (StrEqualsAny(buf + 1, "ifdef", "ifndef", "celldefine") || StrStartsWith(buf + 1, "begin")) {
						levelNext++;
					} else if (StrStartsWith(buf + 1, "end")) {
						levelNext--;
					}
				} else {
					if (StrStartsWith(buf, "end") || StrStartsWith(buf, "join")) {
						levelNext--;
					} else {
						levelNext++;
					}
				}
			}
			break;

		case SCE_V_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
			break;
		}

		if (startPos == lineStartNext) {
			const int lineCommentNext = GetLineCommentState(styler.GetLineState(lineCurrent + 1));
			levelNext = sci::max(levelNext, SC_FOLDLEVELBASE);
			if (lineCommentCurrent) {
				levelNext += lineCommentNext - lineCommentPrev;
			}

			const int levelUse = levelCurrent;
			int lev = levelUse | (levelNext << 16);
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			styler.SetLevel(lineCurrent, lev);

			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineStartNext = sci::min(lineStartNext, endPos);
			levelCurrent = levelNext;
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
		}
	}
}

}

extern const LexerModule lmVerilog(SCLEX_VERILOG, ColouriseVerilogDoc, "verilog", FoldVerilogDoc);
