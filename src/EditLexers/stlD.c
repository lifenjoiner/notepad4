#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_D = {{
//++Autogenerated -- start of section automatically generated
"__gshared __parameters __traits __vector abstract alias align asm assert auto body break "
"case cast catch class const continue debug default delegate delete deprecated do else enum export extern "
"false final finally for foreach foreach_reverse function goto if immutable import in inout int interface invariant is "
"lazy macro mixin module new nothrow null out override package pragma private protected public pure ref return "
"scope shared static struct super switch synchronized template this throw true try typeid typeof union unittest version "
"while with "

, // 1 types
"bool byte cdouble cent cfloat char creal dchar double dstring float idouble ifloat ireal long ptrdiff_t real "
"short size_t string ubyte ucent uint ulong ushort void wchar wstring "

, // 2 preprocessor
"line "

, // 3 attribute
"__future disable gnuAbiTag( live nogc optional property safe selector( system trusted "

, // 4 class
"Error Exception Object Throwable TypeInfo "

, // 5 struct
"Interface ModuleInfo "

, // 6 union
NULL

, // 7 interface
NULL

, // 8 trait
"^get ^has ^is allMembers child classInstanceSize compiles derivedMembers getVirtualIndex identifier initSymbol parent "
"toType "

, // 9 enumeration
NULL

, // 10 constant
"__DATE__ __EOF__ __FILE_FULL_PATH__ __FILE__ __FUNCTION__ __LINE__ __LOCAL_SIZE __MODULE__ __PRETTY_FUNCTION__ "
"__TIMESTAMP__ __TIME__ __VENDOR__ __VERSION__ "

, // 11 asm keywords
"align db dd de df di dl dq ds dw dword even far naked near offsetof ptr qword seg word "

, // 12 asm register
"AH AL AX BH BL BP BPL BX CH CL CR0 CR2 CR3 CR4 CS CX DH DI DIL DL DR0 DR1 DR2 DR3 DR6 DR7 DS DX "
"EAX EBP EBX ECX EDI EDX ES ESI ESP FS GS MM0 MM1 MM2 MM3 MM4 MM5 MM6 MM7 "
"R10 R10B R10D R10W R11 R11B R11D R11W R12 R12B R12D R12W R13 R13B R13D R13W R14 R14B R14D R14W R15 R15B R15D R15W "
"R8 R8B R8D R8W R9 R9B R9D R9W RAX RBP RBX RCX RDI RDX RSI RSP "
"SI SIL SP SPL SS ST TR3 TR4 TR5 TR6 TR7 "
"XMM0 XMM1 XMM10 XMM11 XMM12 XMM13 XMM14 XMM15 XMM2 XMM3 XMM4 XMM5 XMM6 XMM7 XMM8 XMM9 "
"YMM0 YMM1 YMM10 YMM11 YMM12 YMM13 YMM14 YMM15 YMM2 YMM3 YMM4 YMM5 YMM6 YMM7 YMM8 YMM9 "

, // 13 asm instruction
NULL

, // 14 function
NULL
//--Autogenerated -- end of section automatically generated

, // 15 Code Snippet
"for^() if^() switch^() while^() else^if^() else^{} foreach^() foreach_reverse^() with^() "
"synchronized^() catch^() scope^() version^() mixin^() "
}};

static EDITSTYLE Styles_D[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_D_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_D_WORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_D_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_D_ATTRIBUTE, NP2StyleX_Attribute, L"fore:#FF8000" },
	{ SCE_D_CLASS, NP2StyleX_Class, L"fore:#0080FF" },
	{ SCE_D_STRUCT, NP2StyleX_Structure, L"fore:#0080FF" },
	{ SCE_D_UNION, NP2StyleX_Union, L"fore:#0080FF" },
	{ SCE_D_INTERFACE, NP2StyleX_Interface, L"bold; fore:#1E90FF" },
	{ SCE_D_TRAIT, NP2StyleX_Trait, L"bold; fore:#1E90FF" },
	{ SCE_D_ENUM, NP2StyleX_Enumeration, L"fore:#FF8000" },
	{ SCE_D_CONSTANT, NP2StyleX_Constant, L"fore:#B000B0" },
	{ SCE_D_FUNCTION_DEFINITION, NP2StyleX_FunctionDefinition, L"bold; fore:#A46000" },
	{ SCE_D_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ MULTI_STYLE(SCE_D_COMMENTLINE, SCE_D_COMMENTBLOCK, SCE_D_NESTED_COMMENT, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_D_COMMENTLINEDOC, SCE_D_COMMENTBLOCKDOC, SCE_D_NESTED_COMMENTDOC, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_D_COMMENTDOCWORD, SCE_D_COMMENTDOCMACRO, 0, 0), NP2StyleX_DocCommentTag, L"fore:#408080" },
	{ SCE_D_TASKMARKER, NP2StyleX_TaskMarker, L"bold; fore:#408080" },
	{ MULTI_STYLE(SCE_D_STRING, SCE_D_CHARACTER, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ MULTI_STYLE(SCE_D_RAWSTRING, SCE_D_STRING_BT, 0, 0), NP2StyleX_RawString, L"fore:#008080" },
	{ SCE_D_HEXSTRING, NP2StyleX_HexString, L"fore:#C08000" },
	{ MULTI_STYLE(SCE_D_DELIMITED_STRING, SCE_D_DELIMITED_ID, 0, 0), NP2StyleX_DelimitedString, L"fore:#E24000" },
	//{ SCE_D_TOKEN_STRING, NP2StyleX_TokenString, L"fore:#A46000" },
	{ SCE_D_ESCAPECHAR, NP2StyleX_EscapeSequence, L"fore:#0080C0" },
	{ SCE_D_FORMAT_SPECIFIER, NP2StyleX_FormatSpecifier, L"fore:#7C5AF3" },
	{ SCE_D_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ SCE_D_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_D_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ SCE_D_ASM_INSTRUCTION, NP2StyleX_Instruction, L"fore:#0080FF" },
	{ SCE_D_ASM_REGISTER, NP2StyleX_Register, L"fore:#FF8000" },
};

EDITLEXER lexDLang = {
	SCLEX_DLANG, NP2LEX_DLANG,
//Settings++Autogenerated -- start of section automatically generated
		LexerAttr_PrintfFormatSpecifier |
		LexerAttr_CppPreprocessor,
		TAB_WIDTH_4, INDENT_WIDTH_4,
		(1 << 0) | (1 << 1), // class, function
		SCE_D_FUNCTION_DEFINITION,
		'\\', SCE_D_ESCAPECHAR, SCE_D_FORMAT_SPECIFIER,
		0,
		SCE_D_CHARACTER, 0,
		SCE_D_OPERATOR, 0
		, KeywordAttr32(0, KeywordAttr_PreSorted) // keywords
		| KeywordAttr32(1, KeywordAttr_PreSorted) // types
		| KeywordAttr32(2, KeywordAttr_NoLexer | KeywordAttr_NoAutoComp) // preprocessor
		| KeywordAttr32(3, KeywordAttr_NoLexer | KeywordAttr_NoAutoComp) // attribute
		| KeywordAttr32(4, KeywordAttr_PreSorted) // class
		| KeywordAttr32(5, KeywordAttr_PreSorted) // struct
		| KeywordAttr64(8, KeywordAttr_PreSorted) // trait
		| KeywordAttr64(10, KeywordAttr_PreSorted) // constant
		| KeywordAttr64(11, KeywordAttr_PreSorted | KeywordAttr_NoAutoComp) // asm keywords
		| KeywordAttr64(12, KeywordAttr_PreSorted | KeywordAttr_NoAutoComp) // asm register
		| KeywordAttr64(13, KeywordAttr_NoAutoComp) // asm instruction
		| KeywordAttr64(14, KeywordAttr_NoLexer) // function
		, SCE_D_TASKMARKER,
		SCE_D_CHARACTER, SCE_D_DELIMITED_STRING,
//Settings--Autogenerated -- end of section automatically generated
	EDITLEXER_HOLE(L"D Source", Styles_D),
	L"d; di; dd",
	&Keywords_D,
	Styles_D
};
