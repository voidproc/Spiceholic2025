#pragma once

#include "Scene/TitleScene.h"
#include "Scene/MainScene.h"

template <class ... Scenes, class App>
void RegisterScenes(App& app) {
	(app.add<Scenes>(Scenes::Name), ...);
}
