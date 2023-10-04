#include <iostream>
#include<windows.h>
 
using namespace std;
 
class Point
{
public:
	Point()
	{
		// print out the address of this object
		cout << this << endl;
	};
 
	void Print(int num, const char *txt)
	{
		// the control
		if (num == 1)
		{
			cout << txt << endl;
		}
	}
 
private:
};
 
int main()
{
	Point a;
	int i = 0;
	while (true)
	{
		// when you press SPACE it will print "test"
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			i++;
			cout << i << ":";
			a.Print(1, "test");
			cout << &a << endl;	
			Sleep(2000);
		}
	}
	return 0;
};