#!/bin/bash

if [ $# -lt 1 ]; then
    echo "usage: $0 [prefix]"
    exit 1
fi

PREFIX=$1

mkdir -p $PREFIX/bin $PREFIX/lib

echo "generate scripts"
cat > bin/monitor << EOF
#!/bin/bash

export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$PREFIX/lib
exec $PREFIX/bin/basic_monitor \$*

EOF

cat > bin/collector << EOF
#!/bin/bash

export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$PREFIX/lib
exec $PREFIX/bin/basic_receiver \$*

EOF


echo "install bin"
install -m 755 bin/monitor $PREFIX/bin
install -m 755 bin/collector $PREFIX/bin
install -m 755 bin/basic_monitor $PREFIX/bin
install -m 755 bin/basic_receiver $PREFIX/bin

echo "install lib"
cp -pdr lib/* $PREFIX/lib
