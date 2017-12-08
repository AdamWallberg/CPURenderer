#include <iostream>
#include "Core.h"
#include "Engine.h"

int main()
{
#ifdef DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Engine* engine = Engine::createInstance();

	while (engine->update());

	Engine::destroyInstance();

	return 0;
}