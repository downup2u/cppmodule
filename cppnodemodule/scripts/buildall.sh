#!/bin/bash

cd ../deps
sudo apt-get install cmake
sudo apt-get install node-gyp
 
echo "unzip files...."
tar xvzf boost_1_57_0.tar.gz
tar xvzf mysql-connector-c-6.1.5-src.tar.gz
tar xvzf soci-3.2.2.tar.gz 

echo "start gererate boost......"
cd boost_1_57_0
chmod +x ./bootstrap.sh
chmod +x ./tools/build/src/engine/build.sh
./bootstrap.sh
echo "start build boost......"
./b2 cxxflags=-fPIC cflags=-fPIC --with-log --with-thread --with-serialization  --with-date_time  --with-system   --with-chrono --with-filesystem --with-regex --address-model=64 variant=release link=static runtime-link=static threading=multi
cp stage/lib/*.* ../../libs/linux

echo "start gererate soci project......"
cd ../mysql-connector-c-6.1.5-src
cmake -G "Unix Makefiles" -DHAVE_GCC_ATOMIC_BUILTINS=1
echo "start build mysql connector......"
make
cp libmysql/*.a ../../libs/linux

echo "start gererate soci project......"
cd ../soci-3.2.2
cmake -G "Unix Makefiles" -DWITH_BOOST=ON -DWITH_MYSQL=ON -DMYSQL_INCLUDE_DIR="../mysql-connector-c-6.1.5-src/include" -DMYSQL_LIBRARIES="../../libs/linux/libmysqlclient.a" -DSOCI_MYSQL=ON -DSOCI_STATIC=ON -DBoost_INCLUDE_DIR="../boost_1_57_0" -DBoost_DATE_TIME_LIBRARY_RELEASE="../../libs/linux/libboost_date_time.a" -DWITH_DB2=OFF -DWITH_FIREBIRD=OFF -DWITH_ODBC=OFF -DWITH_ORACLE=OFF -DWITH_POSTGRESQL=OFF -DWITH_SQLITE3=OFF -DSOCI_EMPTY=OFF -DSOCI_TESTS=OFF
echo "start build soci......"
make
cp lib/*.a ../../libs/linux

echo "start generate proj......"
cd ../../proj/
node-gyp configure
node-gyp rebuild
cp build/Release/*.node ../output/linux

echo "start generate testsuit...."
cd ../testsuit/proj
gyp --depth=.
make