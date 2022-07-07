#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <algorithm>
#include <assert.h>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace argparse {

    namespace utils {
        static bool str_to_bool(const std::string &str) { return str == "true" ? true : false; }

        static std::string to_upper(const std::string &str)
        {
            std::string result;
            result.reserve(str.size());
            std::transform(str.begin(), str.end(), std::back_inserter(result), [](char c) { return std::toupper(c); });
            return result;
        }
    }// namespace utils

    enum class ArgTypes
    {
        STRING = 0,
        INT,
        BOOL
    };

    enum class ArgFlags : int64_t
    {
        NONE        = 0,
        REQUIRED    = (1LL << 1),
        STORE_TRUE  = (1LL << 2),
        STORE_FALSE = (1LL << 3),

        DEFAULT = STORE_TRUE,
    };

    struct Arg
    {
      private:
        using flag_underlying_type = std::underlying_type<ArgFlags>::type;

      public:
        std::string name;
        ArgTypes    type;
        ArgFlags    flags;
        std::string help_message;
        std::string value;

      public:
        Arg() = default;

      public:
        friend bool operator==(const Arg &rhs, const Arg &lhs) { return rhs.name == lhs.name; }

        template<typename ReturnType>
        [[nodiscard]] auto as() const
        {
            if constexpr (std::is_same<ReturnType, int>::value) {
                int result{};
                std::from_chars(this->value.data(), this->value.data() + this->value.size(), result);
                return result;
            } else if constexpr (std::is_same<ReturnType, std::string>::value) {
                return value;
            } else if constexpr (std::is_same<ReturnType, bool>::value) {
                return utils::str_to_bool(this->value);
            }

            assert(false && "unreachable");
        }

        [[nodiscard]] bool has_flag(const ArgFlags &flag) const
        {
            return (static_cast<flag_underlying_type>(this->flags) & static_cast<flag_underlying_type>(flag));
        }

        Arg &set_type(const ArgTypes &type)
        {
            this->type = type;
            return *this;
        }

        Arg &set_flags(const ArgFlags &flags)
        {
            this->flags = flags;
            return *this;
        }

        Arg &set_help(const std::string &help_message)
        {
            this->help_message = help_message;
            return *this;
        }
    };

    class ArgumentParser
    {
      private:
        using container_type = std::vector<std::string>;
        using map_type       = std::unordered_map<std::string, Arg>;
        using key_type       = map_type::key_type;
        using mapped_type    = map_type::mapped_type;

      public:
        // TODO: Remove argc use argv.size()
        ArgumentParser(const int argc, const char **argv)
          : program_args(argv, argv + argc), program_name{ program_args.at(0) } {};

        [[nodiscard]] container_type args() const noexcept { return this->program_args; }

        mapped_type &add_argument(const key_type &arg_name)
        {
            // TODO: check if another key with that name already existed
            const Arg to_insert      = { .name         = arg_name,
                                         .type         = ArgTypes::STRING,
                                         .flags        = ArgFlags::DEFAULT,
                                         .help_message = "",
                                         .value        = "" };
            const auto [it, success] = this->mapped_args.emplace(arg_name, to_insert);
            return it->second;
        }

        [[nodiscard]] map_type parse_args()
        {
            this->create_usage_message();
            this->create_help_message();

            // Check if help flag is specified
            const auto help_flag = std::find(this->program_args.begin(), this->program_args.end(), "--help");
            if (help_flag != this->program_args.end()) {
                this->print_help();
                return {};
            }

            for (auto &[arg_name, arg] : this->mapped_args) {
                const auto it        = std::find(this->program_args.begin(), this->program_args.end(), arg_name);
                const auto arg_found = it != this->program_args.end();

                if (!arg_found && arg.has_flag(ArgFlags::REQUIRED)) {
                    this->error_required_arg(arg_name);
                    return {};
                }

                if (arg_found) {
                    if (arg.type == ArgTypes::BOOL) {
                        arg.value = arg.has_flag(ArgFlags::STORE_FALSE) ? arg.value = "false" : arg.value = "true";
                        this->mapped_args[arg_name] = arg;
                        continue;
                    }
                    arg.value                   = *std::next(it);
                    this->mapped_args[arg_name] = arg;
                } else if (!arg_found) {
                    if (arg.type == ArgTypes::BOOL) {
                        arg.value = arg.has_flag(ArgFlags::STORE_FALSE) ? arg.value = "true" : arg.value = "false";
                        this->mapped_args[arg_name] = arg;
                        continue;
                    }
                }
            }

            return this->mapped_args;
        }

        void print_help()
        {
            if (!this->help_message.has_value()) { this->create_help_message(); }
            std::cout << this->help_message.value() << '\n';
        }

        void print_usage()
        {
            if (!this->usage_message.has_value()) { this->create_usage_message(); }
            std::cout << this->usage_message.value() << '\n';
        }

        const std::string &get_help_message()
        {
            if (!this->help_message.has_value()) { this->create_help_message(); }
            return this->help_message.value();
        }

        const std::string &get_usage_message()
        {
            if (!this->usage_message.has_value()) { this->create_usage_message(); }
            return this->usage_message.value();
        }

      private:
        std::string format_as_optional(const std::string &arg_name) { return "[" + arg_name + "]"; }

        void error_required_arg(const std::string &arg_name)
        {
            std::cerr << "[argparse] error: arg " << std::quoted(arg_name) << " is required\n";
            this->print_usage();
        }

        void create_usage_message()
        {
            this->usage_message = "usage: " + this->program_name + " [--help] ";
            for (const auto &[arg_name, arg] : this->mapped_args) {
                *this->usage_message += (arg.has_flag(ArgFlags::REQUIRED) ? arg_name : format_as_optional(arg_name));
                *this->usage_message += ' ';
            }

            this->usage_message.value().append("\n\n");
        }

        void create_help_message()
        {
            std::stringstream ss;

            ss << this->usage_message.value();

            ss << "optional arguments:\n";
            ss << "  --help\t\tshow this help message and exit\n";

            for (const auto &[arg_name, arg] : this->mapped_args) {
                const std::string message = "  " + this->format_as_optional(arg_name) + ' ' + utils::to_upper(arg_name)
                                            + ' ' + arg.help_message + '\n';
                if (!arg.has_flag(ArgFlags::REQUIRED)) { ss << message; }
            }

            ss << "\n\n";
            ss << "required arguments:\n";

            for (const auto &[arg_name, arg] : this->mapped_args) {
                if (arg.has_flag(ArgFlags::REQUIRED)) {
                    const std::string message =
                      "  " + arg_name + ' ' + utils::to_upper(arg_name) + ' ' + arg.help_message + '\n';
                    ss << message;
                }
            }

            this->help_message = ss.str();
        }


      private:
        container_type             program_args;
        std::string                program_name;
        map_type                   mapped_args;
        std::optional<std::string> usage_message;
        std::optional<std::string> help_message;
    };

}// namespace argparse
#endif// ARGPARSE_HPP
