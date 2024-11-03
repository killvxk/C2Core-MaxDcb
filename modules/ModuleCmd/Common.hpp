#pragma once

#include <string>
#include <vector>
#include <random>
#include <string_view>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202302L) || __cplusplus >= 202302L)
    #include <format>
#endif


#define SPDLOG_LEVEL_TRACE 0
#define SPDLOG_LEVEL_DEBUG 1
#define SPDLOG_LEVEL_INFO 2
#define SPDLOG_LEVEL_WARN 3
#define SPDLOG_LEVEL_ERROR 4
#define SPDLOG_LEVEL_CRITICAL 5
#define SPDLOG_LEVEL_OFF 6

#if (defined(BUILD_TEAMSERVER) || defined(BUILD_TESTS)) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 202302L) || __cplusplus >= 202302L)

    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO

    #if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
        template<typename... Args>
        inline void SPDLOG_TRACE(const std::format_string<Args...> fmt, Args&&... args)
        {
            std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
        }
    #else
        #define SPDLOG_TRACE(...) (void)0
    #endif

    #if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
        template<typename... Args>
        inline void SPDLOG_DEBUG(const std::format_string<Args...> fmt, Args&&... args)
        {
            std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
        }
    #else
        #define SPDLOG_DEBUG(...) (void)0
    #endif

    #if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
        template<typename... Args>
        inline void SPDLOG_INFO(const std::format_string<Args...> fmt, Args&&... args)
        {
            std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
        }
    #else
        #define SPDLOG_INFO(...) (void)0
    #endif

    #if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
        template<typename... Args>
        inline void SPDLOG_WARN(const std::format_string<Args...> fmt, Args&&... args)
        {
            std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
        }
    #else
        #define SPDLOG_WARN(...) (void)0
    #endif

    #if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
        template<typename... Args>
        inline void SPDLOG_ERROR(const std::format_string<Args...> fmt, Args&&... args)
        {
            std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
        }
    #else
        #define SPDLOG_ERROR(...) (void)0
    #endif

    #if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
        template<typename... Args>
        inline void SPDLOG_CRITICAL(const std::format_string<Args...> fmt, Args&&... args)
        {
            std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
        }
    #else
        #define SPDLOG_CRITICAL(...) (void)0
    #endif

#else

    #define SPDLOG_TRACE(...) (void)0
    #define SPDLOG_DEBUG(...) (void)0
    #define SPDLOG_INFO(...) (void)0
    #define SPDLOG_WARN(...) (void)0
    #define SPDLOG_ERROR(...) (void)0
    #define SPDLOG_CRITICAL(...) (void)0

#endif

const int SizeListenerHash = 32;
const int SizeBeaconHash = 32;


constexpr unsigned long djb2(std::string_view str, unsigned long hash = 5381, std::size_t index = 0) 
{
    return (index == str.size()) ? hash : djb2(str, ((hash << 5) + hash) + str[index], index + 1);
}

template<std::size_t N, std::size_t M>
inline constexpr std::array<char, N> compileTimeXOR(const std::string_view data, const std::string_view key) 
{
    std::array<char, N> result{};
    std::size_t key_size = key.size();
    std::size_t j = 0;

    for (std::size_t i = 0; i < data.size(); ++i) {
        if (j == key_size) {
            j = 0;
        }

        result[i] = data[i] ^ key[j];
        ++j;
    }

    return result;
}


void static inline XOR(std::string& data, const std::string& key) 
{
	int j = 0;
	for (int i = 0; i < data.size(); i++) 
	{
		if (j == key.size()) 
			j = 0;

		data[i] = data[i] ^ key[j];
		j++;
	}
}


std::string static inline random_string(std::size_t length)
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;
    for (std::size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}


bool static inline isNumber(const std::string& str)
{
	for (char const& c : str) {
		if (std::isdigit(c) == 0) return false;
	}
	return true;
}


void static inline splitList(std::string list, const std::string& delimiter, std::vector<std::string>& splitedList)
{
	size_t pos = 0;
	std::string token;
	while ((pos = list.find(delimiter)) != std::string::npos)
	{
		token = list.substr(0, pos);
		splitedList.push_back(token);
		list.erase(0, pos + delimiter.length());
	}
	splitedList.push_back(list);
}
