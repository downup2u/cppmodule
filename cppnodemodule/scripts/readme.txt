1、先解压缩：
1.1、boost_1_57_0.tar.gz
1.2、mysql-connector-c-6.1.5-src.tar.gz
1.3、soci-3.2.2.tar.gz 

2、编译BOOST:
2.1、双击bootstrap.bat
2.2、执行：
b2  --with-locale --with-log --with-thread --with-serialization  --with-date_time  --with-system   --with-chrono --with-filesystem --with-regex --address-model=32 variant=release link=static runtime-link=static threading=multi
2.3、拷贝文件：
"stage/lib/*.*" 到 "./../../libs/win/*.*"

3、编译mysql-connector
3.1、执行：cmake -G "Visual Studio 12"
3.2、拷贝文件
"libmysql/*.lib" 到 "../../libs/win"

4、编译soci
4.1、执行：
cmake -G "Visual Studio 12" -DWITH_BOOST=ON -DWITH_MYSQL=ON -DMYSQL_INCLUDE_DIR="../mysql-connector-c-6.1.5-src/include" -DMYSQL_LIBRARIES="../../libs/windebug/libmysqlclient.lib" -DSOCI_MYSQL=ON -DSOCI_STATIC=ON -DBoost_INCLUDE_DIR="../boost_1_57_0" -DWITH_DB2=OFF -DWITH_FIREBIRD=OFF -DWITH_ODBC=OFF -DWITH_ORACLE=OFF -DWITH_POSTGRESQL=OFF -DWITH_SQLITE3=OFF -DSOCI_EMPTY=OFF -DSOCI_TESTS=OFF
4.2、用vc打开Realse模式
4.3、修改编译选项为/MT
4.4、编译生成文件
4.5、拷贝文件copy lib/Release/*.lib ../../libs/win
=============================================================
5、编译Node
5.1、切换到proj目录
5.2、node-gyp configure
5.3、node-gyp rebuild
5.4、拷贝文件copy build/Release/*.node ../output/win

6、编译测试程序
6.1、切换到cd ../testsuit/proj
6.2、执行gyp --depth=.
6.3、编译并执行
