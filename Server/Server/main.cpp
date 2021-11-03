#include "server.h"

int main()
{
	try {
		Server server;
		server.Update();
	}
	catch (Exception& ex)
	{
		cout << ex.what() << endl;
	}
}