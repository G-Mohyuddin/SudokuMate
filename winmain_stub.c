#ifdef _WIN32
#include <windows.h>

int main(void);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd) {
	(void)hInst; (void)hPrev; (void)lpCmdLine; (void)nShowCmd;
	return main();
}
#endif



