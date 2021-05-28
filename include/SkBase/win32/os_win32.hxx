#pragma once


extern "C"
{


#define NTAPI        __stdcall
#define NTSYSAPI     DECLSPEC_IMPORT
#define NTSYSCALLAPI DECLSPEC_IMPORT


typedef LONG NTSTATUS;

#define NT_SUCCESS(Status)  (((NTSTATUS)(Status)) >= 0)

#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)

#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)
#define STATUS_CANCELLED                 ((NTSTATUS)0xC0000120L)
#define STATUS_CONNECTION_REFUSED        ((NTSTATUS)0xC0000236L)
#define STATUS_IO_TIMEOUT                ((NTSTATUS)0xC00000B5L)
#define STATUS_BUFFER_TOO_SMALL          ((NTSTATUS)0xC0000023L)
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)
#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
#define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034L)
#define STATUS_OBJECT_PATH_NOT_FOUND     ((NTSTATUS)0xC000003AL)
#define STATUS_OBJECT_NAME_INVALID       ((NTSTATUS)0xC0000033L)
#define STATUS_OBJECT_PATH_SYNTAX_BAD    ((NTSTATUS)0xC000003BL)
#define STATUS_SHARING_VIOLATION         ((NTSTATUS)0xC0000043L)
#define STATUS_NO_MORE_ENTRIES           ((NTSTATUS)0x8000001AL)
#define STATUS_NOT_ALL_ASSIGNED          ((NTSTATUS)0x00000106L)



NTSYSAPI
ULONG
__cdecl
DbgPrintEx(
    ULONG ComponentId,
    ULONG Level,
    PCSTR Format,
    ...
);


NTSYSAPI
NTSTATUS
NTAPI
RtlGetVersion(
    PRTL_OSVERSIONINFOW lpVersionInformation
);


typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} ANSI_STRING;

typedef ANSI_STRING *PANSI_STRING;
typedef ANSI_STRING CONST *PCANSI_STRING;


typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWCH   Buffer;
} UNICODE_STRING;

typedef UNICODE_STRING *PUNICODE_STRING;
typedef UNICODE_STRING CONST *PCUNICODE_STRING;


NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    PANSI_STRING     DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN          AllocateDestinationString
);

NTSYSAPI
VOID
NTAPI
RtlFreeAnsiString(
    PANSI_STRING AnsiString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PCANSI_STRING   SourceString,
    BOOLEAN         AllocateDestinationString
);


char _RTL_CONSTANT_STRING_type_check(const void *s);
#define _RTL_CONSTANT_STRING_remove_const_macro(s) (s)
#define RTL_CONSTANT_STRING(s) \
{ \
    sizeof( s ) - sizeof( (s)[0] ), \
    sizeof( s ) / sizeof(_RTL_CONSTANT_STRING_type_check(s)), \
    _RTL_CONSTANT_STRING_remove_const_macro(s) \
}


typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;

typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

#define OBJ_INHERIT                             0x00000002L
#define OBJ_PERMANENT                           0x00000010L
#define OBJ_EXCLUSIVE                           0x00000020L
#define OBJ_CASE_INSENSITIVE                    0x00000040L
#define OBJ_OPENIF                              0x00000080L
#define OBJ_OPENLINK                            0x00000100L
#define OBJ_KERNEL_HANDLE                       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK                  0x00000400L
#define OBJ_VALID_ATTRIBUTES                    0x000007F2L


#define InitializeObjectAttributes(p, n, a, r, s) { \
  (p)->Length = sizeof(OBJECT_ATTRIBUTES);          \
  (p)->RootDirectory = (r);                         \
  (p)->Attributes = (a);                            \
  (p)->ObjectName = (n);                            \
  (p)->SecurityDescriptor = (s);                    \
  (p)->SecurityQualityOfService = NULL;             \
}


NTSYSAPI
NTSTATUS
NTAPI
RtlStringFromGUID(
    REFGUID Guid,
    PUNICODE_STRING GuidString
);

LONG NTSYSAPI
NTAPI
RtlCompareUnicodeString(
    PCUNICODE_STRING String1,
    PCUNICODE_STRING String2,
    BOOLEAN          CaseInSensitive
);

VOID NTSYSAPI
NTAPI
RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
);

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString(
    PUNICODE_STRING Destination,
    PCWSTR Source
);

VOID
NTAPI
RtlFreeUnicodeString(
    PUNICODE_STRING UnicodeString
);

__forceinline
VOID
RtlInitEmptyUnicodeString(
    PUNICODE_STRING UnicodeString,
    PWCHAR Buffer,
    USHORT BufferSize
)
{
    UnicodeString->Length = 0;
    UnicodeString->MaximumLength = BufferSize;
    UnicodeString->Buffer = Buffer;
}

NTSYSAPI
VOID
NTAPI
RtlCopyUnicodeString(
    PUNICODE_STRING DestinationString,
    PCUNICODE_STRING SourceString
);


typedef enum _HARDERROR_RESPONSE {
    ResponseReturnToCaller,
    ResponseNotHandled,
    ResponseAbort,
    ResponseCancel,
    ResponseIgnore,
    ResponseNo,
    ResponseOk,
    ResponseRetry,
    ResponseYes
} HARDERROR_RESPONSE, *PHARDERROR_RESPONSE;

typedef enum _HARDERROR_RESPONSE_OPTION {
    OptionAbortRetryIgnore,
    OptionOk,
    OptionOkCancel,
    OptionRetryCancel,
    OptionYesNo,
    OptionYesNoCancel,
    OptionShutdownSystem
} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;


NTSYSAPI
NTSTATUS
NTAPI
NtRaiseHardError(
    NTSTATUS             ErrorStatus,
    ULONG                NumberOfParameters,
    PUNICODE_STRING      UnicodeStringParameterMask OPTIONAL,
    PVOID                *Parameters,
    HARDERROR_RESPONSE_OPTION ResponseOption,
    PHARDERROR_RESPONSE Response);



NTSYSAPI
NTSTATUS
NTAPI
ZwClose(
    HANDLE Handle
);


typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64,
    MaxKeyValueInfoClass
} KEY_VALUE_INFORMATION_CLASS;

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation,
    KeyCachedInformation,
    KeyFlagsInformation,
    KeyVirtualizationInformation,
    KeyHandleTagsInformation,
    KeyTrustInformation,
    MaxKeyInfoClass
} KEY_INFORMATION_CLASS;

typedef struct _KEY_NODE_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   NameLength;
    WCHAR   Name[1];
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;


NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteKey(
    HANDLE KeyHandle
);

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteValueKey(
    HANDLE          KeyHandle,
    PUNICODE_STRING ValueName
);

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenKey(
    PHANDLE             pKeyHandle,
    ACCESS_MASK          DesiredAccess,
    POBJECT_ATTRIBUTES   ObjectAttributes
);

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateKey(
    PHANDLE            KeyHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    ULONG              TitleIndex,
    PUNICODE_STRING    Class,
    ULONG              CreateOptions,
    PULONG             Disposition
);

NTSYSAPI
NTSTATUS
NTAPI
ZwSetValueKey(
    HANDLE               KeyHandle,
    PUNICODE_STRING      ValueName,
    ULONG                TitleIndex,
    ULONG                Type,
    PVOID                Data,
    ULONG                DataSize
);

NTSTATUS ZwQueryValueKey(
    HANDLE                      KeyHandle,
    PUNICODE_STRING             ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength
);

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateKey(
    HANDLE KeyHandle,
    ULONG Index,
    KEY_INFORMATION_CLASS KeyInformationClass,
    PVOID KeyInformation,
    ULONG Length,
    PULONG ResultLength
);

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateValueKey(
    HANDLE KeyHandle,
    ULONG Index,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID KeyValueInformation,
    ULONG Length,
    PULONG ResultLength
);

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckRegistryKey(
    _In_ ULONG RelativeTo,
    _In_ PWSTR Path
);

#define RTL_REGISTRY_ABSOLUTE     0   // Path is a full path
#define RTL_REGISTRY_SERVICES     1   // \Registry\Machine\System\CurrentControlSet\Services
#define RTL_REGISTRY_CONTROL      2   // \Registry\Machine\System\CurrentControlSet\Control
#define RTL_REGISTRY_WINDOWS_NT   3   // \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
#define RTL_REGISTRY_DEVICEMAP    4   // \Registry\Machine\Hardware\DeviceMap
#define RTL_REGISTRY_USER         5   // \Registry\User\CurrentUser
#define RTL_REGISTRY_MAXIMUM      6
#define RTL_REGISTRY_HANDLE       0x40000000    // Low order bits are registry handle
#define RTL_REGISTRY_OPTIONAL     0x80000000    // Indicates the key node is optional


} // extern "C"
