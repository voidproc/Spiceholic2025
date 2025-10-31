#pragma once

#include "Scene/LogoScene.h"
#include "Scene/TitleScene.h"
#include "Scene/MainScene.h"
#include "Scene/OptionScene.h"
#include "Scene/InputSettingScene.h"
#include "Scene/RecordsScene.h"

template <class ... Scenes, class App>
void RegisterScenes(App& app) {
	(app.add<Scenes>(Scenes::Name), ...);
}
