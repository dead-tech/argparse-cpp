from conans import ConanFile

class Argparse(ConanFile):
    name = 'argparse-cpp'
    version = '0.1.0'
    description = "Argument Parser in C++ inspired by python's argparse module"
    topics = ('conan', 'argparse', 'argparse-cpp', 'cli')
    url = 'https://github.com/dead-tech/argparse-cpp'
    homepage = url
    license = 'MIT'
    exports_sources = 'include/*'
    no_copy_source = True

    def package(self):
        self.copy('*.hpp')
