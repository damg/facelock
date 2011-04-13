env=Environment()
env.Append(LIBS=Split("m"))
env.ParseConfig("pkg-config --libs opencv")

profile = ARGUMENTS.get("profile", "release")
if profile == "debug":
   env.Append(CFLAGS=Split("-g -Wall -Wextra -O0"))
   env.Append(CPPDEFINES=Split("FACELOCK_DBG"))
elif profile == "release":
   env.Append(CFLAGS=Split("-fomit-frame-pointer -O2"))
else:
   print "Invalid profile. Valid profiles: debug release"
   Exit(1) 

SOURCES=Split("facelockd.c")
TARGET="facelockd"

env.Program(source=SOURCES, target=TARGET)
