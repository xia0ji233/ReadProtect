#include<ntifs.h>
#include"Protect.h"
void DriverUnload(PDRIVER_OBJECT pDriver) {
	kprintf(("Line %d:xia0ji233: start unload\n"), __LINE__);
	EndProtect();
}



NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath
) {

	typedef struct _LDR_DATA
	{
		struct _LIST_ENTRY InLoadOrderLinks;
		struct _LIST_ENTRY InMemoryOrderLinks;
		struct _LIST_ENTRY InInitializationOrderLinks;
		VOID* DllBase;
		VOID* EntryPoint;
		ULONG32      SizeOfImage;
		UINT8        _PADDING0_[0x4];
		struct _UNICODE_STRING FullDllName;
		struct _UNICODE_STRING BaseDllName;
		ULONG32      Flags;
	}LDR_DATA, *PLDR_DATA;
	PLDR_DATA ldr;
	ldr = (PLDR_DATA)DriverObject->DriverSection;
	ldr->Flags |= 0x20;

	StartProtect();
	DriverObject->DriverUnload = DriverUnload;
	kprintf(("Line %d:xia0ji233: RegistryPath = %S\n"), __LINE__, RegistryPath->Buffer);
	kprintf(("Line %d:xia0ji233: work success\n"), __LINE__);

	return 0;
}

