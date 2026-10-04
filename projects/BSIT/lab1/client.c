#include <handleapi.h>
#include <ioapiset.h>
#include <psdk_inc/_socket_types.h>
#include <winnt.h>

int main()
{
	HANDLE io_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	SOCKET s;
	CreateIoCompletionPort((HANDLE)s, io_port, 0, 0);

	return 0;
}
