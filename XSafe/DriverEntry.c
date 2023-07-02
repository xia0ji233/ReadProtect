#include<ntifs.h>
#define SYMBOL L"\\??\\xia0ji2333"
#define kprintf(format, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, format, ##__VA_ARGS__)
#define ReadWriteCode CTL_CODE (FILE_DEVICE_UNKNOWN,0x805,METHOD_BUFFERED, FILE_ANY_ACCESS)
NTSTATUS ReadProcessMemory(UINT32 PID,PVOID Addr,UINT32 len,PVOID Buffer) {
	KAPC_STATE apc_state; 
	PEPROCESS Process;
	RtlZeroMemory(&apc_state, sizeof(KAPC_STATE));
	PVOID tmpBuf_Kernel = ExAllocatePool(NonPagedPool, len);
	PsLookupProcessByProcessId(PID, &Process);
	KeStackAttachProcess((PVOID)Process, &apc_state);
	int dwRet = MmIsAddressValid(Addr);
	if (dwRet) {
		kprintf(("xia0ji233:Attach Success\n"));
		RtlCopyMemory(tmpBuf_Kernel, Addr, len);
		kprintf(("xia0ji233:readmemory:%p\n"), *(UINT64*)Addr);
	}
	KeUnstackDetachProcess(&apc_state);
	RtlCopyMemory(Buffer, tmpBuf_Kernel, len);

	ExFreePool(tmpBuf_Kernel);
	return dwRet;
}

NTSTATUS CreateDevice(PDEVICE_OBJECT driver) {
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device = NULL;
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\xia0ji233");
	status = IoCreateDevice(
		driver,
		sizeof(driver->DeviceExtension),
		&MyDriver,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&device
	);
	if (status == STATUS_SUCCESS) {
		UNICODE_STRING Sym;
		RtlInitUnicodeString(&Sym, SYMBOL);
		status = IoCreateSymbolicLink(&Sym, &MyDriver);
		if (status == STATUS_SUCCESS) {
			kprintf(("Line %d:xia0ji233: symbol linked success\n"), __LINE__);
		}
		else {
			kprintf(("Line %d:xia0ji233: symbol linked failed status=%x\n"), __LINE__, status);
		}
	}
	else {
		kprintf(("Line %d:xia0ji233: create device fail status=%x\n"), __LINE__, status);
	}
}
NTSTATUS DeviceIrpCtl(PDEVICE_OBJECT driver, PIRP pirp) {
	kprintf(("Line %d:xia0ji233: enter IRP Function\n"), __LINE__);
	PIO_STACK_LOCATION irpStackL;
	ULONG CtlCode;
	ULONG InputBuffLength;
	irpStackL = IoGetCurrentIrpStackLocation(pirp);//获取应用层传来的参数
	switch (irpStackL->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL: {
		UINT32 code = irpStackL->Parameters.DeviceIoControl.IoControlCode;
		kprintf(("Line %d:xia0ji233: ring3 calls DEVICE CONTROL,control code=%x\n"), __LINE__, code);
		switch (code) {
		case ReadWriteCode: {
			typedef struct Mm {
				UINT32 PID;
				UINT32 len;
				PVOID Addr;
			}Mm;
			char * buffer = (char *)pirp->AssociatedIrp.SystemBuffer;
			Mm *s = (Mm *)buffer;
			kprintf(("Line %d:xia0ji233:pid=%d,addr=%p,len=%d\n"), __LINE__, s->PID, s->Addr,s->len);
			ReadProcessMemory(s->PID, s->Addr, s->len, buffer);
			kprintf(("xia0ji233:buffermemory:%p\n"), *(UINT64*)buffer);
			pirp->IoStatus.Information = s->len;
			break;
		}
		default:
			kprintf(("xia0ji233:unknown code\n"));
		}

		break;
	}
	case IRP_MJ_CREATE: {
		kprintf(("Line %d:xia0ji233: ring3 calls CREATE\n"), __LINE__);
		break;
	}
	case IRP_MJ_CLOSE: {
		kprintf(("Line %d:xia0ji233: ring3 calls CLOSE\n"), __LINE__);
		break;
	}
	default:
		break;
	}
	pirp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	kprintf(("Line %d:xia0ji233: leave IRP Function\n"), __LINE__);
	return STATUS_SUCCESS;

}

void DeleteDevice(PDRIVER_OBJECT pDriver) {
	kprintf(("Line %d:xia0ji233: start delete device\n"), __LINE__);
	if (pDriver->DeviceObject) {
		//删除符号链接
		UNICODE_STRING Sym;//符号链接名字
		RtlInitUnicodeString(&Sym, SYMBOL);//CreateFile 
		kprintf(("Line %d:xia0ji233: Delete Symbol\n"), __LINE__);
		IoDeleteSymbolicLink(&Sym);
		kprintf(("Line %d:xia0ji233: Delete Device\n"), __LINE__);
		IoDeleteDevice(pDriver->DeviceObject);//删除设备对象
	}
	kprintf(("Line %d:xia0ji233: end delete device\n"), __LINE__);
}

void DriverUnload(PDRIVER_OBJECT pDriver) {
	kprintf(("Line %d:xia0ji233: start unload\n"), __LINE__);
	DeleteDevice(pDriver);
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath
) {
	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceIrpCtl;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceIrpCtl;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIrpCtl;
	CreateDevice(DriverObject);
	kprintf(("Line %d:xia0ji233: RegistryPath = %S\n"), __LINE__, RegistryPath->Buffer);
	kprintf(("Line %d:xia0ji233: work success\n"), __LINE__);

	return 0;
}

