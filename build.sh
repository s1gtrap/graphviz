cd lib/cdt
make
cd ../util
make
cd ../cgraph
make
cd ../pack
make
cd ../xdot
make
cd ../common
make
cd ../label
make
cd ../ortho
make
cd ../pathplan
make
cd ../../libltdl
make
cd ../lib/gvc
make
cd ../../plugin/core
make
cd ../../lib/dotgen
make
cd ../../plugin/dot_layout
make
cd ../..
clang -Ilib/common -Ilib/cgraph -Ilib/cdt -Ilib/gvc -Ilib/pathplan -Llib/cgraph/.libs -Llib/gvc/.libs -Llib/pathplan/.libs/ -Llib/xdot/.libs/ -Llib/util/.libs -Llib/cdt/.libs/ -Lplugin/core/.libs -Lplugin/dot_layout/.libs -lcdt_C -lutil_C -lxdot_C -lz -lpathplan_C -lgvc_C -lcgraph -lgvplugin_core_C -lgvplugin_dot_layout_C dot2svg.c
