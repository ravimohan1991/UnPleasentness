#include <iostream>
#include <Windows.h>
int main()
{
	while (true)
	{
		MessageBox(GetFocus(), L"Hello world!", L"detours-example", MB_OK);
		std::cout << "Enter to show messagebox:";
		std::cin.get();
		system("cls");
	}
}