import os

env = Environment(ENV = os.environ)
env.Append(CPPPATH = ['#include'], CCFLAGS = ['-g'], CPPDEFINES = ['HAVE_PTHREAD'])
sources = env.Glob('src/*.c')
env.StaticLibrary('hl7c', sources)

SConscript('tests/SConscript', ['env'])
SConscript('contrib/SConscript', ['env'])
