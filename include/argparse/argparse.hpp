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
    }// namespace utils

    enum class ArgTypes { STRING = 0, INT, BOOL };

    struct Arg
    {
      public:
        std::string name;
        ArgTypes    type;
        bool        required;

      public:
        friend bool operator==(const Arg &rhs, const Arg &lhs) { return rhs.name == lhs.name; }
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
        ArgumentParser(const int argc, const char **argv) : program_args(argv, argv + argc){};

        [[nodiscard]] container_type args() const noexcept { return this->program_args; }

        void add_argument(
          const std::string &name,
          const ArgTypes    &arg_type = ArgTypes::STRING,
          const bool         required = false)
        {
            const Arg arg_to_insert{ .name = name, .type = arg_type, .required = required };
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
        }

        [[nodiscard]] map_type parse_args()
        {
            for (const auto &[key, val] : this->mapped_args) {
                auto elem_in_argv = std::find(this->program_args.begin(), this->program_args.end(), key.name);

                if (elem_in_argv == this->program_args.end() && key.required == true) {
                    std::cerr << "Arg " << std::quoted(key.name) << " is required\n";
                    exit(1);
                }

                if (elem_in_argv != this->program_args.end()) {
                    if (key.type == ArgTypes::BOOL) {
                        this->mapped_args[key] = Value{ "true" };
                        continue;
                    }
                    this->mapped_args[key] = Value{ *std::next(elem_in_argv) };
                } else if (elem_in_argv == this->program_args.end()) {
                    if (key.type == ArgTypes::BOOL) {
                        this->mapped_args[key] = Value{ "false" };
                        continue;
                    }
                }
            }
            return this->mapped_args;
        }

      private:
        container_type program_args;
        map_type       mapped_args;
    };

}// namespace argparse
#endif// ARGPARSE_HPP
