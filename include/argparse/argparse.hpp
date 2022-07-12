#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <algorithm>
#include <cassert>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace argparse {

    template<typename T>
    concept StringLike = std::is_convertible_v<T, std::string>;

    template<typename T>
    concept SupportedArgumentType = std::is_integral_v<T> || std::is_convertible_v<T, std::string>;

    namespace utils {
        [[nodiscard]] static bool str_to_bool(const std::string &str) noexcept { return str == "true" ? true : false; }

        [[nodiscard]] static std::string bool_to_str(const bool boolean) noexcept { return boolean ? "true" : "false"; }

        [[nodiscard]] static std::string to_upper(const std::string &str)
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
        bool        has_default_value = false;

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

        template<SupportedArgumentType T>
        Arg &set_default(T &&value)
        {
            if constexpr (std::is_convertible_v<T, std::string>) {
                this->value = std::forward<T>(value); // NOLINT
            } else if constexpr (std::is_same_v<T, bool>) {
                this->value = utils::bool_to_str(std::forward<T>(value));
            } else {
                this->value = std::to_string(std::forward<T>(value));
            }

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
        using builtins_type  = std::unordered_map<std::string, std::function<void()>>;

      public:
        // clang-format off
        ArgumentParser(const int argc, const char **argv, std::string version = "0.0.1")
          : program_args(argv, argv + argc), program_name{ program_args.at(0) }, version { std::move(version) } {}; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        // clang-format on

        [[nodiscard]] container_type args() const noexcept { return this->program_args; }

        template<StringLike... Names>
        mapped_type &add_argument(Names &&...names)
        {
            static_assert(
              sizeof...(Names) > 0,
              "[argparse] error: add_argument() needs at least one argument as a name (starting with '--' for "
              "positional arguments)");

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

            // check if the names collide with some builtin
            std::for_each(data.begin(), data.end(), [&](const auto &name) {
                assert(
                  !this->builtins.contains(name)
                  && "[argparse] error: add_argument() cannot add an argument with the same name as a builtin");
            });

            // check if another key with that name already existed
            std::for_each(this->mapped_args.begin(), this->mapped_args.end(), [&](const auto &pair) {
                assert(
                  pair.first != *primary_name && pair.second.names.aliases != data
                  && "[argparse] error: add_argument() cannot add an argument with the same name as another argument");
            });

            const auto [it, success] = this->mapped_args.emplace(
              *primary_name, Arg{ ArgNames{ .aliases = data, .primary_name = *primary_name } });
            return it->second;
        }

        [[nodiscard]] map_type parse_args()
        {
            this->create_usage_message();
            this->create_help_message();

            if (const auto builtin = this->get_builtin_if(); builtin.has_value()) {
                const auto fn = builtin.value();
                fn();
                return {};
            }

            std::for_each(this->program_args.begin(), this->program_args.end(), [&](const auto &arg) {
                assert(
                  arg.starts_with('-') && !this->mapped_args.contains(arg)
                  && "[argparse] error: unrecognized argument");
            });


            for (auto &[arg_name, arg] : this->mapped_args) {
                const auto it =
                  std::find_if(this->program_args.begin(), this->program_args.end(), [&](const auto elem) {
                      return arg.has_name(elem);
                  });
                const auto arg_found = it != this->program_args.end();

                if (!arg_found && arg.has_flag(ArgFlags::REQUIRED) && !arg.has_default_value) {
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

        void print_version() const { std::cout << this->version << '\n'; }

        [[nodiscard]] const std::string &get_help_message() const { return this->help_message; }

        [[nodiscard]] const std::string &get_usage_message() const { return this->usage_message; }

        [[nodiscard]] const std::string &get_version() const { return this->version; }

      private:
        [[nodiscard]] std::string format_as_optional(const std::string &arg_name) { return "[" + arg_name + "]"; }

        [[nodiscard]] std::string format_argument_names(const ArgNames &names)
        {
            return std::accumulate(
              std::next(names.aliases.begin()),
              names.aliases.end(),
              names.aliases[0],
              [](const auto &a, const auto &b) { return a + ", " + b; });
        }

        void error_required_arg(const std::string &arg_name) const
        {
            std::cerr << "[argparse] error: arg " << std::quoted(arg_name) << " is required\n";
            this->print_usage();
        }

        void create_usage_message()
        {
            this->usage_message = "usage: " + this->program_name + " [-H] ";
            for (const auto &[arg_name, arg] : this->mapped_args) {
                this->usage_message +=
                  (arg.has_flag(ArgFlags::REQUIRED) ? arg_name + ' ' + utils::to_upper(arg_name)
                                                    : format_as_optional(arg_name + ' ' + utils::to_upper(arg_name)));
                this->usage_message += ' ';
            }

            this->usage_message.append("\n\n");
        }

        void create_help_message()
        {
            std::stringstream optional_ss;
            std::stringstream required_ss;

            optional_ss << "optional arguments:\n";
            optional_ss << "  -H, --help\t\tshow this help message and exit\n";

            required_ss << "required arguments:\n";

            for (const auto &[arg_name, arg] : this->mapped_args) {
                const std::string message = "  " + this->format_argument_names(arg.names) + ' '
                                            + utils::to_upper(arg_name) + ' ' + arg.help_message + '\n';
                if (!arg.has_flag(ArgFlags::REQUIRED)) {
                    optional_ss << message;
                } else {
                    required_ss << message;
                }
            }

            this->help_message = this->usage_message + required_ss.str() + "\n" + optional_ss.str();
        }

        auto get_builtin_if() const -> std::optional<builtins_type::mapped_type>
        {
            using return_type = std::optional<builtins_type::mapped_type>;

            for (const auto &[name, fn] : this->builtins) {
                const auto found = std::find_if(
                  this->program_args.begin(), this->program_args.end(), [&](const auto elem) { return elem == name; });

                if (found != this->program_args.end()) { return return_type{ fn }; }
            }

            return return_type{ std::nullopt };
        }


      private:
        container_type program_args;
        std::string    program_name;
        map_type       mapped_args;
        std::string    usage_message;
        std::string    help_message;
        std::string    version;

        builtins_type builtins = {
            { "--help", [this]() { this->print_help(); } },
            { "-H", [this]() { this->print_help(); } },
            { "--version", [this]() { this->print_version(); } },
            { "-V", [this]() { this->print_version(); } },
        };
    };

} // namespace argparse
#endif // ARGPARSE_HPP
