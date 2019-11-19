#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "../../nclgl/light.h"
#include "Renderer.h"

int main() {
	Window w("Cube Mapping! sky textures courtesy of http://www.hazelwhorley.com", 800, 600, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	Light* l = renderer.getLight();

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		Vector3 pos = l->GetPosition();
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_R)) {
			renderer.ResetTime();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_T)) {
			renderer.loadShaders();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
			pos.z -= 5;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
			pos.z += 5;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
			pos.x -= 5;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			pos.x += 5;
		}
		l->SetPosition(pos);
	}

	return 0;
}