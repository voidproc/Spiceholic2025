#pragma once

namespace Spiceholic
{
	class Dispatch
	{
	public:
		// イベントを発行する
		template<typename... Args>
		void publish(Args&&... args)
		{
			m_dispatcher.trigger(std::forward<Args>(args)...);
		}

		// イベントを購読する
		template<typename Event, auto Candidate, typename Type>
		void subscribe(Type* instance)
		{
			m_dispatcher.sink<Event>().connect<Candidate>(instance);
		}

		// イベントの購読を解除する
		template<typename Event, auto Candidate, typename Type>
		void unsubscribe(Type* instance)
		{
			m_dispatcher.sink<Event>().disconnect<Candidate>(instance);
		}

		// イベントの購読を解除する
		template<typename Event, typename Type>
		void unsubscribe(Type* instance)
		{
			m_dispatcher.sink<Event>().disconnect(instance);
		}

	private:
		entt::dispatcher m_dispatcher;
	};

	namespace GlobalDispatch
	{
		void Init();
	}

	Dispatch& GetDispatch();
}
