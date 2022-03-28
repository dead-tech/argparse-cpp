#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace argparse {

    enum class ArgumentType { STRING = 0, INT, BOOL };

    struct Value
    {
      private:
        using variant_type = std::variant<std::string, int, bool>;

      public:
        explicit Value(variant_type value) : variant{ std::move(value) } {}

        template<typename VariantType>
        [[nodiscard]] auto as() const
        {
            return std::get<VariantType>(this->variant);
        }

      private:
        variant_type variant;
    };

    template<std::size_t ArgCount>
    class ArgumentParser
    {
      private:
        using element_type   = std::string;
        using container_type = std::vector<element_type>;
        using map_type       = std::unordered_map<element_type, Value>;

      public:
        explicit ArgumentParser(const char *argv[]) : argv{ container_type(argv, argv + ArgCount) }
        {
            this->program_name = argv[0];
        }

        [[nodiscard]] auto operator[](const std::string &key) { return this->parsed_args.at(key); }

        [[nodiscard]] auto arg_count() const noexcept { return ArgCount; }
        [[nodiscard]] auto args() const noexcept { return this->argv; }

        void add_argument(const std::string &argument_name, const ArgumentType &argument_type)
        {
            switch (argument_type) {
                case ArgumentType::STRING:
                    this->parsed_args.emplace(argument_name, Value(""));
                    break;
                case ArgumentType::INT:
                    this->parsed_args.emplace(argument_name, Value(0));
                    break;
                case ArgumentType::BOOL:
                    this->parsed_args.emplace(argument_name, Value(false));
                    break;
            }
        }

        [[nodiscard]] auto parse_args() const {}

      public:
        element_type program_name;

      private:
        container_type argv;
        map_type       parsed_args;
    };
}// namespace argparse

#endif// ARGPARSE_HPP
