//  [5/11/2015 uty]
#include <ntddk.h>
//-----------------------------------------------------------------------------//
NTSTATUS
NIAPGetRegKeyValue (
	__in HANDLE Handle,
	__in PWCHAR KeyName,
	__in ULONG KeyNameLength,
	__inout PVOID Data,
	__in ULONG DataLength,
	__out PULONG Type,
	__out PULONG RetrunLength
	)
{
	UNICODE_STRING uniKeyName;
	ULONG length;
	PKEY_VALUE_FULL_INFORMATION fullInfo;

	NTSTATUS Status = STATUS_INSUFFICIENT_RESOURCES;


	RtlInitUnicodeString (&uniKeyName, KeyName);

	length = sizeof(KEY_VALUE_FULL_INFORMATION) + KeyNameLength + DataLength;
	fullInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool, length); 


	if (fullInfo) 
	{
		Status = ZwQueryValueKey (Handle,
			&uniKeyName,
			KeyValueFullInformation,
			fullInfo,
			length,
			&length);


		if (NT_SUCCESS(Status)) 
		{
			//
			// If there is enough room in the data buffer, copy the output
			//

			if (DataLength >= fullInfo->DataLength) 
			{
				RtlCopyMemory (Data, 
					((PUCHAR) fullInfo) + fullInfo->DataOffset, 
					fullInfo->DataLength
					);
			}
			*RetrunLength = fullInfo->DataLength;
			*Type = fullInfo->Type;
		}

		ExFreePool(fullInfo);
	}

	return Status;
}
//-----------------------------------------------------------------------------//
NTSTATUS
NIAPGetRegValue (
	__in PWCHAR pwszRegPath ,
	__in PWCHAR pwszValueName,
	__inout PVOID Buffer,
	__in ULONG Length,
	__out PULONG puType, 
	__out PULONG puReturnLength
	)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING uniRegPath;
	OBJECT_ATTRIBUTES ObjectAttributes = {0};
	HANDLE hKey;

	RtlInitUnicodeString(&uniRegPath, pwszRegPath);

	ObjectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
	ObjectAttributes.ObjectName = &uniRegPath;
	ObjectAttributes.RootDirectory = NULL;
	ObjectAttributes.SecurityQualityOfService = NULL;
	ObjectAttributes.SecurityDescriptor = NULL;
	ObjectAttributes.Attributes = OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE;

	Status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjectAttributes);
	if (STATUS_SUCCESS != Status)
	{
		Status = STATUS_UNSUCCESSFUL;
		goto Exit0;
	}

	Status = NIAPGetRegKeyValue(
		hKey,
		pwszValueName,
		sizeof(pwszValueName),
		Buffer,
		Length,
		puType,
		puReturnLength
		);

	ZwClose(hKey);
Exit0:
	return Status;
}
//-----------------------------------------------------------------------------//