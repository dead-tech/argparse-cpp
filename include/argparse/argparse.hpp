#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <algorithm>
#include <cassert>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace argparse {

    namespace utils {
        template<typename T>
        concept StringLike = std::is_convertible_v<T, std::string>;

        template<typename T>
        concept Streamable = requires(std::ostream &os, const T &value)
        {
            // clang-format off
            { os << value } -> std::convertible_to<std::ostream &>;
            // clang-format on
        };

        [[nodiscard]] static bool str_to_bool(const std::string &str) noexcept { return str == "true" ? true : false; }

        [[nodiscard]] static std::string bool_to_str(const bool boolean) noexcept { return boolean ? "true" : "false"; }

        [[nodiscard]] static std::string to_upper(const std::string &str)
        {
            std::string result;
            result.reserve(str.size());
            std::transform(str.begin(), str.end(), std::back_inserter(result), [](char c) { return std::toupper(c); });
            return result;
        }

        template<Streamable... Args>
        [[nodiscard]] static constexpr auto format(std::string_view fmt, Args &&...args) -> std::string
        {
            assert(
              std::count(fmt.begin(), fmt.end(), '%') == sizeof...(Args)
              && "format() error: wrong number of arguments");

            std::stringstream result;

            const auto format_helper = [&](const auto &value) {
                const auto placeholder = fmt.find('%');
                result << fmt.substr(0, placeholder);
                result << value;
                fmt = fmt.substr(placeholder + 1, fmt.size() - 1);
            };

            (format_helper(std::forward<Args>(args)), ...);
            result << fmt;
            return result.str();
        }
    } // namespace utils

    namespace exceptions {
        class ArgparseException : public std::runtime_error
        {
          public:
            template<utils::Streamable... Args>
            ArgparseException(const std::source_location &location, const std::string_view fmt, Args &&...args)
              : std::runtime_error("")
            {
                const auto expanded_message = utils::format(fmt, std::forward<Args>(args)...);
                this->message               = utils::format(
                  "[argparse] error in file: %(%:%) in function `%` -> %\n",
                  location.file_name(),
                  location.line(),
                  location.column(),
                  location.function_name(),
                  expanded_message);
            }

            [[nodiscard]] const char *what() const noexcept override { return this->message.c_str(); }

          private:
            std::string message;
        };
    } // namespace exceptions

    template<typename T>
    concept SupportedArgumentType = std::is_integral_v<T> || std::is_convertible_v<T, std::string>;

    enum class ArgKind
    {
        Positional,
        Optional,
    };

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
        ArgNames                   names;
        ArgTypes                   type  = ArgTypes::STRING;
        ArgFlags                   flags = ArgFlags::DEFAULT;
        std::string                help_message;
        std::string                value;
        bool                       has_default_value = false;
        std::optional<std::string> metavar;
        size_t                     position = -1; // -1 means is not positional

      public:
        Arg() = default;

        Arg(ArgNames names, const ArgFlags required = ArgFlags::DEFAULT)
          : names{ std::move(names) }, flags{ required } {};

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

        Arg &set_metavar(const std::string &metavar)
        {
            this->metavar = metavar;
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

        template<utils::StringLike... Names>
        mapped_type &add_argument(Names &&...names)
        {
            static_assert(
              sizeof...(Names) > 0,
              "[argparse] error: add_argument() needs at least one argument as a name (starting with '--' for "
              "positional arguments)");

            const std::vector<std::string> data{ names... }; // NOLINT

            // Checks that the argument has at least a name starting with a -- (a primary name)
            // Also returns based on the presence of the primary name the type of argument we are adding
            const auto [arg_kind, primary_name] = this->get_primary_name(data);

            if (arg_kind == ArgKind::Positional) {
                return this->add_positional_argument(data);
            } else if (arg_kind == ArgKind::Optional) {
                return this->add_optional_argument(data, primary_name.value());
            }

            assert(false && "unreachable");
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

            const auto [positional_args, optional_args] = this->split_program_args();

            this->parse_positional_args(positional_args);

            // Throws if a not registered optional argument is found in the program args
            this->throw_if_unrecognized(optional_args);
            this->parse_optional_args(optional_args);

            return this->mapped_args;
        }

        void print_help() const { std::cout << this->help_message << '\n'; }

        void print_usage() const { std::cout << this->usage_message << '\n'; }

        void print_version() const { std::cout << this->version << '\n'; }

        [[nodiscard]] const std::string &get_help_message() const { return this->help_message; }

        [[nodiscard]] const std::string &get_usage_message() const { return this->usage_message; }

        [[nodiscard]] const std::string &get_version() const { return this->version; }

      private:
        [[nodiscard]] std::string format_as_optional(const std::string &arg_name)
        {
            return utils::format("[%]", arg_name);
        }

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
            std::cerr << utils::format("[argparse] error: arg % is required", std::quoted(arg_name));
            this->print_usage();
        }

        void create_usage_message()
        {
            this->usage_message = utils::format("usage: % [-H] ", this->program_name);
            for (const auto &[arg_name, arg] : this->mapped_args) {
                const auto common = utils::format(
                  "% %", arg_name, !arg.metavar.has_value() ? utils::to_upper(arg_name) : arg.metavar.value());
                this->usage_message += (arg.has_flag(ArgFlags::REQUIRED) ? common : format_as_optional(common));
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
                const std::string message = utils::format(
                  "  % % %\n",
                  this->format_argument_names(arg.names),
                  !arg.metavar.has_value() ? utils::to_upper(arg_name) : arg.metavar.value(),
                  arg.help_message);
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

        auto get_primary_name(const std::vector<std::string> &names) const
          -> std::pair<ArgKind, std::optional<std::string>>
        {
            // TODO: maybe do this more efficiently?
            const auto primary_name =
              std::find_if(names.begin(), names.end(), [](const auto elem) { return elem.starts_with("--"); });

            const auto aliases =
              std::find_if(names.begin(), names.end(), [](const auto elem) { return elem.starts_with("-"); });

            if (primary_name == names.end() && aliases == names.end()) {
                return std::make_pair(ArgKind::Positional, std::nullopt);
            } else if (primary_name == names.end() && aliases != names.end()) {
                throw exceptions::ArgparseException(
                  std::source_location::current(),
                  "[argparse] error: add_argument() needs at least one argument as a name (starting with '--' for non"
                  "positional arguments)");
            }

            return std::make_pair(ArgKind::Optional, *primary_name);
        }

        void verify_well_formed_names(const std::vector<std::string> &names) const
        {
            const auto ill_formed_name =
              std::find_if(names.begin(), names.end(), [](const auto elem) { return !elem.starts_with("-"); });

            if (ill_formed_name != names.end()) {
                throw exceptions::ArgparseException(
                  std::source_location::current(),
                  "[argparse] error: add_argument() requires all argument names to start with '-' or '--', "
                  "respectively, for short or long "
                  "versions for non positional arguments");
            }
        }

        void check_for_duplicate_names(const std::vector<std::string> &names) const
        {
            std::for_each(names.begin(), names.end(), [&](const auto &name) {
                const auto found_hash =
                  std::find(this->hashed_names.begin(), this->hashed_names.end(), std::hash<std::string>{}(name))
                  != this->hashed_names.end();

                if (found_hash) {
                    throw exceptions::ArgparseException(
                      std::source_location::current(),
                      "[argparse] error: add_argument() cannot add an argument with name % as it already exists",
                      std::quoted(name));
                }

                if (this->builtins.contains(name)) {
                    throw exceptions::ArgparseException(
                      std::source_location::current(),
                      "[argparse] error: add_argument() cannot add an argument with name % as it is a builtin",
                      std::quoted(name));
                }
            });
        }

        void throw_if_unrecognized(const std::vector<std::string> &optional_args) const
        {
            std::for_each(optional_args.begin(), optional_args.end(), [&](const auto &arg) {
                const auto found_hash =
                  std::find(this->hashed_names.begin(), this->hashed_names.end(), std::hash<std::string>{}(arg))
                  != this->hashed_names.end();

                if (arg.starts_with('-') && !found_hash) {
                    throw exceptions::ArgparseException(
                      std::source_location::current(), "[argparse] error: unrecognized argument %\n", std::quoted(arg));
                }
            });
        }

        [[nodiscard]] Arg &add_positional_argument(const std::vector<std::string> &names)
        {
            if (names.size() > 1) {
                throw exceptions::ArgparseException(
                  std::source_location::current(), "[argparse] error: positional arguments cannot have aliases");
            }

            this->hashed_names.push_back(std::hash<std::string>{}(names.front()));

            Arg to_insert      = Arg{ ArgNames{ .aliases = names, .primary_name = names.front() }, ArgFlags::REQUIRED };
            to_insert.position = this->num_positional_args++;

            const auto [it, success] = this->mapped_args.emplace(names.front(), to_insert);
            return it->second;
        }

        [[nodiscard]] Arg &add_optional_argument(const std::vector<std::string> &names, const std::string &primary_name)
        {
            // Ensures that the argument names all start with -- or - (not supporting positional arguments right now)
            this->verify_well_formed_names(names);

            // Also makes sure that the argument names do not collide with builtins
            this->check_for_duplicate_names(names);

            // Add the hash of the new names
            // It is required to check for naming collisions
            std::for_each(names.begin(), names.end(), [&](const auto &name) {
                this->hashed_names.push_back(std::hash<std::string>{}(name));
            });

            const auto [it, success] = this->mapped_args.emplace(
              primary_name, Arg{ ArgNames{ .aliases = names, .primary_name = primary_name } });
            return it->second;
        }

        [[nodiscard]] auto split_program_args() const -> std::pair<std::vector<std::string>, std::vector<std::string>>
        {
            const auto it =
              std::find_if(std::next(this->program_args.begin()), this->program_args.end(), [](const auto &elem) {
                  return elem.starts_with('-');
              });

            return std::make_pair(
              std::vector(std::next(this->program_args.begin()), it), std::vector(it, this->program_args.end()));
        }

        void parse_positional_args(const std::vector<std::string> &positional_args)
        {
            if (positional_args.size() != this->num_positional_args) {
                throw exceptions::ArgparseException(
                  std::source_location::current(),
                  "[argparse] error: wrong amount of positional arguments provided, % expected, % were provided",
                  this->num_positional_args,
                  positional_args.size());
            }

            if (positional_args.empty()) { return; }

            size_t current_positional_argument{ 0 };
            std::for_each(positional_args.begin(), positional_args.end(), [&](const auto &value) {
                const auto to_update =
                  std::find_if(this->mapped_args.begin(), this->mapped_args.end(), [&](const auto &pair) {
                      return current_positional_argument == pair.second.position;
                  });
                to_update->second.value = value;
                current_positional_argument++;
            });
        }

        void parse_optional_args(const std::vector<std::string> &optional_args)
        {
            if (optional_args.empty()) { return; }

            for (auto it = optional_args.begin(); it != optional_args.end(); ++it) {
                const auto arg_name = *it;

                const auto to_update =
                  std::find_if(this->mapped_args.begin(), this->mapped_args.end(), [&](const auto &pair) {
                      return pair.second.has_name(arg_name);
                  });
                const auto arg_found = to_update != this->mapped_args.end();

                auto &arg = to_update->second;

                if (!arg_found && arg.has_flag(ArgFlags::REQUIRED) && !arg.has_default_value) {
                    this->error_required_arg(arg_name);
                    return;
                }

                if (arg_found) {
                    if (arg.type == ArgTypes::BOOL) {
                        arg.value = arg.has_flag(ArgFlags::STORE_FALSE) ? arg.value = "false" : arg.value = "true";
                        continue;
                    }
                    arg.value = *std::next(it);
                    ++it;
                } else if (!arg_found) {
                    if (arg.type == ArgTypes::BOOL) {
                        arg.value = arg.has_flag(ArgFlags::STORE_FALSE) ? arg.value = "true" : arg.value = "false";
                        continue;
                    }
                }
            }
        }

      private:
        container_type program_args;
        std::string    program_name;
        map_type       mapped_args;
        std::string    usage_message;
        std::string    help_message;
        std::string    version;

        std::vector<std::size_t> hashed_names = {};

        builtins_type builtins = {
            { "--help", [this]() { this->print_help(); } },
            { "-H", [this]() { this->print_help(); } },
            { "--version", [this]() { this->print_version(); } },
            { "-V", [this]() { this->print_version(); } },
        };

        std::size_t num_positional_args = 0;
    };

} // namespace argparse
#endif // ARGPARSE_HPP
