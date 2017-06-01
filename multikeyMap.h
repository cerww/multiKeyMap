#pragma once
#include <unordered_map>
#include <tuple>
#include <type_traits>

template<typename val,typename... Args>
class multiKeyMap {
public:
	multiKeyMap() = default;
	val& get(Args... args) {
		return m_data[std::make_tuple(std::forward<Args>(args)...)];
	};
	auto& set(val first, Args... args) {
		return m_data.insert(std::make_pair(std::make_tuple(std::forward<Args>(args)...),std::move(first)));
	};
private:
	class tupleHash {
		public:
		std::size_t operator()(const std::tuple<Args...>& thing) const{
			return std::hash<std::size_t>()(worky<0>()(thing));
		};
		private:
		template<int i>
		struct worky{
			std::size_t operator()(const std::tuple<Args...>& things)const {
				return std::hash<std::remove_const<std::remove_reference<decltype(std::get<i>(things))>::type >::type >()(std::get<i>(things)) ^ worky<i + 1>()(things);
			}
		};

		template<>
		struct worky<sizeof...(Args)> {
			std::size_t operator()(const std::tuple<Args...>& things)const {
				return -1;
			}
		};
	};
	std::unordered_map<std::tuple<Args...>, val,tupleHash> m_data;
};

