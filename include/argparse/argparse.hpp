#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>
#include <string_view>
#include <vector>

namespace argparse {
    template<std::size_t ArgCount>
    class ArgumentParser
    {
      private:
        using element_type   = std::string_view;
        using container_type = std::vector<element_type>;


      public:
        explicit ArgumentParser(const char *argv[]) : argv{ container_type(argv, argv + ArgCount) }
        {
            this->program_name = argv[0];
        }

        [[nodiscard]] auto arg_count() const noexcept { return ArgCount; }
        [[nodiscard]] auto args() const noexcept { return this->argv; }

      public:
        element_type program_name;


      private:
        container_type argv;
    };
}// namespace argparse

#endif// ARGPARSE_HPP
