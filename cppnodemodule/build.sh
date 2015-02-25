#!/bin/bash

echo "start generate proj......"
shell_HOME=`pwd`
cd $shell_HOME/proj
node-gyp configure
node-gyp rebuild
cp $shell_HOME/proj/build/Release/*.node $shell_HOME/output/linux 
