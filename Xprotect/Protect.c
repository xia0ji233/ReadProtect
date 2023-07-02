#include "Protect.h"
PVOID gs_HandleCallback;
NTKERNELAPI
UCHAR*
PsGetProcessImageFileName(
	__in PEPROCESS Process
);
OB_PREOP_CALLBACK_STATUS MyProtect(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation) {
	if (OperationInformation->KernelHandle) {

	}
	else {
		ACCESS_MASK AccessBitsToClear = PROCESS_TERMINATE | PROCESS_VM_READ | PROCESS_VM_WRITE;
		PEPROCESS process = (PEPROCESS)OperationInformation->Object;
		PUCHAR processName = PsGetProcessImageFileName(process);
		if (_stricmp((char *)processName, "XGame.exe") != 0) {
			return OB_PREOP_SUCCESS;
		}
		ACCESS_MASK Origin = OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		if (Origin&AccessBitsToClear) {
			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~AccessBitsToClear;
			}
			UINT64 NEW = (ACCESS_MASK)(OperationInformation->Parameters->CreateHandleInformation.DesiredAccess);
			kprintf(("xia0ji233: old=%p new=%p\n"), Origin, NEW);
		}



	}
	return OB_PREOP_SUCCESS;
}

void StartProtect() {
	OB_CALLBACK_REGISTRATION obl_callback_reg = { 0 };
	OB_OPERATION_REGISTRATION ob2_operation = { 0 };
	RtlInitUnicodeString(&obl_callback_reg.Altitude, L"321000");

	obl_callback_reg.RegistrationContext = NULL;
	obl_callback_reg.Version = OB_FLT_REGISTRATION_VERSION;
	obl_callback_reg.OperationRegistrationCount = 1;//只注册一个回调 
	obl_callback_reg.OperationRegistration = &ob2_operation;

	ob2_operation.ObjectType = PsProcessType;//设置为进程事件，PsThreadType为线程事件
	ob2_operation.Operations = OB_OPERATION_HANDLE_CREATE;
	ob2_operation.PostOperation = NULL;
	ob2_operation.PreOperation = MyProtect;//设置回调函数
	ObRegisterCallbacks(&obl_callback_reg, &gs_HandleCallback);//注册回调，获得句柄，等会用于关闭
	kprintf(("xia0ji233:Process Proctect start\n"));
	kprintf(("xia0ji233:gs_HandleCallback=%p\n"), gs_HandleCallback);
}
void EndProtect() {
	if (gs_HandleCallback)ObUnRegisterCallbacks(gs_HandleCallback);
	kprintf(("xia0ji233:Process Proctect end\n"));
}