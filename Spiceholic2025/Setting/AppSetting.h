#pragma once

namespace Spiceholic
{
	class AppSetting
	{
	public:
		struct Settings
		{
			String title;
			String version;
			int dummy;
		};

		AppSetting();

		void load();

		const Settings& get() const;

		Settings& get();

	private:
		Settings settings_;
	};
}
