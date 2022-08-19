#include"render.h"
using namespace std;

int main()
{
    Render render;
    // render loop

    while (!glfwWindowShouldClose(render.window))
    {
        render.draw();
    }
	return 0;
}