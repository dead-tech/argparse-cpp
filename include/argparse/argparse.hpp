#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <algorithm>
#include <cassert>
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

    template<typename T>
    concept StringLike = std::is_convertible_v<T, std::string> || std::is_convertible_v<T, const char *>;

    namespace utils {
        static bool str_to_bool(const std::string &str) { return str == "true" ? true : false; }

        static std::string to_upper(const std::string &str)
        {
            std::string result;
            result.reserve(str.size());
            std::transform(str.begin(), str.end(), std::back_inserter(result), [](char c) { return std::toupper(c); });
            return result;
        }
    } // namespace utils

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

    struct ArgNames
    {
        std::vector<std::string> aliases;
        std::string              primary_name;
    };

    struct Arg
    {
      private:
        using flag_underlying_type = std::underlying_type<ArgFlags>::type;

      public:
        ArgNames    names;
        ArgTypes    type  = ArgTypes::STRING;
        ArgFlags    flags = ArgFlags::DEFAULT;
        std::string help_message;
        std::string value;

      public:
        Arg() = default;

        explicit Arg(ArgNames names) : names{ std::move(names) } {};

      public:
        friend bool operator==(const Arg &rhs, const Arg &lhs)
        {
            return rhs.names.primary_name == lhs.names.primary_name;
        }

        template<typename ReturnType>
        [[nodiscard]] auto as() const
        {
            if constexpr (std::is_same<ReturnType, int>::value) {
                int result{};
                std::from_chars(
                  this->value.data(),
                  this->value.data() + this->value.size(), // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                  result);
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

        [[nodiscard]] bool has_name(const std::string &name) const
        {
            return std::any_of(
              this->names.aliases.begin(), this->names.aliases.end(), [&](const auto elem) { return elem == name; });
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
        // clang-format off
        ArgumentParser(const int argc, const char **argv)
          : program_args(argv, argv + argc), program_name{ program_args.at(0) } {};// NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        // clang-format on

        [[nodiscard]] container_type args() const noexcept { return this->program_args; }

        template<StringLike... Names>
        mapped_type &add_argument(Names &&...names)
        {
            static_assert(
              sizeof...(Names) > 0,
              "[argparse] error: add_argument() needs at least one argument as a name (starting with '--')");

            const std::vector<std::string> data{ names... }; // NOLINT

            const auto primary_name =
              std::find_if(data.begin(), data.end(), [](const auto elem) { return elem.starts_with("--"); });

            assert(
                primary_name != data.end() &&
                "[argparse] error: add_argument() requires at least one argument name starting with '--' for non "
                "positional arguments");

            const auto ill_formed_name =
              std::find_if(data.begin(), data.end(), [](const auto elem) { return !elem.starts_with("-"); });

            assert(ill_formed_name == data.end() &&
                "[argparse] error: add_argument() requires all argument names to start with '-' or '--', respectively, for short or long "
                "versions for non"
                "positional arguments");

            // check if another key with that name already existed
            assert(
              this->mapped_args.find(*primary_name) == this->mapped_args.end()
              && "[argparse] error: add_argument() duplicate flag name!\n");

            const auto [it, success] = this->mapped_args.emplace(
              *primary_name, Arg{ ArgNames{ .aliases = data, .primary_name = *primary_name } });
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
                const auto it =
                  std::find_if(this->program_args.begin(), this->program_args.end(), [&](const auto elem) {
                      return arg.has_name(elem);
                  });
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

        void print_help() const { std::cout << this->help_message << '\n'; }

        void print_usage() const { std::cout << this->usage_message << '\n'; }

        [[nodiscard]] const std::string &get_help_message() const { return this->help_message; }

        [[nodiscard]] const std::string &get_usage_message() const { return this->usage_message; }

      private:
        [[nodiscard]] std::string format_as_optional(const std::string &arg_name) { return "[" + arg_name + "]"; }

        void error_required_arg(const std::string &arg_name) const
        {
            std::cerr << "[argparse] error: arg " << std::quoted(arg_name) << " is required\n";
            this->print_usage();
        }

        void create_usage_message()
        {
            this->usage_message = "usage: " + this->program_name + " [--help] ";
            for (const auto &[arg_name, arg] : this->mapped_args) {
                this->usage_message += (arg.has_flag(ArgFlags::REQUIRED) ? arg_name : format_as_optional(arg_name));
                this->usage_message += ' ';
            }

            this->usage_message.append("\n\n");
        }

        void create_help_message()
        {
            std::stringstream ss;

            ss << this->usage_message;

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
        container_type program_args;
        std::string    program_name;
        map_type       mapped_args;
        std::string    usage_message;
        std::string    help_message;
    };

} // namespace argparse
#endif // ARGPARSE_HPP
