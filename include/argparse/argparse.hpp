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
        static std::optional<bool> str_to_bool(const std::string &str)
        {
            if (str == "false") {
                return false;
            } else if (str == "true") {
                return true;
            }

            return std::nullopt;
        }

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
        NONE     = 0LL,
        REQUIRED = (1LL << 0),
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

      public:
        friend bool operator==(const Arg &rhs, const Arg &lhs) { return rhs.name == lhs.name; }

        [[nodiscard]] bool has_flag(const ArgFlags &flag) const
        {
            return (static_cast<flag_underlying_type>(this->flags) & static_cast<flag_underlying_type>(flag));
        }
    };

    class Value
    {
      public:
        std::string str;

      public:
        Value() = default;

        template<typename ReturnType>
        auto as()
        {
            if constexpr (std::is_same<ReturnType, int>::value) {
                int result{};
                std::from_chars(this->str.data(), this->str.data() + this->str.size(), result);
                return result;
            } else if constexpr (std::is_same<ReturnType, std::string>::value) {
                return str;
            } else if constexpr (std::is_same<ReturnType, bool>::value) {
                // TODO: Error handling here
                return *utils::str_to_bool(this->str);
            }
        }
    };

    namespace impl {
        class ArgHashFunction
        {
          public:
            size_t operator()(const argparse::Arg &arg) const noexcept { return std::hash<std::string>{}(arg.name); }
        };

        class ArgMap : public std::unordered_map<Arg, Value, ArgHashFunction>
        {
          public:
            Value at(const std::string &name) const
            {
                const auto it = this->find(Arg{ name });

                if (it == this->end()) { throw std::out_of_range("Invalid key"); }

                return it->second;
            }
        };
    }// namespace impl

    class ArgumentParser
    {
      private:
        using container_type = std::vector<std::string>;
        using value_type     = Value;
        using map_type       = impl::ArgMap;

      public:
        ArgumentParser(const int argc, const char **argv)
          : program_args(argv, argv + argc), program_name{ program_args.at(0) } {};

        [[nodiscard]] container_type args() const noexcept { return this->program_args; }

        void add_argument(
          const std::string &name,
          const ArgTypes    &arg_type     = ArgTypes::STRING,
          const ArgFlags    &flags        = ArgFlags::NONE,
          const std::string &help_message = "")
        {
            const Arg arg_to_insert{ .name = name, .type = arg_type, .flags = flags, .help_message = help_message };
            this->add_argument(arg_to_insert);
        }

        void add_argument(const Arg &arg)
        {
            switch (arg.type) {
                case ArgTypes::STRING: {
                    this->mapped_args[arg] = Value{ "" };
                    break;
                }
                case ArgTypes::INT: {
                    this->mapped_args[arg] = Value{ "0" };
                    break;
                }
                case ArgTypes::BOOL: {
                    this->mapped_args[arg] = Value{ "false" };
                    break;
                }
            }

            this->create_usage_message();
            this->create_help_message();
        }

        [[nodiscard]] map_type parse_args()
        {
            // Check if help flag is specified
            const auto help_flag = std::find(this->program_args.begin(), this->program_args.end(), "--help");
            if (help_flag != this->program_args.end()) {
                this->print_help();
                return {};
            }

            for (const auto &[arg, val] : this->mapped_args) {
                const auto it        = std::find(this->program_args.begin(), this->program_args.end(), arg.name);
                const auto arg_found = it != this->program_args.end();

                if (!arg_found && arg.has_flag(ArgFlags::REQUIRED)) {
                    this->error_required_arg(arg);
                    return {};
                }

                if (arg_found) {
                    if (arg.type == ArgTypes::BOOL) {
                        this->mapped_args[arg] = Value{ "true" };
                        continue;
                    }
                    this->mapped_args[arg] = Value{ *std::next(it) };
                } else if (!arg_found) {
                    if (arg.type == ArgTypes::BOOL) {
                        this->mapped_args[arg] = Value{ "false" };
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
        std::string format_as_optional(const std::string &key_name) { return "[" + key_name + "]"; }

        void error_required_arg(const Arg &arg)
        {
            std::cerr << "[argparse] error: arg " << std::quoted(arg.name) << " is required\n";
            this->print_usage();
        }

        void create_usage_message()
        {
            this->usage_message = "usage: " + this->program_name + " [--help] ";
            for (const auto &[key, val] : this->mapped_args) {
                *this->usage_message += (key.has_flag(ArgFlags::REQUIRED) ? key.name : format_as_optional(key.name));
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

            for (const auto &[key, value] : this->mapped_args) {
                const std::string message = "  " + this->format_as_optional(key.name) + ' ' + utils::to_upper(key.name)
                                            + ' ' + key.help_message + '\n';
                if (!key.has_flag(ArgFlags::REQUIRED)) { ss << message; }
            }

            ss << "\n\n";
            ss << "required arguments:\n";

            for (const auto &[key, value] : this->mapped_args) {
                if (key.has_flag(ArgFlags::REQUIRED)) {
                    const std::string message =
                      "  " + key.name + ' ' + utils::to_upper(key.name) + ' ' + key.help_message + '\n';
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
