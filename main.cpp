#include"base.h"
#include"application.h"
int main() {
	FF::Application app;

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}