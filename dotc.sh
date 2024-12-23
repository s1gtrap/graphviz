clang dotc.c \
  -I./lib/cdt \
  -I./lib/cgraph \
  -I./lib/common \
  -I./lib/gvc \
  -I./lib/pathplan \
  -L./lib/cdt/.libs \
  -L./lib/cgraph/.libs \
  -L./lib/gvc/.libs \
  -L./lib/pathplan/.libs \
  -L./lib/util/.libs \
  -L./lib/xdot/.libs \
  -lcdt_C \
  -lcgraph_C \
  -lgvc_C \
  -lpathplan_C \
  -lutil_C \
  -lxdot_C \
  -lz
echo $?
